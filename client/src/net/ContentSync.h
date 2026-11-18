#pragma once

// Sincronización del contenido del cliente con el ConnectServer, en la intro.
//
// Mientras se ve el logo de Webzen el cliente se conecta al ConnectServer, pide
// el manifiesto de los .toml del operador (F4:10/11), baja los que no coinciden
// con su Content/ (F4:12/13), los verifica contra el SHA-256 y los reemplaza.
// Recién después se cargan los assets.  La conexión queda abierta y el select
// server pide la lista por ella.
//
// Sólo por el canal cifrado.  Si no hay ConnectServer, no contesta, contesta en
// claro o se cuelga más de 3 segundos, la intro sigue con lo que hay en disco:
// el select server se comporta como siempre (con el cartel de desconexión si
// el server está caído).

#include <cstdint>

namespace ContentSync {

// Conecta al ConnectServer y arranca.  Sin ConnectServer configurado no hace nada.
void Start();

// Cada frame de la intro.  true cuando terminó (bien, salteada o vencida).
bool Tick();

// Paquetes del ConnectServer.
void OnManifest(const std::uint8_t* msg, int size);   // F4:11
void OnFile(const std::uint8_t* msg, int size);       // F4:13

} // namespace ContentSync
