#include "net/ContentSyncCore.h"

#include <cstring>

namespace ContentSyncCore {

bool IsSafePath(const std::string& path)
{
    static const char kExt[] = ".toml";
    if (path.empty() || path.size() > 255 || path.size() <= sizeof(kExt) - 1) return false;
    if (path.compare(path.size() - (sizeof(kExt) - 1), sizeof(kExt) - 1, kExt) != 0) return false;
    if (path.front() == '/') return false;
    for (char c : path) {
        const unsigned char u = (unsigned char)c;
        if (u < 0x20 || u >= 0x7F || c == '\\' || c == ':') return false;
    }
    // Ningún componente vacío, "." ni "..".
    std::size_t start = 0;
    while (start <= path.size()) {
        const std::size_t slash = path.find('/', start);
        const std::size_t end = slash == std::string::npos ? path.size() : slash;
        const std::string part = path.substr(start, end - start);
        if (part.empty() || part == "." || part == "..") return false;
        if (slash == std::string::npos) break;
        start = slash + 1;
    }
    return true;
}

bool ParseManifest(const std::uint8_t* msg, int size, ManifestPart& out)
{
    // C2 | size[2] | F4 | 11 | result | serial(4) | total(2) | first(2) | count
    const int head = 15;
    if (!msg || size < head || msg[0] != 0xC2) return false;
    if (((msg[1] << 8) | msg[2]) != size || msg[3] != 0xF4 || msg[4] != 0x11) return false;
    out = ManifestPart{};
    out.result = msg[5];
    std::memcpy(&out.serial, msg + 6, 4);
    std::memcpy(&out.total, msg + 10, 2);
    std::memcpy(&out.first, msg + 12, 2);
    const int count = msg[14];
    int at = head;
    for (int i = 0; i < count; ++i) {
        if (at + 1 > size) return false;
        const int length = msg[at++];
        if (at + length + 4 + 32 > size) return false;
        Entry entry;
        entry.path.assign((const char*)msg + at, length);
        at += length;
        std::memcpy(&entry.size, msg + at, 4);
        at += 4;
        std::memcpy(entry.hash, msg + at, 32);
        at += 32;
        out.entries.push_back(std::move(entry));
    }
    return at == size;
}

std::vector<int> Plan(const std::vector<Entry>& entries, const LocalHash& localHash)
{
    std::vector<int> wanted;
    for (std::size_t i = 0; i < entries.size(); ++i) {
        if (!IsSafePath(entries[i].path)) continue;
        std::uint8_t hash[32];
        if (!localHash(entries[i].path, hash) || std::memcmp(hash, entries[i].hash, 32) != 0)
            wanted.push_back((int)i);
    }
    return wanted;
}

} // namespace ContentSyncCore
