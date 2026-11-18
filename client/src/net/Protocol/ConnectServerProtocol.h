// ConnectServerProtocol.h — espejo de los paquetes ConnectServer -> cliente (F4).
//
// Origen: Mu-Linux-0.97k @ 25ceccb,
// Source/MuServer/ConnectServer/ConnectServerProtocol.h. Sin campos View*.
//
// Incluir después de stdafx.h. Todavía no lo usa ningún parseo (Fase 2).
#pragma once

#include "net/Protocol/ProtocolBase.h"

namespace Proto {

// ── C2:F4:02 ─ lista de servers ── ConnectServerProtocol.h:42 / :48 ───────────
// Net_Process.cpp Recv_ServerList (count en +5, entradas de 4 desde +6).
// El comentario del server dice "C1:F4:02", pero el encabezado es PSWMSG_HEAD (C2).
struct PMSG_SERVER_LIST_SEND
{
    PSWMSG_HEAD header;     // C2:F4:02
    std::uint8_t count;
};
static_assert(sizeof(PMSG_SERVER_LIST_SEND) == 6, "F4:02");
static_assert(offsetof(PMSG_SERVER_LIST_SEND, count) == 5, "F4:02 count");

struct PMSG_SERVER_LIST
{
    std::uint16_t ServerCode;
    std::uint8_t UserTotal;
};
static_assert(sizeof(PMSG_SERVER_LIST) == 4, "F4:02 entrada");     // 3 + 1 de padding final
static_assert(offsetof(PMSG_SERVER_LIST, UserTotal) == 2, "F4:02 UserTotal");

// ── C1:F4:03 ─ IP/puerto del GameServer elegido ── ConnectServerProtocol.h:54 ─
// Net_Process.cpp Recv_Redirect (IP en +4, puerto little-endian en +20).
struct PMSG_SERVER_INFO_SEND
{
    PSBMSG_HEAD header;     // C1:F4:03
    char ServerAddress[16];
    std::uint16_t ServerPort;
};
static_assert(sizeof(PMSG_SERVER_INFO_SEND) == 22, "F4:03");
static_assert(offsetof(PMSG_SERVER_INFO_SEND, ServerAddress) == 4, "F4:03 ServerAddress");
static_assert(offsetof(PMSG_SERVER_INFO_SEND, ServerPort) == 20, "F4:03 ServerPort");

// ── C2:F4:04 ─ nombres de los grupos de servers ── ConnectServerProtocol.h:30 / :36
// Net_Process.cpp Recv_CustomServerList (count big-endian en +5/+6, entradas de
// 34 desde +7). El comentario del server dice "C1:F4:01", pero lo manda
// CCCustomServerListSend con header.set(0xF4, 0x04) y es PSWMSG_HEAD (C2).
struct PMSG_CUSTOM_SERVER_LIST_SEND
{
    PSWMSG_HEAD header;     // C2:F4:04
    std::uint8_t count[2];          // [0] = HB, [1] = LB
};
static_assert(sizeof(PMSG_CUSTOM_SERVER_LIST_SEND) == 7, "F4:04");
static_assert(offsetof(PMSG_CUSTOM_SERVER_LIST_SEND, count) == 5, "F4:04 count");

struct PMSG_CUSTOM_SERVER_LIST
{
    std::uint16_t ServerCode;
    char ServerName[32];
};
static_assert(sizeof(PMSG_CUSTOM_SERVER_LIST) == 34, "F4:04 entrada");
static_assert(offsetof(PMSG_CUSTOM_SERVER_LIST, ServerName) == 2, "F4:04 ServerName");

// ── Contenido del cliente (ClientContent del ConnectServer) ─────────────────
// Sólo por el canal cifrado.  Sin relleno entre campos (#pragma pack(1) en los
// dos lados).  El cliente pide el manifiesto de a partes (F4:10 → F4:11) y los
// archivos de a pedazos (F4:12 → F4:13).
#pragma pack(push, 1)

enum ContentResult : std::uint8_t {
    CONTENT_OK = 0,
    CONTENT_NOT_SECURE = 1,     // la conexión no tiene canal cifrado
    CONTENT_CHANGED = 2,        // el manifiesto cambió: volver a pedirlo
    CONTENT_BAD_REQUEST = 3,    // índice u offset fuera de rango
};

struct PMSG_CONTENT_MANIFEST_RECV
{
    PSBMSG_HEAD header;     // C1:F4:10
    std::uint32_t serial;   // 0 = la versión actual
    std::uint16_t first;
};
static_assert(sizeof(PMSG_CONTENT_MANIFEST_RECV) == 10, "F4:10");

// Seguido de `count` entradas: BYTE largo, ruta (relativa a Content/, con '/'),
// DWORD tamaño, BYTE sha256[32].
struct PMSG_CONTENT_MANIFEST_SEND
{
    PSWMSG_HEAD header;     // C2:F4:11
    std::uint8_t result;
    std::uint32_t serial;
    std::uint16_t total;
    std::uint16_t first;
    std::uint8_t count;
};
static_assert(sizeof(PMSG_CONTENT_MANIFEST_SEND) == 15, "F4:11");

struct PMSG_CONTENT_FILE_RECV
{
    PSBMSG_HEAD header;     // C1:F4:12
    std::uint32_t serial;
    std::uint16_t index;
    std::uint32_t offset;
};
static_assert(sizeof(PMSG_CONTENT_FILE_RECV) == 14, "F4:12");

// Seguido de `length` bytes del archivo desde `offset`.
struct PMSG_CONTENT_FILE_SEND
{
    PSWMSG_HEAD header;     // C2:F4:13
    std::uint8_t result;
    std::uint32_t serial;
    std::uint16_t index;
    std::uint32_t offset;
    std::uint32_t size;     // tamaño del archivo entero
    std::uint16_t length;
};
static_assert(sizeof(PMSG_CONTENT_FILE_SEND) == 22, "F4:13");

#pragma pack(pop)

} // namespace Proto
