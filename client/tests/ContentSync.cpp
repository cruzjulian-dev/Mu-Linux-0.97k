// Sincronización del contenido con el ConnectServer: rutas que se aceptan,
// lectura del manifiesto (F4:11) y qué archivos se bajan.

#include "net/ContentSyncCore.h"
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace {

int s_Failures = 0;

void Require(bool ok, const char* what)
{
    if (!ok) {
        std::printf("FALLA: %s\n", what);
        ++s_Failures;
    }
}

using Bytes = std::vector<std::uint8_t>;

void Put(Bytes& b, const void* p, std::size_t n) { b.insert(b.end(), (const std::uint8_t*)p, (const std::uint8_t*)p + n); }

struct Spec { std::string path; std::uint32_t size; std::uint8_t fill; };

Bytes Manifest(std::uint8_t result, std::uint32_t serial, std::uint16_t total, std::uint16_t first, const std::vector<Spec>& entries)
{
    Bytes b = {0xC2, 0, 0, 0xF4, 0x11, result};
    Put(b, &serial, 4);
    Put(b, &total, 2);
    Put(b, &first, 2);
    b.push_back((std::uint8_t)entries.size());
    for (const Spec& e : entries) {
        b.push_back((std::uint8_t)e.path.size());
        Put(b, e.path.data(), e.path.size());
        Put(b, &e.size, 4);
        for (int i = 0; i < 32; ++i) b.push_back(e.fill);
    }
    b[1] = (std::uint8_t)(b.size() >> 8);
    b[2] = (std::uint8_t)(b.size() & 0xFF);
    return b;
}

void Paths()
{
    using ContentSyncCore::IsSafePath;
    Require(IsSafePath("alias.toml"), "ruta simple");
    Require(IsSafePath("item/models.toml"), "con carpeta");
    Require(IsSafePath("monster/12-Yeti/Yeti.toml"), "con guion y números");
    Require(!IsSafePath(""), "vacía");
    Require(!IsSafePath(".toml"), "sólo la extensión");
    Require(!IsSafePath("item/models.bmd"), "otra extensión");
    Require(!IsSafePath("item/models.TOML"), "extensión en mayúsculas");
    Require(!IsSafePath("../main.toml"), "sale de Content");
    Require(!IsSafePath("item/../../x.toml"), ".. en el medio");
    Require(!IsSafePath("./x.toml"), "componente .");
    Require(!IsSafePath("/etc/x.toml"), "absoluta");
    Require(!IsSafePath("C:/x.toml"), "unidad de Windows");
    Require(!IsSafePath("item\\x.toml"), "barra invertida");
    Require(!IsSafePath("item//x.toml"), "componente vacío");
    Require(!IsSafePath(std::string("a\x01.toml", 7)), "carácter de control");
    Require(!IsSafePath(std::string(252, 'a') + ".toml"), "más de 255");
}

void Parse()
{
    using ContentSyncCore::ParseManifest;
    ContentSyncCore::ManifestPart part;
    const Bytes ok = Manifest(0, 7, 3, 1, {{"alias.toml", 120, 0xAB}, {"item/models.toml", 9000, 0xCD}});
    Require(ParseManifest(ok.data(), (int)ok.size(), part), "manifiesto válido");
    Require(part.result == 0 && part.serial == 7 && part.total == 3 && part.first == 1, "encabezado");
    Require(part.entries.size() == 2, "dos entradas");
    Require(part.entries[1].path == "item/models.toml" && part.entries[1].size == 9000 && part.entries[1].hash[31] == 0xCD, "segunda entrada");

    Bytes cut = ok;
    cut.pop_back();
    cut[1] = (std::uint8_t)(cut.size() >> 8);
    cut[2] = (std::uint8_t)(cut.size() & 0xFF);
    Require(!ParseManifest(cut.data(), (int)cut.size(), part), "entrada cortada");

    Bytes extra = ok;
    extra.push_back(0);
    extra[1] = (std::uint8_t)(extra.size() >> 8);
    extra[2] = (std::uint8_t)(extra.size() & 0xFF);
    Require(!ParseManifest(extra.data(), (int)extra.size(), part), "bytes de más");

    Require(!ParseManifest(ok.data(), (int)ok.size() - 1, part), "tamaño distinto al del encabezado");
    Bytes other = ok;
    other[4] = 0x13;
    Require(!ParseManifest(other.data(), (int)other.size(), part), "otro subcódigo");

    const Bytes empty = Manifest(1, 0, 0, 0, {});
    Require(ParseManifest(empty.data(), (int)empty.size(), part) && part.result == 1 && part.entries.empty(), "sin canal: vacío");
}

void Planning()
{
    std::vector<ContentSyncCore::Entry> entries(5);
    const char* paths[] = {"igual.toml", "distinto.toml", "falta.toml", "../afuera.toml", "otro/igual.toml"};
    for (int i = 0; i < 5; ++i) {
        entries[i].path = paths[i];
        std::memset(entries[i].hash, i + 1, 32);
    }
    std::map<std::string, std::uint8_t> local = {{"igual.toml", 1}, {"distinto.toml", 9}, {"../afuera.toml", 0}, {"otro/igual.toml", 5}};
    const auto wanted = ContentSyncCore::Plan(entries, [&](const std::string& path, std::uint8_t hash[32]) {
        const auto found = local.find(path);
        if (found == local.end()) return false;
        std::memset(hash, found->second, 32);
        return true;
    });
    Require(wanted.size() == 2 && wanted[0] == 1 && wanted[1] == 2, "baja sólo el distinto y el que falta; la ruta insegura se ignora");
}

} // namespace

int main()
{
    Paths();
    Parse();
    Planning();
    if (s_Failures == 0) std::printf("ContentSync: OK\n");
    return s_Failures == 0 ? 0 : 1;
}
