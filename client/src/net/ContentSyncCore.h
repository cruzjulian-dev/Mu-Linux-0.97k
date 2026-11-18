#pragma once

// Parte pura de la sincronización de contenido con el ConnectServer: leer el
// manifiesto (F4:11), validar las rutas y decidir qué archivos bajar.  Sin red
// ni disco, para poder probarla aparte (tests/ContentSync.cpp).

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace ContentSyncCore {

struct Entry {
    std::string path;           // relativa a Content/, con '/'
    std::uint32_t size = 0;
    std::uint8_t hash[32] = {};
};

struct ManifestPart {
    std::uint8_t result = 0;
    std::uint32_t serial = 0;
    std::uint16_t total = 0;
    std::uint16_t first = 0;
    std::vector<Entry> entries;
};

// Una ruta que se puede escribir dentro de Content/: relativa, con '/', sin
// "." ni "..", sin ':' ni '\\', sólo caracteres imprimibles y terminada en
// ".toml".  Lo que el server mande fuera de eso no se toca.
bool IsSafePath(const std::string& path);

// Lee un F4:11 entero (desde el encabezado C2).  false si está mal formado.
bool ParseManifest(const std::uint8_t* msg, int size, ManifestPart& out);

// SHA-256 del archivo local; false si no existe o no se pudo leer.
using LocalHash = std::function<bool(const std::string& path, std::uint8_t hash[32])>;

// Índices de las entradas que hay que bajar: las de ruta segura cuyo archivo
// local falta o tiene otro hash.
std::vector<int> Plan(const std::vector<Entry>& entries, const LocalHash& localHash);

} // namespace ContentSyncCore
