#include "stdafx.h"
#include "net/ContentSync.h"
#include "net/ContentSyncCore.h"
#include "net/NetChannel.h"
#include "net/Network.h"
#include "net/Protocol/ProtocolBase.h"
#include "net/Protocol/ConnectServerProtocol.h"
#include "app/util/Sha256.h"
#include "assets/data/AssetList.h"
#include "platform/ContentLayers.h"
#include "platform/ISocket.h"
#include "platform/ITimer.h"
#include "platform/Text.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

extern "C" void DbgLogPublic(const char* msg);
extern bool CreateSocketNoExit(const char* ip, unsigned int port);

namespace ContentSync {

namespace {

namespace fs = std::filesystem;
using ContentSyncCore::Entry;

enum class Stage { Idle, Connecting, Manifest, Files, Done };

const std::uint32_t kConnectTimeout = 3000;   // conectar y terminar el saludo
const std::uint32_t kIdleTimeout = 3000;      // sin respuesta en medio de la descarga
const std::uint32_t kTotalTimeout = 60000;    // tope de toda la sincronización
const int kMaxRestarts = 3;                   // veces que se acepta "el manifiesto cambió"

Stage s_Stage = Stage::Idle;
std::uint32_t s_Start = 0;
std::uint32_t s_LastActivity = 0;
std::uint32_t s_Serial = 0;
int s_Total = 0;
int s_Restarts = 0;
std::vector<Entry> s_Entries;
std::vector<int> s_Wanted;
std::size_t s_Current = 0;          // posición en s_Wanted
std::vector<std::uint8_t> s_Data;   // archivo en curso
int s_Replaced = 0;

std::uint32_t Now() { return Platform::GetTimer().Ticks(); }

void Log(const char* text)
{
    char line[256];
    Platform::TextFormat(line, "ContentSync: %s", text);
    DbgLogPublic(line);
}

void Finish(const char* why)
{
    if (s_Stage == Stage::Done) return;
    s_Stage = Stage::Done;
    char line[192];
    Platform::TextFormat(line, "%s (%d archivos reemplazados, %u ms)", why, s_Replaced, Now() - s_Start);
    Log(line);
    if (s_Replaced > 0) {
        // Lo que ya estaba leído antes de la intro: las capas (alias.toml y los
        // .toml de cada entidad) y la lista de texturas del logo.
        Platform::ContentLayers::Init();
        AssetList_ClearCache();
    }
    s_Entries.clear();
    s_Wanted.clear();
    s_Data.clear();
}

void RequestManifest(std::uint16_t first)
{
    Proto::PMSG_CONTENT_MANIFEST_RECV req{};
    req.header = {0xC1, (std::uint8_t)sizeof(req), 0xF4, 0x10};
    req.serial = first == 0 ? 0 : s_Serial;
    req.first = first;
    NetChannel::Send((const char*)&req, sizeof(req));
    s_LastActivity = Now();
}

void RequestFile()
{
    Proto::PMSG_CONTENT_FILE_RECV req{};
    req.header = {0xC1, (std::uint8_t)sizeof(req), 0xF4, 0x12};
    req.serial = s_Serial;
    req.index = (std::uint16_t)s_Wanted[s_Current];
    req.offset = (std::uint32_t)s_Data.size();
    NetChannel::Send((const char*)&req, sizeof(req));
    s_LastActivity = Now();
}

void Restart()
{
    if (++s_Restarts > kMaxRestarts) {
        Finish("el contenido del server cambia sin parar, se sigue con lo que hay");
        return;
    }
    s_Stage = Stage::Manifest;
    s_Entries.clear();
    s_Wanted.clear();
    s_Data.clear();
    s_Current = 0;
    RequestManifest(0);
}

fs::path LocalPath(const std::string& path)
{
    return fs::path("Content") / fs::u8path(path);
}

bool HashLocal(const std::string& path, std::uint8_t hash[32])
{
    std::ifstream file(LocalPath(path), std::ios::binary);
    if (!file) return false;
    const std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    CSha256 sha;
    sha.Update(data.data(), data.size());
    sha.Final(hash);
    return true;
}

// Escribe en un temporal y lo pone en su lugar: un corte a mitad de camino no
// deja un archivo roto.
bool WriteLocal(const std::string& path, const std::vector<std::uint8_t>& data)
{
    const fs::path target = LocalPath(path);
    fs::path temp = target;
    temp += ".sync";
    std::error_code ec;
    fs::create_directories(target.parent_path(), ec);
    {
        std::ofstream file(temp, std::ios::binary | std::ios::trunc);
        if (!file) return false;
        file.write((const char*)data.data(), (std::streamsize)data.size());
        if (!file) return false;
    }
    fs::rename(temp, target, ec);
    if (ec) {
        fs::remove(temp, ec);
        return false;
    }
    return true;
}

void NextFile()
{
    s_Data.clear();
    if (s_Current >= s_Wanted.size()) {
        Finish("contenido al día");
        return;
    }
    RequestFile();
}

} // namespace

void Start()
{
    s_Stage = Stage::Done;
    s_Replaced = 0;
    s_Restarts = 0;
    s_Start = Now();
    if (!gNetwork.HasConnectServer()) return;
    gNetwork.BeginConnectServerSession();
    if (!CreateSocketNoExit(szServerIpAddress, (unsigned int)g_ServerPort)) {
        Log("no se pudo conectar al ConnectServer, se sigue con lo que hay");
        return;
    }
    s_Stage = Stage::Connecting;
    s_LastActivity = s_Start;
    Log("conectando al ConnectServer");
}

bool Tick()
{
    if (s_Stage == Stage::Done || s_Stage == Stage::Idle) return true;
    const std::uint32_t now = Now();
    if (!Platform::GetSocket().IsOpen(SocketClientSocket)) {
        Finish("la conexión con el ConnectServer se cerró, se sigue con lo que hay");
        return true;
    }
    if (now - s_Start > kTotalTimeout) {
        Finish("la sincronización tardó demasiado, se sigue con lo que hay");
        return true;
    }
    if (s_Stage == Stage::Connecting) {
        if (NetChannel::IsSecure()) {
            s_Stage = Stage::Manifest;
            RequestManifest(0);
        } else if (NetChannel::IsPlain()) {
            Finish("el ConnectServer no tiene canal cifrado, no se sincroniza");
            return true;
        } else if (now - s_Start > kConnectTimeout) {
            Finish("el ConnectServer no contestó, se sigue con lo que hay");
            return true;
        }
        return false;
    }
    if (now - s_LastActivity > kIdleTimeout) {
        Finish("el ConnectServer dejó de contestar, se sigue con lo que hay");
        return true;
    }
    return s_Stage == Stage::Done;
}

void OnManifest(const std::uint8_t* msg, int size)
{
    if (s_Stage != Stage::Manifest) return;
    ContentSyncCore::ManifestPart part;
    if (!ContentSyncCore::ParseManifest(msg, size, part)) {
        Finish("manifiesto mal formado, se sigue con lo que hay");
        return;
    }
    if (part.result == Proto::CONTENT_CHANGED) { Restart(); return; }
    if (part.result != Proto::CONTENT_OK) {
        Finish("el ConnectServer no reparte contenido por esta conexión");
        return;
    }
    if (s_Entries.empty()) {
        s_Serial = part.serial;
        s_Total = part.total;
    } else if (part.serial != s_Serial || part.first != s_Entries.size()) {
        Restart();
        return;
    }
    for (auto& entry : part.entries) s_Entries.push_back(std::move(entry));
    if ((int)s_Entries.size() < s_Total) {
        if (part.entries.empty()) { Finish("manifiesto incompleto, se sigue con lo que hay"); return; }
        RequestManifest((std::uint16_t)s_Entries.size());
        return;
    }
    s_Wanted = ContentSyncCore::Plan(s_Entries, HashLocal);
    char line[96];
    Platform::TextFormat(line, "manifiesto versión %u: %d archivos, %d para bajar", s_Serial, s_Total, (int)s_Wanted.size());
    Log(line);
    s_Stage = Stage::Files;
    s_Current = 0;
    NextFile();
}

void OnFile(const std::uint8_t* msg, int size)
{
    if (s_Stage != Stage::Files) return;
    Proto::PMSG_CONTENT_FILE_SEND head;
    if (!msg || size < (int)sizeof(head)) return;
    std::memcpy(&head, msg, sizeof(head));
    if (head.result == Proto::CONTENT_CHANGED) { Restart(); return; }
    if (head.result != Proto::CONTENT_OK || (int)sizeof(head) + head.length != size
        || head.serial != s_Serial || head.index != s_Wanted[s_Current] || head.offset != s_Data.size()) {
        Finish("respuesta inesperada del ConnectServer, se sigue con lo que hay");
        return;
    }
    const Entry& entry = s_Entries[head.index];
    if (head.size != entry.size || s_Data.size() + head.length > entry.size) {
        Finish("el archivo no coincide con el manifiesto, se sigue con lo que hay");
        return;
    }
    s_Data.insert(s_Data.end(), msg + sizeof(head), msg + sizeof(head) + head.length);
    s_LastActivity = Now();
    if (s_Data.size() < entry.size) {
        if (head.length == 0) { Finish("archivo cortado, se sigue con lo que hay"); return; }
        RequestFile();
        return;
    }
    std::uint8_t hash[32];
    CSha256 sha;
    sha.Update(s_Data.data(), s_Data.size());
    sha.Final(hash);
    char line[320];
    if (std::memcmp(hash, entry.hash, 32) != 0) {
        Platform::TextFormat(line, "%s llegó con otro hash, no se usa", entry.path.c_str());
        Log(line);
    } else if (!WriteLocal(entry.path, s_Data)) {
        Platform::TextFormat(line, "no se pudo escribir Content/%s", entry.path.c_str());
        Log(line);
    } else {
        ++s_Replaced;
        Platform::TextFormat(line, "Content/%s actualizado", entry.path.c_str());
        Log(line);
    }
    ++s_Current;
    NextFile();
}

} // namespace ContentSync
