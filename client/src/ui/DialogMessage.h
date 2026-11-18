#pragma once

// Estados de la máquina de diálogos del 0.97k: los valores que toman
// ErrorMessage (0x083A7C24) y NextErrorMessage (0x083A7C28).
//
// Hay dos familias:
//   * < 100: el código de protocolo que dejó la respuesta del server
//     (CurrentProtocolState).  El login (F1/01) usa 0x15 + resultado, salvo
//     las excepciones anotadas; el cartel muestra el texto de esa respuesta.
//   * >= 100: diálogos de la interfaz.  Coinciden con el enum MESSAGE_* de
//     MU 5.2 (_enum.h) corrido en uno a partir de 111, porque el 0.97k tiene
//     un estado más en ese punto; los nombres siguen los de 5.2 donde aplican.
//
// El número entre paréntesis es el GlobalText que dibuja RenderErrorMessage
// (0x0051AF50) para ese estado.
enum DialogMessage : unsigned int
{
    MESSAGE_NONE                        = 0,

    // ── Respuesta del login (F1/01) ──────────────────────────────────────────
    MESSAGE_LOGIN_WRONG_PASSWORD        = 0x15,  // (407) resultado 0
    MESSAGE_LOGIN_INVALID_ACCOUNT       = 0x16,  // (414) resultado 2
    MESSAGE_LOGIN_ALREADY_CONNECTED     = 0x17,  // (415) resultado 3
    MESSAGE_LOGIN_SERVER_FULL           = 0x18,  // (416) resultado 4
    MESSAGE_LOGIN_BLOCKED               = 0x19,  // (417) resultado 5
    MESSAGE_LOGIN_VERSION               = 0x1A,  // (405/406) resultado 6; cierra el cliente
    MESSAGE_LOGIN_CONNECTION_ERROR      = 0x1B,  // (408) resultado 7 y por defecto
    MESSAGE_LOGIN_TOO_MANY_ATTEMPTS     = 0x1C,  // (409) resultado 8; cierra el cliente
    MESSAGE_LOGIN_SUBSCRIPTION_TERM     = 0x1D,  // (410) resultado 0x0A
    MESSAGE_LOGIN_SUBSCRIPTION_TIME     = 0x1E,  // (411) resultado 0x0B
    MESSAGE_LOGIN_IP_TERM               = 0x1F,  // (412) resultado 0x0C
    MESSAGE_LOGIN_IP_TIME               = 0x20,  // (413) resultado 0x0D
    MESSAGE_LOGIN_POINT_NO_DATES        = 0x22,  // (597) resultado 0xC0/0xD0
    MESSAGE_LOGIN_POINT_NO_POINTS       = 0x23,  // (598) resultado 0xC1/0xD1
    MESSAGE_LOGIN_IP_NOT_ALLOWED        = 0x24,  // (599) resultado 0xC2/0xD2
    MESSAGE_LOGIN_NO_BILLING            = 0x25,  // (433) resultado 9
    MESSAGE_LOGIN_AGE_RESTRICTED        = 0x26,  // (435) resultado 0x11

    // ── Crear / borrar personaje ─────────────────────────────────────────────
    MESSAGE_CHARACTER_INVALID_NAME      = 0x36,  // (394/395)
    MESSAGE_CHARACTER_SLOTS_FULL        = 0x37,  // (396)
    MESSAGE_CHARACTER_DELETE_FAILED     = 0x3A,  // (438/439/401 según el motivo)

    // ── Recuperación de cuenta (F1/03, F1/04, F1/05) ─────────────────────────
    MESSAGE_RECOVERY_INVALID_ACCOUNT    = 0x3F,  // (414) F1/03 resultado 0
    MESSAGE_RECOVERY_INVALID_ACCOUNT_2  = 0x41,  // (414) F1/04 resultado 0
    MESSAGE_RECOVERY_INVALID_ACCOUNT_3  = 0x45,  // (414) F1/05 resultado 0
    MESSAGE_RECOVERY_WRONG_PASSWORD     = 0x47,  // (407) F1/05 resultado 3

    // ── Diálogos de la interfaz ──────────────────────────────────────────────
    MESSAGE_INPUT_ID                    = 100,   // (403)
    MESSAGE_INPUT_PASSWORD              = 101,   // (404)
    MESSAGE_LOG_OUT                     = 110,   // menú de Esc
    MESSAGE_VERSION                     = 112,   // (405/406) cierra el cliente
    MESSAGE_SERVER_LOST                 = 113,   // (402) cierra el cliente
    MESSAGE_DELETE_CHARACTER            = 114,   // (397/398) pide el código personal
    MESSAGE_RESTRICTED_WORDS            = 115,   // (392/393)
    MESSAGE_INPUT_GOLD                  = 116,   // (420/421/422) baúl o trade
    MESSAGE_NOT_ENOUGH_GOLD             = 117,   // (423)
    MESSAGE_OVERFLOW_GOLD               = 118,   // (424) más de 50.000.000
    MESSAGE_GUILD                       = 119,   // invitación a guild
    MESSAGE_PARTY                       = 120,   // (425) invitación a party
    MESSAGE_TRADE                       = 121,   // (419) pedido de trade
    MESSAGE_MIN_LENGTH                  = 122,   // (390)
    MESSAGE_SPECIAL_NAME                = 123,   // (391)
    MESSAGE_DELETE_GUILD                = 126,   // (427/428) expulsar / salir del guild
    MESSAGE_NO_GUILD_MARK               = 127,   // (426)
    MESSAGE_GUILD_WAR                   = 128,   // (430) declaración de guerra
    MESSAGE_DELETE_CHARACTER_WARNING    = 132,   // (399/400) nivel demasiado alto
    MESSAGE_BLOCKED_CHARACTER           = 133,   // (434)
    MESSAGE_STORAGE_PASSWORD_WRONG      = 134,   // (440)
    MESSAGE_STORAGE_ALREADY_LOCKED      = 135,   // (441)
    MESSAGE_STORAGE_PASSWORD_TOO_SIMPLE = 136,   // (442)
    MESSAGE_STORAGE_PASSWORD_NOT_MATCH  = 137,   // (445)
    MESSAGE_STORAGE_RESIDENT_WRONG      = 138,   // (401)
    MESSAGE_CUSTOM                      = 139,   // CreateOkMessageBox (0x0051D6F0)
    MESSAGE_DEVIL_SQUARE_RANK           = 140,   // ranking de Devil Square (0x93)
    MESSAGE_DIALOG_INTERFACE            = 141,   // CreateDialogInterface: texto paginado
    MESSAGE_DIALOG                      = 142,   // diálogo de NPC con respuestas
    MESSAGE_SELECT_CHAOS                = 143,   // (734) tipo de combinación
    MESSAGE_SERVER_IMMIGRATION_RESIDENT = 144,   // (401) 0x99 resultado 0
    MESSAGE_SERVER_IMMIGRATION_DB       = 145,   // 0x99 resultado 1
    MESSAGE_OPTIONS                     = 150,   // opciones
    MESSAGE_ITEM_CHECK                  = 151,   // Sí/No con el item (vender, tirar)
    MESSAGE_LOCK_STORAGE                = 152,   // (443/444) candado del baúl
    MESSAGE_USE_STATE                   = 153,   // Sí/No para usar la fruta o la mascota
    MESSAGE_BLOOD_CASTLE_RESULT         = 154,   // resultado de Blood Castle (0x93, flag 0xFF)
};
