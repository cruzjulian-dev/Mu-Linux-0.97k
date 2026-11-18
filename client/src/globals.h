#pragma once
#include "game/entity/EntityTypes.h"
#include "render/DamagePointTypes.h"
#include "game/item/ItemDefines.h"
#include "scenes/SceneObjectCell.h"
#include "render/EffectPools.h"
#include "render/ParticleTypes.h"
#include "render/JointTypes.h"
struct GATE_ATTRIBUTE;
struct OBJECT;
// globals.h — extern declarations for all Ghidra DAT_ globals.
// Los tipos se infieren del contexto; el prefijo _DAT_ = alias float de la misma dirección.
// NO incluir este header directo — entra vía stdafx.h.

// Reinterpret a DWORD global as float (for globals that have no _DAT_ float alias).
// Uso: Ff(DAT_xxx) para lecturas/escrituras float; Fi(DAT_xxx) para lecturas int.
#ifndef Ff
#define Ff(sym)  (*(float*)&(sym))
#define Fi(sym)  (*(int*)&(sym))
#endif

// ── Low-address constants (x87 FPU/hardware, Ghidra artifacts) ────────────────
// Aparecen cerca de la dirección 0 — se acceden como float vía el alias _DAT_.
extern float  _DAT_00000010;
extern float  _DAT_00000014;
extern float  _DAT_00000018;
extern float  _DAT_0000001c;
extern float  _DAT_00000020;
extern float  _DAT_00000024;

// ── Game math / render constants (0x005524xx – 0x00552dxx) ────────────────────
// Casi todos se acceden como float vía el alias _DAT_; en algunos también hay acceso entero.
extern float  _DAT_005524f4;
extern float  _DAT_00552504;
extern float  _DAT_00552530;
extern float  _DAT_00552540;   // particle dir scale (Matrix_TransformPoint branch)
extern float  _DAT_0055256c;
extern DWORD   DAT_00552580;
extern float  _DAT_005526e4;
extern float  _DAT_00552838;   // 1/480 — Y scale constant
extern float  _DAT_0055283c;   // 1/640 — X scale constant
extern float  _DAT_00552868;
extern float  _DAT_005528b4;
extern float  _DAT_005529c0;   // particle velocity scale constant
extern float  _DAT_005529c8;   // angle-to-radian scale (weather rotation)
extern float  _DAT_00552a00;

// ── Entity / render constants (0x005590xx – 0x00559dxx) ───────────────────────
extern BYTE    PacketXorKey16[16];   // DAT_00559050 — 16-byte XOR key table (Crypto/anti-tamper)
// DAT_00559050 es un alias de compatibilidad (nombre de Ghidra) de PacketXorKey16;
// lo usa Combat/Combat_AttackEffect.cpp.
extern BYTE (&DAT_00559050)[16];
extern float  _DAT_00559070;   // Verlet physics damping/gravity scalar
extern DWORD   g_bUseChatListBox; // IDA: g_bUseChatListBox (0x005590AC)
// Layout de los 3 botones popup del ChatListBox — flt_5590B0/B4/B8 en IDA.
// Leídos del binario: 295.0 / 417.0 / 18.0 (ver comentario en globals.cpp).
extern float   ChatListBox_TabButtonsX;       // DAT_005590b0 — X del primer botón
extern float   ChatListBox_TabButtonsY;       // DAT_005590b4 — Y de los tres
extern float   ChatListBox_TabButtonSpacing;  // DAT_005590b8 — separación horizontal
extern BYTE    Version[5]; // IDA: Version (0x0055961C)
extern BYTE    Serial[16]; // IDA: Serial (0x00559624)
extern DWORD   PacketXorKey3; // IDA: PacketXorKey3 (0x00559678)
extern char    DAT_005597a0;    // PathFinder debug string (first char; take &)
extern DWORD   DAT_005597c4;
extern float  _DAT_005597c8;   // LOD scale: 1.0/1.2/1.4
extern DWORD m_bBlockWhisper;   // IDA: m_bBlockWhisper (0x00559BF0)
extern int    RainSpeed;   // IDA: RainSpeed (0x00559B9C) — wind speed result (fsin * tick, *10)
extern DWORD   MouseUpdateTimeMax;  // IDA: MouseUpdateTimeMax (0x00559BEC) - movement cooldown threshold
// Hover/selection state.  Names follow the 5.2 reference only where the
// 0.97k behaviour and lifetime match; the DAT address is kept for IDA work.
extern int     SelectedItem;       // DAT_00559c48 — item on ground (-1 = none)
extern int     SelectedNpc;        // DAT_00559c4c — NPC/shop entity (-1 = none)
extern int     SelectedCharacter;  // DAT_00559c50 — monster/player (-1 = none)
extern int     SelectedOperate;    // DAT_00559c54 — special world object (-1 = none)
extern int     Attacking; // IDA: Attacking (0x00559C58) -- -1 = no esta atacando. NO es un "hover target"
extern char    m_bAutoAttack;   // IDA: m_bAutoAttack (0x00559C5C)
extern int     DAT_00559c60;   // QKey: tipo de item de la tecla Q (NO arma equipada; F3/30 le suma 448)
extern int     DAT_00559c64;   // WKey: tipo de item de la tecla W
extern int     DAT_00559c68;   // EKey: tipo de item de la tecla E
extern int     DAT_00559c70;   // hover attack target (entity index copy)
extern DWORD   m_dwTextColor;
// 0x00559C7C — IDA `SetTextColor_0` (color del prefijo/guild en sub_47F360).
// Una sola memoria: el alias existe porque parte del port lo nombra DAT_ y
// otra parte SetTextColor_0.  Ver el comentario en globals.cpp.
#ifdef __cplusplus
extern "C" DWORD SetTextColor_0;
#else
extern DWORD SetTextColor_0;
#endif
extern DWORD   SetBackgroundTextColor;   // IDA: DAT_00559c80 (0x00559C80)
extern DWORD   InputEnable; // IDA: DAT_00559c84 (0x00559C84)
extern DWORD   InputNumber;   // IDA: DAT_00559c88 (0x00559C88)
extern "C" int InputTextWidth;   // IDA: InputTextWidth (0x00559C8C)
extern DWORD   InputResidentNumber;   // IDA: InputResidentNumber (0x00559C90)
extern int     _InputTextMaxArr[8];
// 0.97.40: tope en grapheme clusters por slot (0 = sólo el tope en bytes de
// _InputTextMaxArr). ClearInput lo vuelve a 0 en los slots que limpia.
extern int     InputTextMaxClusters[10];
inline void Input_SetLimit(int slot, int bytes, int clusters)
{
    _InputTextMaxArr[slot] = bytes;
    InputTextMaxClusters[slot] = clusters;
}
extern int&    InputTextMax;
extern int&    _DAT_00559c98;
extern DWORD   DAT_00559bf1;   // chat/entity flag (allows type 3 show)
extern DWORD   DAT_00559cc4;   // chat history index (0..4)
extern DWORD   DAT_00559ce0;   // entity table iteration offset
// Alias de los tres globals del panel de evento (ver m_iMatchTime mas abajo).
extern DWORD   g_dwOldSent;   // IDA: g_dwOldSent (0x00559CD8)
extern int     DAT_00559ce8;   // hover target index copy (char-select slot or NPC)
extern char    DAT_00559f5e;   // multi-select item result slot A
extern char    DAT_00559f5f;
extern DWORD   DAT_0055a3e8[4];   // chaos-mix info (ReceiveTalk sub 3)
extern int     EventType;         // EventWindow type (ReceiveTalk sub 4/6)
extern int     g_PartyPanelScratchX, g_PartyPanelScratchY;
extern int     g_GuildCreatorScratchX, g_GuildCreatorScratchY;   // 1 = talk 0x30 enviado, server puede tener Interface.use=1
extern "C" { extern int g_bServerDivisionEnable; extern int g_bServerDivisionAccept; }  // ReceiveTalk sub 5
extern DWORD   DAT_0055a3e4;
extern BYTE    DAT_0055a76c;    // unk_55A76C — gate de la 2da pasada del terreno
                                // (capa de billboards de pasto/arena).  Constante
                                // de .data: vale 1 en el binario.
extern BYTE    DAT_0055a770[3]; // clave XOR de BuxConvert_1 (0x004F6EB0) -- en CERO a proposito, ver globals.cpp
extern DWORD   FrustrumBoundMaxX_1;   // IDA: DAT_0055a774 (0x0055A774)
extern DWORD   FrustrumBoundMaxY_1;   // IDA: DAT_0055a778 (0x0055A778)
extern DWORD   FrustrumBoundMaxX_2;   // IDA: DAT_0055a77c (0x0055A77C)
extern DWORD   FrustrumBoundMaxY_2;   // IDA: DAT_0055a780 (0x0055A780)
extern char    DAT_0055a79c[];  // "Data\\"  path prefix
extern char    DAT_0055a7a4[];  // "Data2\\" path prefix
// IDA: World (0x0055A7AC): gMapManager.GetCurrentMap().
extern float  _DAT_0055a7c0;

// ── Game loop / scene state (0x005615xx – 0x005617xx) ─────────────────────────
// Nota: SceneFlag, g_hWnd y g_hDC se declaran en stdafx.h.
extern float   CameraViewNear;   // IDA: CameraViewNear (0x0056154C)
extern float   CameraViewFar;    // IDA: CameraViewFar (0x00561550)
extern float   CameraFOV;        // IDA: CameraFOV (0x00561554)
extern float   CameraDistanceTarget;   // DAT_005616B4: MoveMainCamera smoothing target
extern float   CameraDistance;         // DAT_083A45D0: current MoveMainCamera distance
extern DWORD   CachTexture;   // IDA: CachTexture (0x00561574), último slot bindeado
extern char   *szServerIpAddress; // IDA: szServerIpAddress (0x005615B8)
extern WORD    g_ServerPort; // IDA: g_ServerPort (0x005615BC)

// ── ConnectServer flow ──────────────────────────────────────────
// SceneFlag (in stdafx.h)
// g_lpszMp3 (IDA 0x005615C4, rutas de los BGM): ver CMapManager (Game/MapManager.cpp).
// Índice del diálogo de quest activo.  Lo escriben CSQuest::ShowDialogText
// (0x4017E7) y sub_51D840, y lo leen sub_401AF0 y UI_InGameMenu.
extern int     g_iCurrentDialogScript;   // IDA: g_iCurrentDialogScript (0x005615DC)
extern DWORD   DAT_005615e0;
extern DWORD   CurrentCameraCount;  // IDA: CurrentCameraCount (0x005615E8) - fade timer
// CameraWalk[7][6] contiguous waypoint table (see globals.cpp for layout).
extern float   CameraWalk_005615ec[42];
extern DWORD&  DAT_005615ec;   // alias → CameraWalk[0]  (wp0.pos.x)
extern DWORD&  DAT_005615f0;   // alias → CameraWalk[1]
extern DWORD&  DAT_005615f4;   // alias → CameraWalk[2]
extern DWORD&  DAT_005615f8;   // alias → CameraWalk[3]
extern DWORD&  DAT_005615fc;   // alias → CameraWalk[4]
extern DWORD&  DAT_00561600;   // alias → CameraWalk[5]
extern DWORD&  DAT_00561664;   // alias → CameraWalk[30] (wp5.pos.x)
extern DWORD&  DAT_00561668;   // alias → CameraWalk[31]
extern DWORD&  DAT_0056166c;   // alias → CameraWalk[32]
extern DWORD&  DAT_00561670;   // alias → CameraWalk[33]
extern DWORD&  DAT_00561674;   // alias → CameraWalk[34]
extern DWORD&  DAT_00561678;   // alias → CameraWalk[35]
extern DWORD   ServerSelectHi;   // IDA: DAT_00561694 (0x00561694)
extern DWORD   ServerSelectLo;   // IDA: DAT_00561698 (0x00561698)
extern DWORD   DAT_005616a0;
extern int     DAT_005616a4;   // dialog Y (signed; ver globals.cpp)
extern int     DAT_005616a8;   // camera X (signed; ver globals.cpp)
extern DWORD   SelectedHero; // IDA: DAT_005616ac (0x005616AC)
extern DWORD   DAT_005616b0;
extern DWORD   TimeRemain; // IDA: TimeRemain (0x005616B8)
extern DWORD   DAT_005617a0;
extern char    DAT_00561a30[];   // "%s" format string used by Scene_CharSelect labels
extern DWORD   DAT_00561b04;
extern char    DAT_00561b70[]; // "OZJ" JPEG extension suffix
extern char    DAT_00561ba8[];  // OpenTGA extension suffix used in non-Data2 path (e.g. ".tga")

// Literales de string (datos de sólo lectura, etiquetas de Ghidra)
extern char    szHealthWarning1[64];   // IDA: 0x00561A3C (sin nombre) — aviso de salud coreano, línea 1; acá vacío
extern char    szHealthWarning2[64];   // IDA: 0x00561A58 (sin nombre) — aviso de salud, línea 2
extern char    szHealthWarning3[64];   // IDA: 0x00561A68 (sin nombre) — aviso de salud, línea 3

// ── Misc game globals (0x00583dxx – 0x00590xxx) ───────────────────────────────
// DAT_00583d8c @ 0x00583D8C ES `g_csQuest` — el PUNTERO al objeto CSQuest, no
// el objeto.  Lo confirman los xrefs: ReceiveQuestHistory / State / Result
// referencian 0x583D8C y el decompile los muestra como `g_csQuest`; el objeto
// es `unk_567500` (= DAT_00567500), que es a donde apunta.
// Todos los consumidores lo usan como puntero (`(uintptr_t)DAT_00583d8c +
// 0x1c87f`), por eso es un alias de g_csQuest y no un objeto propio.
// DAT_00590ac8 = g_EnableSound (sound master flag) — see declaration below in Sound section.
// DAT_00590ac9 = g_Enable3DSound (3D audio flag)    — see declaration below in Sound section.
// DAT_00590acc = SoundLoadCount                     — see declaration below in Sound section.
// DAT_00590ad0 = g_lpDS (IDirectSound*)             — see declaration below in Sound section.
// DAT_00590ad8 = wavefile (CWaveFile* being loaded) — see declaration below in Sound section.
// DAT_00583da8 = g_dwBufferBytes (current WAV data chunk size).

// ── Engine / Window handles (0x055c9xxx – 0x055caxxx) ─────────────────────────
// Nota: g_hWnd y g_hDC viven en CWindow; el DC del texto (DAT_055c9fec) en CFont.
extern char    g_szSavedUsername[12];   // IDA: byte_55C9BA0 (sin nombre) — usuario guardado del login
extern DWORD   DAT_055c9b80;
extern char    ConfigLoginVersion[12]; // IDA: m_ExeVersion — config.ini [LOGIN] Version string
// Contexto de la ofuscación por HashTable — buffer contiguo (el binario original tiene el
// 4 fields physically contiguous at 0x055c9bc8..0x055c9bd4). Callers do
// `&MAIN_HASH_CLASS + 3 * sizeof(void*)` esperando la capacidad; las macros superponen el buffer para que
// holds. See globals.cpp for init.
extern uintptr_t g_HashTableCtx[4];
#define MAIN_HASH_CLASS  (g_HashTableCtx[0])   // vtable pointer
#define DAT_055c9bcc  (g_HashTableCtx[1])   // values array
#define DAT_055c9bd0  (g_HashTableCtx[2])   // keys array
#define DAT_055c9bd4  (g_HashTableCtx[3])   // capacity
extern BYTE*   DAT_055c9be0;
extern float  _DAT_055c9be4;
extern BYTE*   DAT_055c9be8;
extern BYTE*   DAT_055c9bec;
extern DWORD   DAT_055c9bf0;
extern std::uintptr_t DAT_055c9e44; // Reserva de la tabla primaria de atributos de items.
extern std::uintptr_t DAT_055c9e48;
// RandomTable @ 0x055C9E58 — 100 enteros (`rand() % 360`) que siembra WinMain
// (0x41E8A0 L522-525).  Sólo lo usa Entity_Render (0x5038E0) para repartir en
// círculo las monedas del montón de Zen del suelo.
// Tiene que ser el array completo y estar sembrado: si no, las monedas se apilan
// en un mismo punto.
extern int     RandomTable[100];  // IDA: RandomTable (0x055C9E58)
extern BYTE*   DAT_055c9ff0;   // objeto del historial de chat; layout pendiente de B2
extern BYTE*   DAT_055c9ff4;   // objeto de susurros; layout pendiente de B2
extern uintptr_t g_pRenderText;   // IDA: g_pRenderText (0x055C9FF8), objeto CUIRenderText
// DAT_055ca004 = g_hDC — vive en CWindow (gWindow.GetHdc()).
extern DWORD   DAT_055ca018;
extern char    DAT_055ca019;
extern DWORD   g_iNoMouseTime;   // IDA: DAT_055ca028 (0x055CA028)
extern DWORD   DAT_055ca038;
extern DWORD   DAT_055ca03c;
extern DWORD   DAT_055ca040;
// Socket context struct (contiguous buffer, ~0x4030 bytes). Callers of
// CWsctlc_Startup/Net_Connect/Net_Disconnect pass the base address directly
// (originalmente el literal 0x55ca160). Los campos se superponen al buffer:
extern char    SocketClient[0x260000]; // IDA: SocketClient (0x055CA160)
#define SocketClientIsGame  (*(DWORD*)(SocketClient + 4))    // IDA: SocketClient.m_bGame (0x055CA164)
#define SocketClientSocket  (*(SOCKET*)(SocketClient + 8))   // IDA: SocketClient.m_socket (0x055CA168)
#define SocketClientSendBuffer  ((char*)(SocketClient + 0xC))    // IDA: SocketClient.m_SendBuf (0x055CA16C)
// El envío y FD_WRITE deben consultar el mismo contador dentro de CWsctlc.
#define SocketClientSendBufferLength (*(DWORD*)(SocketClient + 0x200C)) // IDA: 0x055CC16C
extern DWORD   SocketClientLogPrint;         // IDA: SocketClient.m_LogPrint (0x055CE174)

// ── Network / login state (0x05826xxx) ────────────────────────────────────────
extern DWORD   SoccerTime; // IDA: DAT_05826c08 (0x05826C08)
                               // evento; lo escriben InitGame y el F3/22.
extern DWORD   DAT_00562e48[4];   // CSimpleModulus XOR-deobfuscate key table
extern DWORD   g_SimpleModulusCS[17]; // IDA: g_SimpleModulusCS (0x05826C10)
extern DWORD   g_SimpleModulusSC[17]; // IDA: g_SimpleModulusSC (0x05826C58)
extern DWORD   m_nTempMyTradeGold; // IDA: m_nTempMyTradeGold (0x05826C9C)
extern DWORD   HeroIndex; // IDA: HeroIndex (0x05826CA0)
extern DWORD   HeroKey; // IDA: HeroKey (0x05826CAC)
extern DWORD   CurrentProtocolState; // IDA: DAT_05826cb0 (0x05826CB0)
extern char    ChatWhisperID[41]; // IDA: ChatWhisperID (0x05826CB4); 0.97.40: 40 bytes + NUL
extern char    LogInID[16]; // IDA: DAT_05826CD4 (0x05826CD4)
extern DWORD   g_bGameServerConnected; // IDA: g_bGameServerConnected (0x05826CF0)
extern DWORD   g_dwLatestMagicTick; // IDA: g_dwLatestMagicTick (0x05826CF4)
extern DWORD   LogIn; // IDA: DAT_05826CF8 (0x05826CF8)
extern DWORD   ChatTime; // IDA: ChatTime (0x05826D08)
// Teleport es 0x05826D14 (ida_xrefs_to: ReceiveTeleport, Attack, CheckGate,
// Skills_PacketHandler, sub_482BE0, sub_4D23B0...).
// DAT_05826d04 es un alias para que Attack escriba el mismo global que leen los
// demás (si no, el gate del Town Portal no se entera del teleport en curso).
extern char    DAT_05826adc[0x100]; // last-sent chat message buffer (rate-limit compare); 0.97.40: hasta 200 bytes
extern char    Teleport; // IDA: Teleport (0x05826D14)
                               // 1=gate/teleport en curso.  La etiqueta vieja
                               // ("NPC script keepalive") era falsa.
extern DWORD   BuyCost; // IDA: BuyCost (0x05826D18)
                               // lo escribe ProtocolCore y lo lee sub_4D23B0.
extern DWORD   EnableUse;      // IDA: EnableUse (0x05826D1C) — cooldown de equipar/usar item;
                               // lo ponen en 10 los usos y lo resetean las respuestas del server
extern DWORD   DAT_05826d20;
extern DWORD   SummonLife; // IDA: DAT_05826d24 (0x05826D24)
// AttackPlayer (IDA @0x05826D28): indice de slot del ULTIMO atacante.  Lo
// escriben ReceiveAction (act 100/101), Skills_PacketHandler y
// ReceiveMagicPosition; lo lee ReceiveAttackDamage para el destello de
// bloqueo (efecto 259).
extern int     AttackPlayer;
// MixState (0x07EAA140): 0 = inactivo, 1 = mezclando (esperando al server),
// 2..50 = animacion de la caja de Chaos, que `RenderItemsBoxes` avanza un paso
// por frame hasta 51.  Ver la nota de HUD_Pass4.cpp.
// Cuenta regresiva de evento (0x07E11D8C / 0x07E11D90).  Definidos en
// Render/HUD_Pass4.cpp; los escribe StartMatchCountDown (opcode 0x92).
extern "C" { extern int m_iMatchCountDownType; extern DWORD m_dwMatchCountDownStart; }
                               // invocada; lo alias-ea HUD_Pass2.cpp.
extern char    SoccerObserver; // IDA: DAT_05826d33 (0x05826D33)
struct PathContext;
extern PathContext* DAT_05826df4;
extern DWORD   FpsWindowStartTimeMs; // IDA: DAT_05826e04
// WorldTime se declara más abajo, con su comentario, en la sección "Entity / animation tick globals"
extern DWORD   FpsTimerInitialized;  // IDA: DAT_05826e0c
extern DWORD   DAT_05826e10;
extern char    DAT_05826e18[200 * 0x10];   // BoneQuaternion scratch (200 huesos x 16B)
// DAT_05828d58 declared below in "Model data table" section

// ── Bone / skeleton data (0x06970xxx) ─────────────────────────────────────────
// Bone-matrix scratch pool (see globals.cpp). All DAT_06970XXX siblings alias
// slots en el mismo buffer, con stride 0x30 entre slots de hueso adyacentes.
// Los offsets de abajo se calculan relativos a 0x06970a9c (la etiqueta de Ghidra que
// fija el slot 0). Cada macro produce un lvalue DWORD, así que `&DAT_06970XXX` devuelve
// un puntero al inicio del slot y se mantiene la semántica vieja de leer/escribir DWORD.
extern char    g_BoneScratch[200 * 0x30];   // MAX_BONES=200 (más chico desborda sobre el preview char)
#define DAT_06970a9c   (*(DWORD*)(g_BoneScratch + 0x000))  // root bone slot
#define DAT_06970acc   (*(DWORD*)(g_BoneScratch + 0x030))
#define DAT_06970afc   (*(DWORD*)(g_BoneScratch + 0x060))
#define DAT_06970b2c   (*(DWORD*)(g_BoneScratch + 0x090))
#define DAT_06970b5c   (*(DWORD*)(g_BoneScratch + 0x0c0))
#define DAT_06970bbc   (*(DWORD*)(g_BoneScratch + 0x120))
#define DAT_06970bec   (*(DWORD*)(g_BoneScratch + 0x150))
#define DAT_06970c1c   (*(DWORD*)(g_BoneScratch + 0x180))
#define DAT_06970c4c   (*(DWORD*)(g_BoneScratch + 0x1b0))
#define DAT_06970c7c   (*(DWORD*)(g_BoneScratch + 0x1e0))
#define DAT_06970d0c   (*(DWORD*)(g_BoneScratch + 0x270))
#define DAT_06970d6c   (*(DWORD*)(g_BoneScratch + 0x2d0))
#define DAT_06970e2c   (*(DWORD*)(g_BoneScratch + 0x390))
#define DAT_06970e8c   (*(DWORD*)(g_BoneScratch + 0x3f0))
#define DAT_06970eec   (*(DWORD*)(g_BoneScratch + 0x450))
#define DAT_0697106c   (*(DWORD*)(g_BoneScratch + 0x5d0))
#define DAT_0697139c   (*(DWORD*)(g_BoneScratch + 0x900))
#define DAT_0697154c   (*(DWORD*)(g_BoneScratch + 0xab0))
#define DAT_0697157c   (*(DWORD*)(g_BoneScratch + 0xae0))
#define DAT_0697160c   (*(DWORD*)(g_BoneScratch + 0xb70))

// ── Preview character entity (0x07abf050) ─────────────────────────────────────
// CharacterView ES el struct de entidad del PREVIEW char de char-select (creado
// por CreateCharacterPointer con model 0xab). Es un entity struct COMPLETO (stride
// 0x394; el original lo espacia 0x580 hasta el array en 0x07abf5d0):
// CreateCharacterPointer escribe hasta +908. Los símbolos _DAT_07abf05c …
// _DAT_07abf5cc son CAMPOS de esta entidad (Ghidra los separó) y son macros que
// proyectan dentro del buffer. Ver globals.cpp.
extern char    CharacterView[0x580];  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf060 (*(float*)(CharacterView + 0x010))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf064 (*(float*)(CharacterView + 0x014))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf068 (*(float*)(CharacterView + 0x018))  // IDA: CharacterView (0x07ABF050)
#define  DAT_07abf0d4 (*(DWORD*)(CharacterView + 0x084))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf138 (*(float*)(CharacterView + 0x0e8))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf13c (*(float*)(CharacterView + 0x0ec))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf140 (*(float*)(CharacterView + 0x0f0))  // IDA: CharacterView (0x07ABF050)
#define  DAT_07abf20c (*(DWORD*)(CharacterView + 0x1bc))  // IDA: CharacterView (0x07ABF050)
#define _DAT_07abf5c4 (*(float*)(CharacterView + 0x574))  // IDA: CharacterView (0x07ABF050) - preview entity field (BodyOrigin x)
#define _DAT_07abf5c8 (*(float*)(CharacterView + 0x578))  // IDA: CharacterView (0x07ABF050) - preview entity field (BodyOrigin y)
#define _DAT_07abf5cc (*(float*)(CharacterView + 0x57c))  // IDA: CharacterView (0x07ABF050) - preview entity field (BodyOrigin z)
extern std::uintptr_t CharactersClient; // IDA: DAT_07abf5d0 (0x07ABF5D0)
extern int     DAT_07abf5d4;   // visible entity counter (reset each frame)
extern char   *Hero; // IDA: DAT_07abf5d8 (0x07ABF5D8)
extern float  _DAT_07abf5e8;   // extra flag (reset 0 each frame)
extern PARTICLE Particles[kParticlePoolSize];   // IDA: unk_7ABF5F0 (0x07ABF5F0), 3000 slots; nombre de MU 5.2

// ── Skill effects pool (0x07b1xxxx – 0x07c7xxxx) ─────────────────────────────
extern unsigned char DAT_07c5ab3c[200 * 0x70];   // SkillEffect pool: 200 slots × 0x70 bytes
// El unk_7C5AB5C de IDA no es una segunda alocación: es el +0x20 dentro del
// pool de efectos 0x7C5AB3C. Weather_Update lo recorre con stride de 0x70 bytes.
// Mantenerlo como global separado desplaza ese recorrido al pool de blur.
// IDA MoveLeaves arranca en 0x7C5AB5C y SkillEffect_Render en 0x7C5AB3C, ambos
// sobre el MISMO pool cuyo slot base es 0x7C5AB30. Como DAT_07c5ab3c[0] es el
// slot base en este arbol, el alias correcto es +0x2C (no +0x20): la diferencia
// 0x7C5AB5C - 0x7C5AB30 = 0x2C. Verificado con dos campos independientes —
// el tipo (i-40 == v0-8) y la posicion (i-28 == v0+4) caen en el mismo lugar.
#define DAT_07c5ab5c (*(DWORD*)(DAT_07c5ab3c + 0x2c))
// RenderCharacter (00456770) captures these bone transforms for action kind
// 0x4D; AttackEffect (00445230) consume los dos primeros en un tick posterior.
extern float   g_AttackEffectMatrix_04D[3][4];
extern float   g_AttackEffectMatrix_04D_Alt[3][4];
extern float   g_AttackEffectMatrix_04D_Aux[3][4];
// Pool de render de joints/trails/blur — ver globals.cpp. La base es el array
// `g_RenderPool_07c608a8`; DAT_07c608b4 es el campo ancla +12 del slot[0].
extern char    g_RenderPool_07c608a8[100 * 0x2f0];
// Dueños de slot: los pools conservan el layout x86, con 4 bytes para el puntero.
extern uintptr_t g_ParticleOwner[kParticlePoolSize];
extern uintptr_t g_TrailOwner[100];
extern uintptr_t g_OperateObjects[200];
#define DAT_07c608b4  (*(DWORD*)(&g_RenderPool_07c608a8[12]))
extern DWORD   RainTarget;  // IDA: RainTarget (0x07C74AE0)
extern DWORD   RainCurrent;  // IDA: RainCurrent (0x07C74AE4) - weather intensity target (interpolated)
extern DWORD   RainAngle;   // IDA: RainAngle (0x07C74AE8) — rain Y offset (driven by fsin)
extern DWORD   RainPosition;  // IDA: RainPosition (0x07C74AEC) - rain position counter (0..2000)

// ── Player render pool ─────────────────────────────────────────────────────────
// El walker v1 de Player_Render arranca en DAT_07c74f54 y lee offsets NEGATIVOS
// hasta -0xEC. El pool REAL es Planes[100] cubriendo todos
// los slots; DAT_07c74f54 es un alias a offset +0xEC (donde v1 vive en cada slot).
extern OBJECT  Planes[100];   // IDA: unk_7C74E68 (0x07C74E68); nombre de MU 5.2
extern DWORD*  DAT_07c74f54;   // = Planes + 0xEC (v1 anchor)

// ── g_CharData XOR-encoded block (0x07cfxxxx) ─────────────────────────────────
extern void   *CharacterAttribute;  // IDA: CharacterAttribute (0x07CF1FF4) - g_CharData sub-pointer (XOR-encoded)
extern void   *CharacterMachine;   // IDA: CharacterMachine (0x07CF1FFC), buffer de 0x584 bytes

// ── Player input / targeting state (0x07e01xxx – 0x07e11xxx) ─────────────────
extern DWORD   TargetX;   // target grid X (from ray cast or hover click)  [IDA 0x07E016C0]
extern DWORD   TargetY;   // target grid Y  [IDA 0x07E016C4]
// 0x07E109C8 es `ItemKey` (confirmado por xrefs de IDA: lo escribe
// Player_InputTick al clickear un item del suelo y lo leen los 3 sitios de
// `Action` que arman el pickup).  NO es un indice de NPC — esa etiqueta
// vieja hizo que el camino de llegada releyera SelectedItem en vivo.
extern DWORD   ItemKey;
extern DWORD   DAT_07db8708;   // hover target entity type (from entity+2)
extern char    DAT_07e113e4[5 * 256];   // historial de chat: 5 entradas x 256B

// ── UI / HUD data (0x07e1xxxx – 0x07eaxxxx) ──────────────────────────────────
extern DWORD  _DAT_07e118e4;   // player facing angle (float, sent in movement packets)
extern int     HeroTile;         // IDA: HeroTile (0x07E118E8)
extern char    GuildInputEnable;   // char-select init flag  [IDA 0x07E11D70]
extern char TabInputEnable;   // IDA: TabInputEnable (0x07E11D71)
extern char    GoldInputEnable;   // char-select flag C  [IDA 0x07E11D72]
extern int     InputGold;   // gold / currency (checked vs 0x2faf081)  [IDA 0x07E11D74]
extern DWORD   InputIndex;   // InputIndex — indice del campo de input activo (lo rota el Tab)
extern DWORD   MacroTime;  // IDA: MacroTime (0x07E11D7C) - cooldown de las macros de chat
extern DWORD   LoadingWorld;  // IDA: LoadingWorld (0x07E11D1C) - per-frame cooldown tick counter
extern DWORD   MouseUpdateTime;  // IDA: MouseUpdateTime (0x07E11D28) - movement debounce step counter
extern DWORD   EditFlag;  // IDA: EditFlag (0x07E11D30) - frame counter (anti-tamper)
extern DWORD   DontMove;   // IDA: DontMove (0x07E11D64) — terrain walk flag (0=walkable 1=blocked)
extern float  _DAT_07e11d4c;   // facing tick reference (last frame time)
extern float  _DAT_07e11d50;   // facing dt (delta * speed scale)
// IDA: InputTextHide (0x07E113D8): por campo de input, 1 = se dibuja con '*'
// (el [1] es el password del login).  El port tenía además una copia
// `InputTextHide[10]` en HUD_Pass4: el login escribía una y el render de
// HUD_Pass4 leía la otra.
extern BYTE    InputTextHide[40];
extern DWORD   g_dwOldConv;   // IDA: g_dwOldConv (0x07E11D94)
extern DWORD   DAT_07e11d98;
extern DWORD   NoticeInverse;   // IDA: NoticeInverse (0x07E11DA0)
extern DWORD   DebugTextCount;   // IDA: DebugTextCount (0x07E11DB4)
extern DWORD   DAT_07e11db8;   // movement step count (must be >0x27 for facing packet)
extern int     DAT_07e11dbc;   // last facing tick time
extern char    DAT_07e11dc0;   // movement lock flag B
extern DWORD   DAT_07e11dc4;   // NPC script: dialog-active flag
extern DWORD   DAT_07e11dc8;   // NPC script: keepalive timer (GetTickCount at last send)
extern DWORD   TotalPacketSize;   // IDA: DAT_07e11dcc (0x07E11DCC)
extern DWORD   DAT_07e11e78;   // target entity index (active selection)
// 0x07E11E9C = Party + 0x1C (indice de entidad del slot 0). Era un char suelto.
#define DAT_07e11e9c (*(char*)(Party + 0x1C))
extern char    DAT_07e11d6e;            // flag reset each login frame
extern int     MouseOnWindow;           // IDA: MouseOnWindow (0x07D78094) — cursor sobre una ventana de UI
extern int     InputLength[10];          // IDA: InputLength (0x07D780A8) — largo de cada campo de InputText
// InputLength[1] — el largo del campo 1 (destino del susurro).
//
// No es un DWORD aparte: es `InputLength[1]`. En el binario las dos cosas son la misma
// memoria (0x07D780A8 es InputLength[10] y 0x07D780AC es su elemento 1), y los
// sitios que escriben el largo del susurro (historial de flechas, tab-complete,
// click derecho sobre un jugador, cierre del menu) tienen que llegar al array.
// El buffer companero (DAT_07db8810) es igualmente un alias del slot 1.
#define DAT_07d780ac   (*(DWORD*)&InputLength[1])
// IDA: InputText[10][256] @ 0x07db8710. Slot 0 = username/chat, slot 1 =
// contraseña/destino-del-susurro. DAT_07db8810 es el alias +0x100 del slot 1 que usan
// los caminos de login/susurro; el mismo storage respalda a RenderInputText.
extern char    InputText[10][256];    // input slot table (10 × 256 bytes)
#define DAT_07db8810   (InputText[1]) // alias: +0x100 = slot 1
// extern char    DAT_07d4c3ec[256];       // connecting status string buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4c644[256];       // char name send buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4c770[256];       // char name confirm buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d530e8[256];       // server group name buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d53214[256];       // server channel name buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4ac7c[256];       // username display buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4ada8[256];       // password display buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4b708[128];       // char name format string   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4b12c[128];       // version string 1   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4b258[128];       // version string 2   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d4b384[128];       // version string 3 format   // -> alias a GlobalText, ver el final del archivo
constexpr int GROUND_ITEM_STRIDE = 0x48 + OBJECT_STRIDE;  // cabecera + OBJECT
extern unsigned char Items[1000 * GROUND_ITEM_STRIDE];   // GroundItem pool: 1000 × 0x204 bytes
extern char    pPickedItem[0x44];       // IDA: pPickedItem (0x07E91350), el ITEM que lleva el cursor
// IDA: pPickedItem.Key (0x07E91388), "hay un item en el cursor".  En el
// port es una variable aparte del campo Key de pPickedItem: el pickup con
// click derecho y el auto-equipado de flechas copian el item con su Key
// puesto y la bandera se prende recien cuando el pickup se concreta.
extern "C" DWORD   pPickedItemKey;
extern byte    DAT_07e9138e;   // UI grid selected column (byte, Item_ReturnPickedItem)
extern byte    DAT_07e9138f;   // UI grid selected row (byte, Item_ReturnPickedItem)
extern short   DAT_07e91394[10];   // IDA: word_7E91394
extern char    DAT_07eaa1a4;       // IDA: byte_7EAA1A4
extern DWORD   DAT_07e913a8;
extern DWORD   DAT_07e91428;
extern int GuildListStartY;   // IDA: GuildListStartY (0x07E91784)
extern int GuildListStartX;   // IDA: GuildListStartX (0x07E91788)
// Tabla de nombres stride 80 (guild members / buffs). NO es un DWORD: los
// consumidores (HUD_Pass2, HUD_Pass3, SecondPassword, SMD_Parser y el
// handler 0x52 de Net_Process) la recorren con `&DAT_07e919bc + N*80`.
extern char    DAT_07e919bc[0x13C30];
// Pools de índices de nombres de personaje — cada uno es un buffer de 0x880 bytes (32 entradas × 0x44).
// Lo recorre CloseInventoryRelatedWindows (CharPreview_Reset) con stride 0x44.
// DAT_07ea5298: alias de Inventory (ver el bloque de alias de pools mas abajo).
extern DWORD   DAT_07ea5b18;
// unk_7EA9504 / unk_7EA9328 NO son un buffer aparte: son dos posiciones dentro
// del inventario real (OffsetInventoryItems @ 0x07EA8410).  Ver la derivación
// completa en globals.cpp, junto a la definición de los punteros.
extern int    *p_DAT_07ea9504_;   // &OffsetInventoryItems[63*0x44+56] (slot 63, Key)
extern int    *p_DAT_07ea9328_;   // &OffsetInventoryItems[56*0x44+56] (slot 56, Key)
#define DAT_07ea9504  (*p_DAT_07ea9504_)
#define DAT_07ea9328  (*p_DAT_07ea9328_)
extern BYTE*   DAT_07ea9800;
extern BYTE*   g_ItemMoveSourcePool;
extern BYTE*   g_ItemMoveTargetPool;
extern DWORD   DAT_07ea9804;   // NPC shop context A
extern DWORD   DAT_07ea9808;   // NPC shop context B
extern DWORD   DAT_07ea980c;   // NPC shop context C
extern DWORD   DAT_07ea9810;
extern char    DAT_07ea9814[16];   // IDA: dword_7EA9814 — texto tipeado en el teclado del PIN
// Alias dentro de ese buffer (el binario los aborda como globales sueltos):
#define DAT_07ea9818   (*(unsigned int*)&DAT_07ea9814[4])
#define DAT_07ea981c   (*(unsigned short*)&DAT_07ea9814[8])
#define DAT_07ea981e   (DAT_07ea9814[10])
#define DAT_07ea981f   (*(unsigned int*)&DAT_07ea9814[11])   // copia del PIN de la 1ra pasada

extern int CharacterInfoStartX;   // IDA: CharacterInfoStartX (0x07EA982C)
extern int CharacterInfoStartY;   // IDA: CharacterInfoStartY (0x07EA9830)
extern char    DAT_07ea9834[41];    // IDA: nombre remoto de Trade (0.97.40: 40 bytes del paquete + NUL)
extern "C" int g_nGuildMemberCount;     // IDA: g_nGuildMemberCount (0x07EAA0D0)
extern DWORD   GuildPlayerKey;  // IDA: GuildPlayerKey (0x07EAA0D8)
// 0x07EAA0E0 = PartyNumber. Era una copia que nadie escribia.
#define DAT_07eaa0e0 (*(DWORD*)&PartyNumber)
extern DWORD   m_nYourTradeGold;  // IDA: m_nYourTradeGold (0x07EAA0F0)
extern DWORD   m_nMyTradeGold;  // IDA: m_nMyTradeGold (0x07EAA0F4)
extern int     DAT_07eaa0f8;   // CharData_RecalcDurability accumulator (count of items needing repair)
extern DWORD   m_bYourConfirm;   // IDA: DAT_07eaa0fc (0x07EAA0FC)
extern char    m_bMyConfirm;   // IDA: DAT_07eaa0fd (0x07EAA0FD)
extern int     TradeYourWait;       // IDA: m_nYourTradeWait
extern int     TradeMyWait;         // IDA: m_nMyTradeWait
extern DWORD   TradeRemoteGuildKey; // IDA: DAT_00559F54 durante Trade
extern WORD    TradeRemoteLevel;    // IDA: DAT_07EAA0EC durante Trade
extern DWORD   m_nMyTradeWait;   // shop/item selection A  [IDA 0x07EAA104]
extern DWORD   StorageGoldFlag;   // shop/item selection B  [IDA 0x07EAA108]
extern char    GuildOpened;  // IDA: GuildOpened (0x07EAA114)
extern char    PartyOpened;  // IDA: PartyOpened (0x07EAA115)
extern char    CharacterOpened;  // IDA: CharacterOpened (0x07EAA116)
extern char    InventoryOpened;  // IDA: InventoryOpened (0x07EAA117)
extern char    ShopOpened;  // IDA: ShopOpened (0x07EAA118)
extern char    WarehouseOpened;  // IDA: WarehouseOpened (0x07EAA119)
extern char    ChaosMixOpened;  // IDA: ChaosMixOpened (0x07EAA11A)
extern char    TradeOpened;  // IDA: TradeOpened (0x07EAA11B)
extern char    EventWindowOpened;  // IDA: EventWindowOpened (0x07EAA11C)
extern DWORD   GuildCreatorOpened;  // IDA: GuildCreatorOpened (0x07EAA124)
extern "C" DWORD   GoldenArcherOpenType;   // IDA: DAT_07eaa128 (0x07EAA128)
extern "C" DWORD   DAT_07eaa13c;
extern DWORD   MixType;   // guild join/select current selection index  [IDA 0x07EAA16C]
extern DWORD   g_iKeyPadEnable;  // IDA: g_iKeyPadEnable (0x07EAA144)
extern DWORD   DAT_07eaa148;   // shop/item selection C
extern DWORD   DAT_07eaa14c;
extern DWORD   DAT_07eaa150;
extern DWORD   TextNum;  // IDA: TextNum (0x07EAA154)
extern uintptr_t CheckInventory;  // IDA: CheckInventory (0x07EAA160)
extern char    EquipmentItem;  // IDA: EquipmentItem (0x07EAA165) - char-select packet-sent flag
extern DWORD   DAT_07eaa168;   // TextureEnable (GL texture state cache)
extern char    DAT_07eaa179;
extern char    DAT_07eaa190;   // inventory drop error message ID string
// Frustum world corners: 5 corners × 3 floats = 15 DWORDs at 0x07eab1b0
extern float   FrustrumVertex;   // corner 0 X (apex)  [IDA 0x07EAB1B0]
extern float   DAT_07eab1b4;   // corner 0 Y
extern float   DAT_07eab1b8;   // corner 0 Z
extern float   DAT_07eab1bc;   // corner 1 X (top-left)
extern float   DAT_07eab1c0;   // corner 1 Y
extern float   DAT_07eab1c4;   // corner 1 Z
extern float   DAT_07eab1c8;   // corner 2 X (top-right)
extern float   DAT_07eab1cc;   // corner 2 Y
extern float   DAT_07eab1d0;   // corner 2 Z
extern float   DAT_07eab1d4;   // corner 3 X (bot-right)
extern float   DAT_07eab1d8;   // corner 3 Y
extern float   DAT_07eab1dc;   // corner 3 Z
extern float   DAT_07eab1e0;   // corner 4 X (bot-left)
extern float   DAT_07eab1e4;   // corner 4 Y
extern float   DAT_07eab1e8;   // corner 4 Z
extern DWORD   TerrainIndex1;  // IDA: TerrainIndex1 (0x07EAB1EC)
extern DWORD   TerrainIndex2;  // IDA: TerrainIndex2 (0x07EAB1F0)
extern DWORD   TerrainIndex3;  // IDA: TerrainIndex3 (0x07EAB1F4)
extern DWORD   TerrainIndex4;  // IDA: TerrainIndex4 (0x07EAB1F8)
extern float   TerrainGrassWind[256 * 256];  // IDA: TerrainGrassWind (0x07EAB200) - water-wave heights (row*256+col floats)
                               // DAT_081cb608 @0x081CB608). Global muerto, sin
                               // usos — no reintroducirlo.
extern DWORD   FrustrumFaceD;   // IDA: DAT_07eeb200 (0x07EEB200)
extern DWORD   DAT_07eeb204;
extern DWORD   DAT_07eeb208;
extern DWORD   DAT_07eeb20c;
extern DWORD   DAT_07eeb210;
extern float   WaterMove;      // IDA: WaterMove (0x07EEB214) — WaterMove — terrain water UV scroll offset (RenderTerrain)
extern float   FrustrumY[4];   // frustum quad Y[4] (per TestFrustrum2D)  [IDA 0x07EEB218]
extern float   FrustrumX[4];   // frustum quad X[4]  [IDA 0x07EEB228]
extern float   TerrainLight[256 * 256 * 3];   // IDA: TerrainLight (0x07EEB238) — RGB ambiente por tile
extern float   g_TerrainTexCoord[8];   // TerrainTextureCoord[4][2] — UVs per tile face (RenderTerrainFace/FaceTexture)
// Tile pick corners buffer (12 floats contiguous, 4 vec3 corners)
extern float   g_TilePickBuf[12];
#define DAT_07feb258  (g_TilePickBuf[0])
#define DAT_07feb25c  (g_TilePickBuf[1])
#define DAT_07feb260  (g_TilePickBuf[2])
#define _DAT_07feb264 (g_TilePickBuf[3])
#define _DAT_07feb268 (g_TilePickBuf[4])
#define _DAT_07feb26c (g_TilePickBuf[5])
#define _DAT_07feb270 (g_TilePickBuf[6])
#define _DAT_07feb274 (g_TilePickBuf[7])
#define _DAT_07feb278 (g_TilePickBuf[8])
#define _DAT_07feb27c (g_TilePickBuf[9])
#define _DAT_07feb280 (g_TilePickBuf[10])
#define _DAT_07feb284 (g_TilePickBuf[11])
extern float   DAT_07feb288[256 * 256 * 3];   // TerrainNormal[256*256][3] vertex normals

// ── Large game data arrays (0x080xxxxx – 0x083bxxxx) ─────────────────────────
extern DWORD   SelectXF;  // IDA: SelectXF (0x080AB288) - cursor screen X (float)
extern DWORD   SelectYF;   // IDA: SelectYF (0x080AB28C) — cursor screen Y (float)
extern unsigned char  TerrainMappingLayer2[0x10000];   // TileTex2[256*256] tile texture index 2  [IDA 0x080AB2B4]
extern unsigned char  TerrainMappingLayer1[0x10000];   // TileTex1[256*256] tile texture index 1  [IDA 0x080BB2B4]
extern float   BackTerrainHeight[0x10000];  // IDA: BackTerrainHeight (0x080CB2CC) - [256*256] tile height
extern float   TerrainGrassTexture[0x10000];          // IDA: TerrainGrassTexture (0x0810B2CC) — TerrainNoise[256*256] random noise init
extern DWORD   CurrentLayer;   // IDA: CurrentLayer (0x0814B2DC)
// Ambient terrain-object table — sub_4F7060 (Terrain_SpawnAmbientObjects) walks 100 records de
// stride 8 leyendo campos en [-5..+1].  Backing zeroado con prefijo de 5 bytes
// para que las lecturas [-5] queden in-bounds; vacío en nuestro build (el loader
// de objetos .obj no está wireado) → el loop no spawnea nada.
// &DAT_081cb2ed == &g_TerrainObjTable[5].
extern BYTE    g_TerrainObjTable[0x328];
#define DAT_081cb2ed  (g_TerrainObjTable[5])
// Buffer de iluminación por tile, vivo — dimensionado para 256×256 tiles × 3
// floats. Las macros de abajo proyectan DAT_081cb60c/610 en el 2do/3er DWORD del
// slot, como el layout contiguo del binario original.
extern float   DAT_081cb608[256 * 256 * 3];
extern float   BackTerrainLight[256 * 256 * 3];   // IDA: BackTerrainLight (0x0828B608) - TerrainLightData[256*256][3] post-normal lighting
// cb60c/cb610 son el 2do/3er DWORD del slot 0 de cb608 (y análogamente para
// 0828b608). El código los accede como `(&DAT_*60c)[iVar2*3]` (lee el 2do campo
// del slot iVar2) o `(char*)&DAT_*60c + iVar2*12` (escribe en el 2do campo).
#define DAT_081cb60c   (*(DWORD*)((char*)&DAT_081cb608[0] + 4))
#define DAT_081cb610   (*(DWORD*)((char*)&DAT_081cb608[0] + 8))
#define DAT_0828b60c   (*(DWORD*)((char*)&BackTerrainLight[0] + 4))
#define DAT_0828b610   (*(DWORD*)((char*)&BackTerrainLight[0] + 8))
extern float   TerrainMappingAlpha[0x10000];          // TerrainMappingAlpha[256*256] — blend factor layer1↔layer2 (IDA/DLL; NO es height)  [IDA 0x0834B608]
extern float   FrustrumFaceNormal;  // frustum plane 0 normal X  [IDA 0x0838B7C4]
extern float   DAT_0838b7c8;  // frustum plane 0 normal Y
extern float   DAT_0838b7cc;  // frustum plane 0 normal Z
extern float   DAT_0838b7d0;  // frustum plane 1 normal X
extern float   DAT_0838b7d4;  // frustum plane 1 normal Y
extern float   DAT_0838b7d8;  // frustum plane 1 normal Z
extern float   DAT_0838b7dc;  // frustum plane 2 normal X
extern float   DAT_0838b7e0;  // frustum plane 2 normal Y
extern float   DAT_0838b7e4;  // frustum plane 2 normal Z
extern float   DAT_0838b7e8;  // frustum plane 3 normal X
extern float   DAT_0838b7ec;  // frustum plane 3 normal Y
extern float   DAT_0838b7f0;  // frustum plane 3 normal Z
extern float   DAT_0838b7f4;  // frustum plane 4 (near) normal X
extern float   DAT_0838b7f8;  // frustum plane 4 (near) normal Y
extern float   DAT_0838b7fc;  // frustum plane 4 (near) normal Z
extern unsigned char DAT_0838b800[1080];   // BMPHeader (BITMAPFILEHEADER + DIB + palette)
extern DWORD   TerrainFlag;  // IDA: TerrainFlag (0x0838BC44) - (RenderTerrain pass selector: 0=base, 2=alpha overlay)
extern DWORD   DAT_0839bc88;    // terrain-light double-buffer toggle (RenderTerrain ^=1 cada frame)
extern DWORD   DAT_0839bc86;
extern DWORD   WaterTextureNumber;  // IDA: WaterTextureNumber (0x0839BC8C)
extern DWORD   FrustrumBoundMinX_1;   // IDA: DAT_0839bc90 (0x0839BC90)
extern DWORD   FrustrumBoundMinY_1;   // IDA: DAT_0839bc94 (0x0839BC94)
extern DWORD   FrustrumBoundMinX_2;   // IDA: DAT_0839bc98 (0x0839BC98)
extern DWORD   FrustrumBoundMinY_2;   // IDA: DAT_0839bc9c (0x0839BC9C)

// ── Scene / state machine vars (0x083a0xxx – 0x083bxxxx) ─────────────────────
extern char    DAT_083a2370[0x960];   // Entity render-state pool (stride 0xc, 128 slots, 0x960 bytes)
#define DAT_083a2cd0 DAT_083a2370[0x960-1]  // end-marker alias
extern OBJECT  Fishs[10];   // IDA: unk_83A2E90 (0x083A2E90); nombre de MU 5.2
// `Operates` de IDA = 0x083A2378 = DAT_083a2370 + 8, o sea el campo [2] (puntero
// al objeto) de la ENTRADA 0 de la lista de objetos interactuables.  Los
// consumidores lo indexan `Operates[3 * SelectedOperate]`, que salta de entrada
// en entrada (3 DWORDs = los 12 bytes de stride).
//
// No es un DWORD suelto: `((int*)&DAT_083a2378)[i*3]` tiene que caer dentro de
// la lista (con un global aparte, SelectedOperate == 0 da `*(short*)(0 + 2)` →
// crash).
extern float  EarthQuake;
// Grilla 16x16. El contrato x86 conserva los 0x1000 bytes del binario.
// Los consumidores usan celdas y punteros nativos, nunca strides de DWORD.
extern SceneObjectCell g_ObjectBucketGrid[256];
extern DWORD   EnableEvent;  // IDA: EnableEvent (0x083A3FF0)
extern char    DAT_083a410c;   // Scene_Intro: normal-path flag
// DAT_083a4110_arr — 3-DWORD storage para mouse-ray endpoint (Camera_MouseRay
// escribe out_ray[0..2]). DAT_083a4110 es referencia al [0].
extern DWORD   DAT_083a4110_arr[3];
extern DWORD&  DAT_083a4110;
extern DWORD   MouseLButtonPush;  // IDA: MouseLButtonPush (0x083A4124) - MouseLButtonPush flag — VALORES VÁLIDOS: {0,1}
// Helper de defensa: usar IsClickPushed() en vez de truthy-check `&& MouseLButtonPush`.
// Algún stub stompea MouseLButtonPush con un valor wild (0x4397355A observado) que
// pasaría como verdadero y dispara phantom clicks. La igualdad estricta == 1
// suprime el síntoma mientras buscamos al culpable.
static inline bool IsClickPushed() { return MouseLButtonPush == 1; }  // IDA: MouseLButtonPush (0x083A4124)
extern float   DAT_083a4130;   // cursor billboard screen X
extern float   DAT_083a4134;   // cursor billboard screen Y
extern float   DAT_083a4138;   // terrain pick Z result
extern DWORD   MouseLButtonPop;  // IDA: MouseLButtonPop (0x083A413C)
// CameraMatrix es un buffer de 48 bytes (matrix 3×4 floats) — ver globals.cpp.
// DAT_083a414c / DAT_083a415c / DAT_083a416c son referencias a índices 3/7/11
// del mismo array; comparten memoria para que GL_GetModelViewMatrix (que escribe 12
// DWORDs) no desborde sobre globales vecinos.
extern DWORD   CameraMatrix[12];   // IDA: DAT_083a4140 (0x083A4140)
extern char    GrabFileName[0x104];   // screenshot name buf (26 chars+)  [IDA 0x083A4174]
extern DWORD   MouseY;  // IDA: MouseY (0x083A4278)
extern DWORD   MouseX;  // IDA: MouseX (0x083A427C)
extern DWORD   OpenglWindowWidth;   // viewport width (pixels)  [IDA 0x083A4280]
// CameraRayOriginX_arr — 3-DWORD storage para camera world position (escrito por
// Camera_MouseRay vía Vector_InverseRotate(...&CameraRayOriginX)). _CameraRayOriginX..428c son
// aliases float al mismo storage.
extern DWORD   CameraRayOriginX_arr[3];
extern DWORD&  CameraRayOriginX;       // DAT_083A4284
extern DWORD&  CameraRayOriginY;       // DAT_083A4288
extern DWORD&  CameraRayOriginZ;       // DAT_083A428C
extern char    DAT_083a4299;
extern DWORD   ViewportCenterX;        // DAT_083A429C
extern DWORD   ViewportCenterY;        // DAT_083A42A0
extern DWORD   MouseRButton;  // IDA: MouseRButton (0x083A42AC) - UI flag: mouse button state
extern DWORD   OpenglWindowHeight;   // viewport height (pixels)  [IDA 0x083A42B0]
// CameraAngle/CameraPosition contiguous — DAT_ names are aliases (see globals.cpp)
extern DWORD&  DAT_083a42b8;   // alias → CameraAngle[0]
extern DWORD&  DAT_083a42bc;   // alias → CameraAngle[1]
extern DWORD&  DAT_083a42c0;   // alias → CameraAngle[2]
extern DWORD   MouseLButton;  // IDA: MouseLButton (0x083A42C4)
extern DWORD   OpenglWindowX;   // viewport x offset  [IDA 0x083A42C8]
extern DWORD   OpenglWindowY;   // viewport y offset  [IDA 0x083A42CC]
extern DWORD   MouseRButtonPush;   // UI flag: right-click / context menu  [IDA 0x083A42D0]
extern float&  _DAT_083a42d4;  // alias → CameraPosition[0]
extern float&  _DAT_083a42d8;  // alias → CameraPosition[1]
extern DWORD&  DAT_083a42d8;   // alias → CameraPosition[1] (DWORD view)
extern float&  _DAT_083a42dc;  // alias → CameraPosition[2]
extern char    CameraTopViewEnabled;   // DAT_083A42E9: top-view / force-cull-bypass flag
extern char    FogEnable;  // IDA: FogEnable (0x083A42EA) - reset to 0 each frame in Scene_Loading
extern char    DAT_083a42eb;   // auto-drop trigger flag (inventory)
extern DWORD   GrabEnable;    // IDA: GrabEnable (0x083A42EC), captura de pantalla
// DAT_083a42fc  — macro alias dentro de DAT_083a42f8 (ver bloque de dialog button rects)
extern DWORD   DAT_083a4320;
// DAT_083a4324 @ 0x083A4324 ES `g_iNumLineMessageBoxCustom` — la cantidad de
// lineas que SeparateTextIntoLines dejo en el cuadro de dialogo.
// Confirmado en el disassembly de CSQuest::ShowDialogText 0x401806:
// `a3 24 43 3a 08` = `mov [0x083A4324], eax` justo despues del
// SeparateTextIntoLines.  La etiqueta vieja ("shop item list display count")
// era una misidentificacion.
extern int     g_iNumLineMessageBoxCustom;   // IDA: g_iNumLineMessageBoxCustom (0x083A4324)
extern DWORD   DAT_083a4328;
// CurrentCameraPosition[3] contiguous (see globals.cpp).
extern float   CurrentCameraPosition[3];
extern DWORD&  DAT_083a432c;   // alias → CurrentCameraPosition[0]
extern DWORD&  DAT_083a4330;   // alias → CurrentCameraPosition[1]
extern float&  _DAT_083a4334;  // alias → CurrentCameraPosition[2]
extern char    g_lpszMessageBoxCustom[7][0x26];   // IDA: g_lpszMessageBoxCustom (0x083A44C4) — 7 líneas × 38
extern float   DAT_083a45d4;   // login background animation angle (Scene_Dispatch)
// Local server-name table poblada por F4/04 (custom list); stride 300 por grupo.
extern char    DAT_07d52c34[24 * 300];

// Login/scene init flags (byte-sized)
extern DWORD   DAT_083a7ac8;   // IDA: unk_83A7AC8 (0x083A7AC8), GetTickCount al empezar a conectar
extern char    DAT_083a7acc;
// CurrentCameraAngle[3] contiguous (see globals.cpp).
extern float   CurrentCameraAngle[3];
extern float&  DAT_083a7ad0;   // alias → CurrentCameraAngle[0]
extern float&  DAT_083a7ad4;   // alias → CurrentCameraAngle[1]
extern float&  DAT_083a7ad8;   // alias → CurrentCameraAngle[2]
extern DWORD   CameraWalkCut;  // IDA: CameraWalkCut (0x083A7AF4) - fade-in flag (1 = fading in)
extern DWORD   MoveSceneFrame; // IDA: MoveSceneFrame (0x083A7C00)
extern DWORD   DAT_083a7c04;   // in-game UI context A
extern DWORD   DAT_083a7c08;   // in-game UI context B
extern char    DAT_083a7c09;   // in-game UI flag byte
// g_iNumAnswer @ 0x083A7C0C ES `g_iNumAnswer`.  Disassembly 0x40182E:
// `89 35 0c 7c 3a 08` = `mov [0x083A7C0C], esi`.
extern int     g_iNumAnswer;     // IDA: g_iNumAnswer (0x083A7C0C)
extern DWORD   EnableMainRender;  // IDA: EnableMainRender (0x083A7C10)
extern int     LoginSubState; // IDA: DAT_083a7c14 (0x083A7C14)
extern DWORD   DAT_083a7c18;
extern DWORD   DAT_083a7c1c;
extern DWORD   DAT_083a7c20;
// Valores de ErrorMessage / NextErrorMessage: enum DialogMessage.
#include "ui/DialogMessage.h"
extern DWORD   ErrorMessage;  // IDA: ErrorMessage (0x083A7C24) - secondary in-game UI state machine
extern DWORD   NextErrorMessage;  // IDA: NextErrorMessage (0x083A7C28) - queued next UI state (applied at end of tick)
// Chaos Machine category selected by the local ErrorMessage=143 modal:
// 0 = general, 1 = Chaos Weapon.  It is not a server recipe/type byte.
extern DWORD   DAT_083a7c2c;
extern DWORD   CurrentCameraWalkType;  // IDA: CurrentCameraWalkType (0x083A7C38)
extern DWORD   CurrentCameraNumber;  // IDA: CurrentCameraNumber (0x083A7C3C)
extern DWORD   ServerListCount;
extern DWORD   DAT_083a7c44;
extern char    EnableSocket;   // IDA: EnableSocket (0x083A7C48) — connection-check-enable flag (byte, NO DWORD)
extern char    InitLogIn;  // IDA: InitLogIn (0x083A7C49) - Scene_Login init flag
extern char    InitLoading;  // IDA: InitLoading (0x083A7C4A) - Scene_Loading init flag
extern char    CharSelectSceneInitialized; // IDA: InitCharacterScene
extern char    InitMainScene;  // IDA: InitMainScene (0x083A7C4C)
extern char    m_bAccountBlockItem;  // IDA: m_bAccountBlockItem (0x083A7C4D)
extern DWORD   DAT_083a7c50;
extern DWORD   TestTime; // IDA: TestTime (0x083A7C54)
extern DWORD   DAT_083a7c5c;   // chat-log label for exit-countdown
extern char    DAT_083a7c64[64];  // server IP string buffer (filled on server select)
extern DWORD   DAT_083a7c68;
extern DWORD   DAT_083a7c6c;
extern DWORD   DAT_083a7c70;
extern DWORD   DAT_083a7c74;
extern DWORD   DAT_083a7c78;
extern DWORD   DAT_083a7c7c;
extern DWORD   DAT_083a7c80;
extern DWORD   DAT_083a7c84;
extern DWORD   DAT_083a7c88;
extern DWORD   DAT_083a7c8c;
extern DWORD   DAT_083a7c90;
extern DWORD   DAT_083a7c94;
extern DWORD   DAT_083a7c98;
// ── Bitmaps table (contiguous, 1450 entries × 0x38 stride = 0x13D30 bytes) ───
// DESVIACION: tabla de capacidad dinámica y base estable; el catálogo no
// recicla sus slots al cambiar de mapa.
// Original address range: 0x083A7CA0 .. 0x083BB9D0
// Slot layout: +0x00 active, +0x20 width(f), +0x24 height(f), +0x28 bpp(b),
//              +0x2C gl_handle(u), +0x30 ref_count(b), +0x34 pixels(ptr)
extern char*   g_BitmapsRaw;
constexpr int TEXTURE_STRIDE = sizeof(void*) == 4 ? 0x38 : 0x40;   // IDA: BITMAP_t, 0x38 bytes
constexpr int TEXTURE_STRIDE_DWORDS = TEXTURE_STRIDE / 4;

// Accesores retrocompatibles al slot[0] — el código usa (&DAT_xxx)[offset]
// para indexar la tabla contigua con aritmética de punteros.
#define DAT_083a7ca0  (*(DWORD*) &g_BitmapsRaw[0x00])
#define DAT_083a7cc0  (*(DWORD*) &g_BitmapsRaw[0x20])
#define DAT_083a7cc4  (*(DWORD*) &g_BitmapsRaw[0x24])
#define DAT_083a7cc8  (*(char*)  &g_BitmapsRaw[0x28])
#define DAT_083a7ccc  (*(DWORD*) &g_BitmapsRaw[0x2C])
#define DAT_083a7cd0  (*(char*)  &g_BitmapsRaw[0x30])
extern DWORD   m_dwUsedTextureMemory;   // IDA: DAT_083bb9d0 (0x083BB9D0)

// ── Named pipe handle (0x00563b58) ────────────────────────────────────────────

// ── Model data table (0x05828d58, stride 0xbc per entity_type) ────────────────
extern std::uintptr_t Models; // Base de Models; los desplazamientos siguen expresados en bytes.
extern void*   ModelsDump;   // IDA: ModelsDump (0x06F42A58) — model memory pool base (allocated once in Model_LoadItems)
// BoneVertex pool (see globals.cpp). Backed by g_BoneVertexBuf.
// DAT_0584621c = base del slot 0; &DAT_0584621c es DWORD*, así que el código existente
// like (float*)(&DAT_0584621c + vertIdx * 3) does k*12-byte stride correctly
// (DWORD=4 × 3 = 12 B per vertex). (char*)&DAT_0584621c yields a byte pointer.
extern char    g_BoneVertexBuf[32 * 15000 * 12];
#define DAT_0584621c  (*(DWORD*)(g_BoneVertexBuf + 0))
// DAT_05846224 es una etiqueta de Ghidra 8 bytes adentro del mismo buffer (el inicio de pfOut
// de Skeleton_Transform: pfDst = pfOut - 2 = &DAT_0584621c + local_64). Las escrituras abarcan 12 bytes desde pfDst.
#define DAT_05846224  (*(DWORD*)(g_BoneVertexBuf + 8))

// ── BMD bounding-box scratch arrays (BMD_CreateBoundingBox — BMD_ComputeBounds) ────────
// Scratch de BMD_CreateBoundingBox — una entrada por hueso (ver globals.cpp).
// Los companions +4/+8 son macros que proyectan dentro del slot 0 de su base,
// igual que DAT_081cb60c, para que `(char*)&DAT_x + n` y `((float*)&DAT_x)[i]`
// sigan valiendo lo mismo que en el binario.
extern short   DAT_077d87fc[200];      // contador de vertices por hueso
extern float   DAT_05827a98[200*3];    // bbox max, 3 floats por hueso
extern float   DAT_06f42a5c[200*3];    // bbox min, 3 floats por hueso

// ── UI name-list panel data (ShowCheckBox) ────────────────────────────────────
// DAT_083a430c  — macro alias dentro de DAT_083a42f8 (ver bloque de dialog button rects)

// ── Entity / animation tick globals ───────────────────────────────────────────
extern float   WorldTime; // IDA: WorldTime (0x05826E08)
extern char    SoccerObserver;   // SoccerObserver (IDA @0x05826D33) - lo escriben
                               // InitGame y el F3/23, y lo lee RenderCharacter.
                               // La etiqueta vieja ("guild name flag B") era falsa.

// DAT_07e919bc — base del array de info de guild (indexado por slot de party) — declarado arriba como DWORD
// DAT_07abf5dc — player/entity slot count — declared above as DWORD
// DAT_07abf5d8 — entity_array[0] base pointer — declared above as char*
// FrustrumY/228 — login camera Y/X screen-pos — declared above as DWORD

// ── Buffers temporales de posición de huesos (usados en el traspaso de animación) ──
extern DWORD   DAT_07abf444[12]; // temp bone matrix buffer A
extern DWORD   DAT_07abf474[12]; // IDA flt_7ABF474 (usado por MoveJoint subtipo 0xC)
extern DWORD   DAT_07abf3e4[12]; // temp bone matrix buffer B

// ── Animation distance / frequency constants ───────────────────────────────────
extern float  _DAT_00552650;   // animation LOD cutoff distance

// ── Teleport-anim pool (0x07c80110, stride 0x70, 100 slots) ──────────────────
// Sized via IDA bound: (0x7c82cd0 - 0x7c80110) / 0x70 = 0x2BC0 / 0x70 = 100.
// Entity_TeleportAnim recorre los 100 slots escribiendo 0x70 bytes por paso: tiene
// que ser el array completo.
extern DamagePoint DAT_07c80110[kDamagePointCount];   // active-flag at +0; slot stride 0x70

// ── Character/effect update pool (0x07c85890, stride 0x1BC, 1002 slots) ──────
extern OBJECT  Sprites[kSpritePoolSize];   // IDA: unk_7C85890 (0x07C85890); nombre de MU 5.2

// ── Math / animation constants ────────────────────────────────────────────────
// _DAT_0055283c extern declarado arriba (1/640, X scale constant)

// Input / player name buffers

// ── Sprite entity pool (RenderParticles — decoration/item-drop sprite render) ────
extern float  _DAT_00552914;       // anim speed scale constant B

// ── Server select input (CServerSelWin_UpdateWhileActive) ───────────────────────────────────────
extern DWORD   ServerLocalSelect;        // selected channel (port % 0x14 + 1)  [IDA 0x0056169C]

// ── Char menu UI builder (RenderHelpWindow) ───────────────────────────────────────
extern int     DAT_07e11d20;        // UI mode: 1=class-list-A, 2=class-list-B, 3=stats
extern int     DAT_07e11d24;        // character class/subtype ID (range 0..0x1FF)
extern char    TextList[];   // IDA: TextList (0x07E90798) — 30 líneas x 100 del tooltip
extern int     TextListColor[30];  // IDA: TextListColor (0x07E91708) - color por linea
extern int     TextBold[30];  // IDA: TextBold (0x07EA7B10) - negrita por linea
// extern char    DAT_07d329c4;        // class name table A (first entry base)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d32af0;        // class info list A (stride 300, limit 0x7d34134)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d34134;        // class name table B (first entry base)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d34260;        // class info list B (stride 300, limit 0x7d358a4)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d358a4;        // class name / info buffer C   // -> alias a GlobalText, ver el final del archivo
extern "C" std::uintptr_t DAT_07d78068; // Base desplazada de atributos de items, filas de 64 bytes.
// ── Posición del item que se está arrastrando ───────────────────────────────
// En IDA esto vive en `Inventory[32].Type` (el pool de shop/trade-in,
// slot 32, campo Type en offset 0) — lo usan sub_47D410 (preview de stats) y el
// render del footprint durante el drag.
//
// NO usar DAT_07ea9844 para esto: en el binario esa dirección se escribe SOLO
// como byte (`mov byte ptr [7EA9844], bl` en 0x4D2586 y `..., 0` en 0x4D1D8B) y
// se lee únicamente en Scene_MapTick, como 4º parámetro (bSell) de
// RenderItemInfo/RenderRepairInfo: un índice de slot != 0 haría que el tooltip
// muestre el PRECIO DE VENTA fuera de una tienda.
extern "C" BYTE Inventory[];
#define ItemPickedPos  (*(int*)&Inventory[32 * 68])   // Inventory[32].Type

// Base de la tabla ItemAttribute.  Devuelve 0 si todavia no se cargo, para
// que los callers no computen attrBase = type*0x40 + basura.
static __forceinline std::uintptr_t ItemAttribute_Base(void) {
    const std::uintptr_t p = DAT_07d78068;
    const std::uintptr_t allocation = DAT_055c9e44;
    // Reserva: (ITEM_MAX_EX + 0x400) filas de 64 bytes, con base
    // desplazada hasta 0x3ff filas. No asumir un mapa virtual de Windows.
    if (!allocation || p < allocation || p - allocation > 0x400u * 64u ||
        (p - allocation) % 64u != 0) return 0;
    return p;
}
// Fila `p` de la tabla ItemAttribute (base + type*64), validada contra la
// tabla cargada y no contra rangos de direcciones: en Linux x86 el heap puede
// quedar por encima de 2 GB.
static __forceinline bool ItemAttribute_IsRow(uintptr_t p) {
    const uintptr_t base = ItemAttribute_Base();
    return base && p >= base && (p - base) % 64u == 0 && (p - base) / 64u < (uintptr_t)ITEM_MAX_EX;
}
// DAT_07e91530/534/53c/540 — columnas 2/3/5/6 de la fila 0 de DAT_07e91528
// (ver la declaracion de la tabla mas abajo).
#define DAT_07e91530   (DAT_07e91528[2])
#define DAT_07e91534   (DAT_07e91528[3])
#define DAT_07e9153c   (DAT_07e91528[5])
#define DAT_07e91540   (DAT_07e91528[6])
// Strings de formato de la UI (datos de sólo lectura en el binario)
// RenderItemInfo string constants
extern char    DAT_0055a4e0[];   // item name format string
extern char    DAT_0055a5f4[];   // item stats header format string
extern char    DAT_0055a4e4[];   // salto de linea — separador tras el precio
extern char    DAT_0055a570[];   // salto de linea — separador tras el nombre
extern char    DAT_0055a5f0[];   // salto de linea — separador de media altura
extern char    DAT_0055a5fc[];   // salto de linea — separador de media altura
extern char    DAT_0055a640[];   // salto de linea — separador de media altura
// extern char    DAT_07d3b40c[];   // item level line format   // -> alias a GlobalText, ver el final del archivo

// ── Misc low-address globals ──────────────────────────────────────────────────
// (07abf06, 07d29e5, 07eaa11 — byte flags or array references)

// ── Chat ring buffer (UI_RenderChatLogOverlay renderer / UIChatLogWindow_AddText) ──────
// Único buffer real: 0x77 entries × 0x118 bytes stride.
// Per-slot layout:
//   +0x000..+0x00A  sender name (11 bytes)
//   +0x00B..+0x10B  message text (257 bytes)
//   +0x10C          type/channel DWORD (0..5)
//   +0x114          cached text-extent cx (LONG)
// DAT_07df938b, DAT_07df948c, DAT_07df9494 en el binario original son ALIASES
// dentro de este buffer a los offsets 0x0B / 0x10C / 0x114 del slot 0 (Ghidra
// los recuperó como globales independientes); acá son macros para que writer y
// reader vean la misma memoria.
extern char    DAT_07df9380[0x77 * 0x118];
// Alias del primer byte del campo msg (slot 0). Las fórmulas
// `&DAT_07df938b + slot*0x118` del decompile de Ghidra resuelven al byte
// correcto gracias a que éste es un lvalue dentro del buffer real.
#define DAT_07df938b (DAT_07df9380[0x0B])
// Alias del DWORD type/channel (slot 0). Mantiene el tipo int para que los
// accesos `(&DAT_07df948c)[slot*0x46]` del decompile avancen 0x118 bytes por
// slot (0x46 DWORDs = 280 bytes).
#define DAT_07df948c (*(int*)&DAT_07df9380[0x10C])
// Alias del LONG cached cx (slot 0). Los call-sites usan
// `*(LONG*)(&DAT_07df9494 + slot*0x118)` — ese cast de pointer-arith a char*
// se hace en el call-site, acá sólo exponemos el DWORD base.
#define DAT_07df9494 (*(int*)&DAT_07df9380[0x114])

// Key debounce for scroll/nav
extern DWORD   DAT_07e11970;           // PageUp debounce
extern DWORD   DAT_07e11974;           // PageDown debounce
extern DWORD   DAT_07e11984;           // Up-arrow debounce
extern DWORD   DAT_07e1198c;           // Down-arrow debounce
extern DWORD   DAT_07e119f4;           // 'B' key debounce
extern DWORD   DAT_07e11a34;           // 'R' key debounce

// Item/hotkey assignment UI
extern char    DAT_07db870c;           // item-selection toggle (char)
extern DWORD   DAT_07ea840c;           // UI cursor / hotkey X position
extern DWORD   DAT_07ea8408;           // UI cursor / hotkey Y position

// Chat input channel buffers (9 channels × 0x100 bytes)
extern char    MacroText[10 * 0x100];   // macro hotkey table: 10 slots × 256 bytes  [IDA 0x07E0FFC8]
// Fila 9 de la tabla de arriba (0x07E0FFC8 + 0x900 = 0x07E108C8), no un global
// aparte: es el buffer del destinatario de susurro y lo llenan FUN_00494520,
// strcmp/strlen/memcpy (hasta 255 bytes).
#define DAT_07e108c8   (MacroText[0x900])

// Player name (for chat name-match / tab-complete)
extern char    DAT_005592dc;           // player name string (char array)

// Text input / command buffers
// extern char    DAT_07d3d284;           // "/whisper" command string   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3d3b0;           // "/pvp" command string   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3cdd4;           // GM command string   // -> alias a GlobalText, ver el final del archivo
// Chat command parser name buffers (Chat_ValidateCommandName)
// extern char    DAT_07d3cb7c;           // player name buffer 1 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3cca8;           // player name buffer 2 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3c924;           // player name buffer 3 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3c6cc;           // player name buffer 4 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3bfc4;           // player name buffer 5 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3c0f0;           // player name buffer 6 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3d608;           // whisper target display buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d3d734;           // pvp-toggle result message buffer   // -> alias a GlobalText, ver el final del archivo
extern DWORD   m_bBlockWhisper_1;  // IDA: m_bBlockWhisper_1 (0x07E11DAC) - command-result flag (1=whisper, 0=pvp-off)

// B-key toggle guards
// 0x07EAA130 ES g_bServerDivisionEnable (confirmado por xrefs de IDA:
// ReceiveTalk, SendMove, sub_492F10, Chat_InputTick, GetScreenWidth,
// RenderServerDivision).  ServerDivisionOpened es un alias: el writer
// (Net_Process, ReceiveTalk sub 5) y los readers tienen que ver el mismo byte.
// 0x07EAA131 es g_bServerDivisionAccept, un int aparte en el port.
#define ServerDivisionOpened   (*(char*)&g_bServerDivisionEnable)
extern char    DAT_07eaa132;           // guard for B-key toggle enable
extern char    RepairEnable_0;           // IDA: RepairEnable_0 (0x07EAA134)

// Screenshot / error dialog context
extern DWORD   strID;   // IDA: strID (0x00583D88) — etiqueta vacía de UIChatLogWindow_AddText
// extern char    DAT_07d55410;           // error/info message for UIChatLogWindow_AddText   // -> alias a GlobalText, ver el final del archivo

// Connection-check context

// DAT_05826ceb — rolling packet counter byte (declared above near 0x05826xxx block)

// ── Timers de cuenta regresiva de chat/UI (los usan Sound_Queue.cpp y UI/Chat.cpp) ──
extern int     NoticeTime;  // IDA: NoticeTime (0x00559CDC) - system-message scroll timer (reset to 300 on Chat_AddMessage)
extern int     DAT_00559ce4;           // player-chat scroll timer   (reset to 0x96 on UI_ShowDialog)
// Strings que Sound_Countdown1 / Sound_Countdown2 vuelven a mostrar al vencer el timer:
extern char    DAT_07e11dd0[256];      // byte_7E11DD0: texto del aviso periódico (MoveNotices/CreateNotice)
// Siempre vacios: nadie los escribe en todo el binario (un xref cada uno, la
// lectura de Chat_TickMessageTimer).  Son buffers y no char sueltos porque se
// leen como cadenas -- ver la nota en globals.cpp.
extern char    DAT_07e11dd8[4];        // strText          (arg2 de UIChatLogWindow_AddText)
extern char    DAT_07e11ddc[12];       // byte_7E11DDC     (arg1 de UIChatLogWindow_AddText)

// ── Chat ring buffers ─────────────────────────────────────────────────────────
// System message buffer (UI_AddNotice / Chat_DrawMessages)
extern char    Notice[6 * 0x108];  // IDA: Notice (0x07DB80D8) - system chat ring buffer (6 slots × 0x108 bytes)

// Server-config globals (poblados por los opcodes 0xDD/DE/DF).
extern "C" {
    extern DWORD g_MaxCharacterLevel;     // 0xDF — cap de level (default 400)
    extern WORD  g_CharDeleteMaxLevel;    // 0xDD — max level que permite delete
    extern BYTE  g_CharCreationEnable;    // 0xDE — permite crear chars
}
extern int     DAT_07e11d9c;           // system message count (max 5, i.e. 6 entries 0..5)
// Player chat / notification buffer (UIChatLogWindow_AddText / UI_DrawPlayerList)
// Entry stride: 0x118 bytes (= 0x46 × sizeof(DWORD), Ghidra type convention)
// Layout per entry: sender[0..0xA] + msg[0xB..0x10B] + type[0x10C]
extern int     DAT_07e11da4;           // player chat entry count (max 0x77 = 119 entries)

// ── Chat text-extent work variables (Chat_DrawField / Chat_DrawEntry) ─────────
extern SIZE    g_TextExtent07E113D0;   // IDA TextSize: one contiguous cx/cy pair
extern SIZE g_TextExtent07E113D0;     // IDA: TextSize (0x07E113D0)
extern int&   _DAT_07e113d4;           // last GetTextExtentPointA result (cy)
extern DWORD   InputFrame;           // char-index/step counter (draw-field loop)
extern char    DAT_07e11cec[10 * 4];   // 10 slots x 4 bytes; WinMain escribe con slot*4 (slot 0..9)

// ── Chat string constants (displayed on login/cursor events) ──────────────────

// ── Guild system globals (opcodes 0x90-0x99) ──────────────────────────────────
// InventoryOpened — declared above as char
// CharacterOpened — declared above as char
// GoldenArcherOpenType — declared above as DWORD
extern int     GoldenArcherItemCount;           // g_shEventChipCount (IDA 0x07EAA12C)  [IDA 0x07EAA12C]
// StorageGoldFlag — guild UI flag — declared above as DWORD
extern BYTE    GoldenArcherLuckyNumberText[64];       // guild entity pool (zeroed on stage 3, 0x40 bytes)  [IDA 0x07EA97C0]
extern char    GoldenArcherLuckyNumberTicket;           // char-select flag D (set 1 when guild stage==3)  [IDA 0x07E11D73]
// Guild message string buffers (shown via ShowGuildMessage / CreateOkMessageBox):
// extern BYTE    DAT_07d5b680;           // guild create result msg 1   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    DAT_07d5b7ac;           // guild create result msg 2   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    DAT_07d5c10c;           // guild create result msg 3   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    DAT_07d5c238;           // guild create result msg 4   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    DAT_07d5b8d8;           // guild create result msg 5   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    DAT_07d6813c;           // guild join OK message   // -> alias a GlobalText, ver el final del archivo
extern char    param_2_07d68268;       // guild join error format string (wsprintfA %d)
// extern BYTE    DAT_07d58ea8;           // guild request OK message   // -> alias a GlobalText, ver el final del archivo
extern char    param_2_07d58fd4;       // guild request error format string (wsprintfA %d)
// Posición objetivo del guild (la setean los opcodes 0x94 / 0x96):
extern DWORD   GoldenArcherLuckyNumber;          // guild target tile X (4-byte, overlapping)  [IDA 0x00559F58]
extern WORD    DAT_00559f5c;           // guild target tile Y (2-byte)
// Campos del sub-estado de login (también los escribe la rama 3 del guild 0x94):
// InputTextMax (int&), InputEnable (= DAT_00559c84) e InputNumber: declarados más arriba.
// ── Estado de la UI de guild (bloque 0x083a, usado por ShowGuildMessage / la lista de miembros) ──
// g_iNumLineMessageBoxCustom y g_lpszMessageBoxCustom (char[7][0x26]): declarados más arriba.
// ── Dialog button rects (0x083A42F8, 2 entradas × 5 ints = 0x28 bytes) ───────
// Layout por entrada: [0]=bitmapId-240 (1..4) [1]=x [2]=y [3]=width [4]=height.
// Escrito por CreateOkMessageBox/CreateDialogInterface/ShowCheckBox/sub_51D9E0/
// sub_51DA80 (todos hacen `memset(&unk_83A42F8, 0, 0x28)` -> la region es
// EXACTAMENTE 40 bytes) y leido por RenderErrorMessage (&unk_83A4304, o sea el
// campo width de la entrada 0, avanzando de a 5 ints) y por el hit-test de
// UI_InGameMenu (&unk_83A42FC = campo x).
//
// En el binario 42F8 / 42FC / 4304 / 430C son OFFSETS DENTRO DE ESTA MISMA REGION:
// por eso 42FC / 4304 / 430C son macros sobre DAT_083a42f8 y no globals propios.
extern DWORD   DAT_083a42f8[10];       // UI panel descriptor array (set by guild funcs)
#define DAT_083a4304   (*(int*)&DAT_083a42f8[3])         // entry0.width
#define DAT_083a430c   ((DWORD*)&DAT_083a42f8[5])        // entry1 base
// ErrorMessage  — declared above as DWORD
// NextErrorMessage  — declared above as DWORD
extern int     DAT_083a7c30;           // guild member count for UI
extern int     DAT_083a7c34;           // guild UI auxiliary param


// ── Más declaraciones de globals definidos en globals.cpp ─────────────────────
extern char    LockInputStatus; // IDA: DAT_07e11d6f (0x07E11D6F)
extern DWORD   DAT_07e11d84;              // UseSkillWarrior 43: tick de activacion
extern char    DAT_07e11dec;
extern char    DAT_07e11df0;
extern int     g_WorldLoading;            // >0 mientras corre OpenWorld (ver WinMain WM_USER)
extern int     m_iDevilSquareLimitLevel[4][2];
extern int     m_iBloodCastleLimitLevel[12][2];

// Ranking de Devil Square / Blood Castle (opcode 0x93): 11 registros
// (0x083A7AF8..0x083A7C00 en el binario, de 0x18 bytes). Desde 0.97.40 cada
// registro es name[40], score, RewardExperience, RewardMoney = 52 bytes.
#define GUILD_MEMBER_STRIDE       52
#define GUILD_MEMBER_TABLE_BYTES  (11 * GUILD_MEMBER_STRIDE)
extern BYTE    DAT_083a7af8[GUILD_MEMBER_TABLE_BYTES];
#define DAT_083a7b04  (*(DWORD*)&DAT_083a7af8[40])   // score del registro 0
#define DAT_083a7b0c  (*(DWORD*)&DAT_083a7af8[48])   // zen
// Columnas de datos de miembro de guild (parte del mismo array de stride 0x18 que arranca en DAT_083a7af8):

// Guild leaderboard UI strings (RenderMatchScore / GuildOverview_Render):
// extern char    DAT_07d59358;           // guild panel title string ("Guild War Score" etc.)   // -> alias a GlobalText, ver el final del archivo
extern char    param_2_07d59484;       // format string for current player name ("%s")
// extern char    DAT_07d5ba04;           // column headers array (stride 300, ~5 entries, ends at 0x7d5bfe0)   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d5bfe0;           // last column header entry   // -> alias a GlobalText, ver el final del archivo
extern char    param_2_005618a4;       // rank number format string ("%d.")
extern char    param_2_005618a8;       // kills column format string ("%d")
extern char    param_2_005618b0;       // deaths column format string ("%d")
extern char    param_2_005618b4;       // score column format string ("%d")
extern char    param_2_07d68e20;       // score total format string
extern char    param_2_07d68f4c;       // opponent deaths format string ("%d")
extern char    param_2_07d69078;       // own kills format string ("%d")

// ── Pre-compile missing globals (found by diff scan) ─────────────────────────

// Game math constants missing from 0x005524xx–0x00552dxx section:
extern float  FloatZero;   // float constant 0.0 (guard for division / degenerate-length check)

// Hash table / debug string (0x00558xxx):
extern char    DAT_00558128[];  // debug log data block / format string

// Compressed-assets flag (0x0055a7c4):
extern char    DAT_0055a7c4;   // 0=plain files, 1=compressed OZJ/OZT; alias: g_tex_ext_mode
#define g_tex_ext_mode  DAT_0055a7c4

// Misc.cpp / Entity gravity constants:
extern float   _DAT_00552570;

// FPS timer (0x07e016f0) — DWORD millisecond timestamp, accessed via _DAT_ Ghidra alias:
extern DWORD   _DAT_07e016f0;  // last FPS tick timestamp (timeGetTime value)

// Pool en 0x083a1218 (array Butterfles; lo recorren SMD_Parser.cpp y Entity_Render.cpp, entre otros):
extern OBJECT  Butterfles[10];   // IDA: unk_83A1218 (0x083A1218); nombre de MU 5.2

// Float aliases for view/projection matrix globals (0x083a4xxx):
// DWORD counterparts (DAT_083a414c etc.) are already declared above.
extern float& _DAT_083a414c;   // alias → CameraMatrix[3]
extern float& _DAT_083a415c;   // alias → CameraMatrix[7]
extern float& _DAT_083a416c;   // alias → CameraMatrix[11]
extern float& _CameraRayOriginX;   // _DAT_083A4284: camera position X alias
extern float& _CameraRayOriginY;   // _DAT_083A4288: camera position Y alias
extern float& _CameraRayOriginZ;   // _DAT_083A428C: camera position Z alias
extern float  PerspectiveX;   // projection scale X (float alias of PerspectiveX)
extern float  PerspectiveY;   // projection scale Y (float alias of PerspectiveY)
extern float  ScreenCenterYFlip;   // projection center Y (float)  [IDA 0x083A4294]

// Bone transform scratch buffer (0x06989xxx) — 3x4 row-major matrix (12 floats).
// Respaldado por DAT_06989c9c_matrix (contiguo). DAT_06989c9c es la dirección de
// matrix[0]; los dos alias _DAT apuntan a los elementos [5] y [10] de la diagonal.
// El código que pasa `&DAT_06989c9c` como float* obtiene un puntero adentro del array,
// so (&DAT_06989c9c)[N] reaches matrix[N] correctly.
extern float   DAT_06989c9c_matrix[12];
extern float&  DAT_06989c9c;   // alias → DAT_06989c9c_matrix[0]

// Bone normal-intensity pool (see globals.cpp). Backed by g_BoneNormalBuf.
// &DAT_077e298c es DWORD*, así que (float*)(&DAT_077e298c + meshIdx * 15000)
// avanza k*4 bytes por float (correcto para un buffer escalar por normal).
extern char    g_BoneNormalBuf[32 * 15000 * 4];
#define DAT_077e298c  (*(DWORD*)(g_BoneNormalBuf + 0))

// DAT_05846224 — declared above as #define aliasing g_BoneVertexBuf + 8.

// ── CharMenu_Build sub-function globals ──────────────────────────────────────
extern int     DAT_07eaa158;           // secondary text-entry counter (CharMenu_AppendSkillReq)
// Tabla de requisitos de stats @ 0x07E91528 — 12 filas x 10 ints.  Los simbolos
// DAT_07e9152c/530/534/538/53c/540/544/548/54c son las COLUMNAS 1..9 de la fila 0
// (macros de arriba), no globals independientes.  Fila i, columna c =
// DAT_07e91528[10*i + c].
extern int     DAT_07e91528[12 * 10];
// extern char    DAT_07d359d0;           // skill description string table base (slot 0, stride ~0x138)   // -> alias a GlobalText, ver el final del archivo
extern int     DAT_00559fe0;           // class-data cache guard (last built class_id)

// ── UI_StatsPanel (RenderErrorMessage) globals ─────────────────────────────────────
// Float step/scale constants (0x00552xxx)
// Dialog text color control
extern DWORD   m_dwTextColor;   // current text color ABGR (0xffffffff = white)
// SetBackgroundTextColor — declared above as DWORD
// InputTextWidth — declarado arriba (int)
// Toggle flags
extern char    m_bAutoAttack;   // IDA: m_bAutoAttack (0x00559C5C)
extern char    m_bWhisperSound;   // IDA: m_bWhisperSound (0x07E11D80) — aviso sonoro de susurros
// Entity/level data
// Format strings for numeric dialogs
// extern char    DAT_07d46e60;   // login account name format   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d493e0;   // duel offer sub-text format   // -> alias a GlobalText, ver el final del archivo
// Vendor/NPC name tables
// DAT_083a4304  — macro alias dentro de DAT_083a42f8 (ver bloque de dialog button rects)
// g_lpszDialogAnswer @ 0x083A4348 — las 10 respuestas del
// dialogo, 38 (0x26) bytes cada una.  Disassembly 0x401812:
// `bf 48 43 3a 08` = `mov edi, 0x083A4348` seguido de `mov ecx, 5Fh; rep stosd`
// = 0x5F dwords = 380 bytes = 10 * 38, que fija el tamano real.
extern char    g_lpszDialogAnswer[10][1][38];  // IDA: g_lpszDialogAnswer (0x083A4348)
// Guild message decode state
// DAT_083a7c08 — declared above as DWORD
extern char    DAT_083a7c09;   // guild dialog flag B
// g_iNumAnswer — declared above as DWORD
// Zona del cursor / hit-test (también se usa en otros paneles)
// MouseLButtonPush — declared above as DWORD
// Y del cursor de selección en la lista de servers (también se usa en el panel 0x96)
// String literals at fixed addresses (Ghidra: s__s_xxx_address)
extern char    s__s_On_0056184c[];    // "Sound: On"  / toggle-on string
extern char    s__s_Off_00561854[];   // "Sound: Off" / toggle-off string
extern char    s__s_On_0056185c[];    // "Music: On"
extern char    s__s_Off_00561864[];   // "Music: Off"
extern char    szServerMigrationError[];   // IDA: byte_56186C (sin nombre) — "error al migrar de servidor" en coreano; acá vacío
// Error/message string table (in-game dialogs, ~0x07d4xxxx range)

// ── GL_State cached state globals ────────────────────────────────────────────
extern int AlphaBlendType;   // IDA: AlphaBlendType (0x083A412C)
extern char    AlphaTestEnable;  // IDA: AlphaTestEnable (0x083A411D) - cache (glEnable/Disable 0xBC0). El comentario viejo decia 'fog' y es FALSO: ver GL_SetAlphaTest/680.
extern char    TextureEnable;  // IDA: TextureEnable (0x083A4125) - cache (glEnable/Disable 0xDE1 = GL_TEXTURE_2D). El comentario viejo decia 'depth test' y es FALSO.
extern int     DAT_083a42f4;   // screenshot active flag
extern int     GrabScreen;   // IDA: GrabScreen (0x083A42F0) — screenshot counter (mod 10000)
// ── Float constants (overlapping globals) ────────────────────────────────────
extern float   _DAT_00552580;  // 0.0f guard constant
extern float   Math_DegreesToRadians;  // degrees-to-radians (π/180 = 0.017453f)
// ── Effect pool ──────────────────────────────────────────────────────────────
extern OBJECT  Effects[kEffectPoolSize];   // IDA: unk_7B11670 (0x07B11670), 200 slots hasta 0x07B27150; nombre de MU 5.2
// ── Monster_Data string literals ─────────────────────────────────────────────
extern FILE   *SMDFile_0;  // IDA: SMDFile_0 (0x07D7806C) - file handle (MonsterSetBase2 parser)
extern char    DAT_005580ac[]; // "rb" fopen mode string
extern char    bBuxCode[3]; // IDA: bBuxCode (0x00558090)
extern char    TextParserTokenString[256]; // DAT_07CF1EF0 — TokenString output of
                                   // GetToken (0x47A1F0).  NO confundir con
                                   // ParserTokenString, que es el buffer del OTRO
                                   // tokenizer (Parse_NextToken, 0x50E2C0).
extern char    DAT_00559088;   // sentinel "END" string base
extern int     DAT_07d7807c;   // spawn slot index
// ── Monster sound filename strings (DAT_0055d…) ──────────────────────────────
// ── Monster sound s_ named strings (0055cxxx range) ──────────────────────────

// ── CreateEffect float constants ────────────────────────────────────────────
// ── Boids (40 slots de OBJECT; IDA: sub_500E80 y su render) ─────────────────
// Los 30+ globals DAT_0839bc?? son macros que indexan dentro de un único buffer
// contiguo: Weather_Update y Particle_PathUpdate los acceden con stride 0x1bc
// (slot stride) o 0x6f (int stride = 0x1bc/4).
extern OBJECT  Boids[40];   // IDA: unk_839BCB0 (0x0839BCB0); nombre de MU 5.2
#define DAT_0839bcb0  (*(char *)&Boids[0].Live)                              // slot[0] active flag (char)
#define DAT_0839bcb2  (*(short *)&Boids[0].Type)          // slot[0] type (short, stride 0xde × short)
#define DAT_0839bcbc  (*(unsigned int *)&Boids[0].Scale)          // slot[0] angular speed (int/float)
#define DAT_0839bcc0  (*(unsigned int *)&Boids[0].Position[0])          // slot[0] pos_x (int/float)
#define DAT_0839bcc4  (*(unsigned int *)&Boids[0].Position[1])          // slot[0] pos_y (int/float)
#define DAT_0839bcc8  (*(unsigned int *)&Boids[0].Position[2])          // slot[0] pos_z (int/float)
#define DAT_0839bccc  (*(unsigned int *)&Boids[0].Angle[0])          // slot[0] vel_x (int/float)
#define DAT_0839bcd0  (*(unsigned int *)&Boids[0].Angle[1])          // slot[0] vel_y (int/float)
#define DAT_0839bcd4  (*(unsigned int *)&Boids[0].Angle[2])          // slot[0] phase (int/float)
#define DAT_0839bd08  (*(unsigned int *)&Boids[0].HiddenMesh)          // slot[0] color R or -1 (int)
#define DAT_0839bd10  (*(unsigned int *)&Boids[0].LifeTime)          // slot[0] color G/intensity (int)
#define DAT_0839bd14  (*(unsigned int *)&Boids[0].BlendMesh)          // slot[0] color B (int)
#define DAT_0839bd30  (*(unsigned int *)&Boids[0].Timer)          // slot[0] rotation angle (int/float)
#define DAT_0839bd40  reinterpret_cast<char*>(Boids)[0x090]                              // slot[0] direction vector base (3 floats)
#define DAT_0839bd70  (*(char *)&Boids[0].Direction[0])                              // slot[0] cached head pos X (float byte addr)
#define DAT_0839bd74  (*(char *)&Boids[0].Direction[1])                              // slot[0] cached head pos Y
#define DAT_0839bd78  (*(char *)&Boids[0].Direction[2])                              // slot[0] velocity Z (float)
#define DAT_0839bd7c  (*(unsigned int *)&Boids[0].Velocity)          // slot[0] scale (int/float)
#define DAT_0839bd84  (*(char *)&Boids[0].ShadowScale)                              // slot[0] size (float byte addr)
#define DAT_0839bd88  (*(char *)&Boids[0].Gravity)                              // slot[0] altitude limit (float byte addr)
#define DAT_0839bd98  (*(char *)&Boids[0].Light[0])                              // slot[0] color R float byte addr
#define DAT_0839bd9c  (*(char *)&Boids[0].Light[1])                              // slot[0] color G float byte addr
#define DAT_0839bda0  (*(char *)&Boids[0].Light[2])                              // slot[0] color B float byte addr
#define DAT_0839bdb4  (*(char *)&Boids[0].AI)                              // slot[0] state byte (cloud life phase)
#define DAT_0839bdb5  (*(char *)&Boids[0].CurrentAction)                              // slot[0] blend mode byte
#define DAT_0839bdb6  (*(char *)&Boids[0].PriorAction)                              // slot[0] field at +0x106
#define DAT_0839bdb8  (*(char *)&Boids[0].AnimationFrame)                              // slot[0] rotation float byte addr
#define DAT_0839bdbc  (*(char *)&Boids[0].PriorAnimationFrame)                              // slot[0] field at +0x10c
#define DAT_0839be11  (*(char *)&Boids[0].AlphaEnable)                              // slot[0] byte field
#define DAT_0839be14  (*(unsigned int *)&Boids[0].AlphaTarget)          // slot[0] alpha fade (int/float)
// ── Weather float constants ───────────────────────────────────────────────────

// ── Camera / viewport globals ─────────────────────────────────────────────────
extern int     DAT_0055a7b0;   // SetActionObject: tipo de objeto que se derrumba
extern int     DAT_0055a7b4;   // SetActionObject: World en el que aplica
extern int     DAT_0055a7b8;   // SetActionObject: contador de frames (20 = arranca)
extern float   _DAT_0055a7bc;  // viewport height (float)

// ── GL_State cache ────────────────────────────────────────────────────────────
extern char    CullFaceEnable;  // IDA: CullFaceEnable (0x083A411C) - GL state cache byte
extern char    DepthTestEnable;  // IDA: DepthTestEnable (0x083A411E) - GL state cache byte
extern float   DAT_083a4120;   // Map_InitRayCast running closest-hit ray-t (was DWORD; widened to match IDA flt_83A4120 — audit #7)
extern DWORD   DepthMaskEnable;  // IDA: DepthMaskEnable (0x083A42E8) - GL state cache dword
#define DAT_0055a7bc _DAT_0055a7bc

// ── Fog / projection globals ──────────────────────────────────────────────────
extern float   DAT_00561558;   // fog start distance
extern DWORD   FogColor;   // fog color pointer / float start  [IDA 0x0056155C]

// ── Perspective constant ──────────────────────────────────────────────────────

// ── Joint pool ────────────────────────────────────────────────────────────────
// JOINT lleva un puntero en +0x40: los campos siguientes se corren en x64.
constexpr int JOINT_FIELD_SHIFT = (int)sizeof(void*) - 4;
constexpr int JOINT_STRIDE = 0x9D8 + JOINT_FIELD_SHIFT;
extern JOINT   Joints[kJointPoolSize];   // IDA: unk_7B27150 (0x07B27150), 500 slots; nombre de MU 5.2
static_assert(sizeof(JOINT) == JOINT_STRIDE, "JOINT_STRIDE");

// ── Joint_Create float constants ─────────────────────────────────────────────
extern float   _DAT_00552a14;  // joint float constant

// ── Timer globals ─────────────────────────────────────────────────────────────
extern DWORD   DAT_05826e14;   // FPS counter B
extern DWORD   FrameTimeCurrentMs;   // IDA: DAT_05826df0
extern DWORD   DAT_05826e00;   // FPS accumulator ring buffer
extern float   DeltaT;  // delta time per frame  [IDA 0x0055979C]
extern DWORD   FrameTimePreviousMs;  // IDA: DAT_05826dfc
extern float   FPS;  // smoothed FPS value  [IDA 0x05826DF8]

// ── Music.cpp globals ─────────────────────────────────────────────────────────
// IDA: DAT_055C9D04

// ── Sound_DS3D globals ────────────────────────────────────────────────────────

// ── Net_Connect globals ───────────────────────────────────────────────────────
extern DWORD   First; // IDA: First (0x055CA15C)

// ── Net_PacketSession globals ─────────────────────────────────────────────────
// Char-select entry pools — IDA layout:
//   DAT_07ea5b68 .. 0x07ea7b48 (= 8160 bytes = 116 slots × 0x44 stride)
//   DAT_07ea9880 .. 0x07eaa100 (= 2176 bytes =  32 slots × 0x44 stride)
// Cada slot es el registro del panel de char-select. En nuestro build estaban
// declarados como DWORDs sueltos, lo que hacía que CloseInventoryRelatedWindows (CharPreview_Reset)
// recorriera mucho más allá del final, en memoria random → AV.
extern DWORD   DAT_07eaa0e8;
extern DWORD   MarkColor[16];   // MarkColor[16] — paleta de la marca de guild (ARGB)
extern BYTE    DAT_07e11f78[0x880];
// Alias de CAMPO sobre los pools de items (verificado en el desensamblado de
// CloseInventoryRelatedWindows 0x4CBD36-0x4CBD9C y los errores de
// ida_get_function): en el binario no son copias sino el mismo pool abordado
// desde otro campo.  Los bucles originales escriben Type en `ptr - 0x38` y
// Key en `ptr`, o sea DAT_x + 0x38 = Key del slot 0 (no son arrays propios).
//   0x07EA5298 Inventory              0x07EA52D0 Inventory.Key
//   0x07EA7B88 OffsetTradeItems       0x07EA7BC0 OffsetTradeItems.Key
//   0x07EA9880 OffsetMixItems.Key     0x07EA8448 OffsetInventoryItems.Key
//   0x07EA5B68 Key del pool de 0x07EA5B30 (baul; en IDA tambien la tienda)
//   0x07E11FB0 Key de word_7E11F78 (trade del otro jugador)
// Son lvalues de array: `&`, la aritmetica y el decay a BYTE* funcionan igual.
#define DAT_07ea5298   (*(BYTE(*)[0x880])(Inventory))
#define DAT_07e11fb0   (*(BYTE(*)[0x880])(DAT_07e11f78 + 0x38))
extern DWORD   DAT_055c9b7c;
extern DWORD   DAT_07eaa164;

// ── Entity_Init globals ───────────────────────────────────────────────────────

// ── Scene_CharPreview globals ─────────────────────────────────────────────────
// IDA: pPickedItem.Level (0x07E91354) — el campo +4 del item levantado, no un
// global aparte.  Como variable propia valía siempre 0 y los chequeos de
// apilado comparaban contra nivel 0.
#define DAT_07e91354 (*(DWORD*)&pPickedItem[4])
extern char    DAT_07e9136b;
// Buffer del item del dialogo ShowCheckBox(153) -- 0x44 bytes, 0x07EA5240.
// DAT_07ea5244 (Level) y DAT_07ea525b (Option1) son campos DENTRO de el, no
// globals aparte: declararlos sueltos hacia que el memcpy del click derecho no
// los alcanzara nunca.  Mismo patron que DAT_081cb60c / DAT_083a42fc.
extern BYTE    DAT_07ea5240[0x44];
#define DAT_07ea5244   (*(DWORD*)(DAT_07ea5240 + 4))
#define DAT_07ea525b   (*(char*) (DAT_07ea5240 + 0x1b))

// ── Effect_Tick globals ───────────────────────────────────────────────────────
// IDA: MovePlanes usa Planes +0x60; no tiene otra asignación.
extern char    g_PointerPool[100 * 0x70]; // IDA: Pointers (0x07C82CD0).

// ── Terrain_Light globals ─────────────────────────────────────────────────────
extern DWORD   ActiveTerrain;   // IDA: ActiveTerrain (0x0839BC84)
// cb60c / 0828b60c / cb610 / 0828b610 NO son globals separados —
// son el 2do/3er DWORD del slot 0 de cb608 / 0828b608. Las macros están
// definidas junto a las declaraciones de cb608 / 0828b608, más arriba en este header.

// ── Scene_Resources string literals ──────────────────────────────────────────
// Nombres base de BMD/SMD que consumen AccessModel / OpenModel en Scene_Resources.
extern char    DAT_0055e834[8];     // "Ship"
extern char    DAT_005606ac[8];     // "Logo"
extern char    DAT_005607c0[8];     // "Face"
extern char    s_Interface_GFx_Interface2_jpg[];
extern char    s_Data_Interface_jpg[];
extern char    s_Data_Interface2_jpg[];
extern char    s_Object1_Object1_jpg[];
extern char    s_warrior_bmd[];
extern char    s_main_bmd[];
extern char    s_fairy_bmd[];

// ── Scene_Resources strings ───────────────────────────────────────────────────


// ── Additional misc globals ───────────────────────────────────────────────────
// DAT_07e016f0 — misma dirección que _DAT_07e016f0 (ya declarado arriba)
// DAT_07e01720 -> macro sobre el pool (ver abajo)   // Tooltip/bubble pool: 26 × 0x254 bytes
extern BYTE    DAT_0055984c[9];  // bone index table for glitter effect (Entity_UpdateRender)
extern DWORD   KeyState[256];  // key-state edge-trigger table  [IDA 0x07E118EC]
extern DWORD   DAT_07e11aac;
extern DWORD   DAT_07e11ab0;
extern DWORD   DAT_07e11ab4;
extern DWORD   DAT_07e11ab8;
extern DWORD   DAT_07e11da8;
extern DWORD   InventoryStartY;  // IDA: InventoryStartY (0x07EA5284)
extern DWORD   InventoryStartX;  // IDA: InventoryStartX (0x07EA5288)
extern DWORD   DAT_07ea9844;
// IDA Ambient_ParticleUpdate (0x00502320): el walker arranca en 0x083A2F78, pero
// cada slot empieza en `v1 - 58` (-0xE8) y el pool que le pasa a
// Particle_PathUpdate es &unk_83A2E90. O sea es la MISMA tabla que los boids,
// vista desde el ancla +0xE8. Como array propio, los campos con índice negativo
// del slot 0 caían 0xE8 bytes antes y pisaban g_PhysicsManager.
extern float   _DAT_00590af0;
extern DWORD   g_PhysicsManager[4];   // IDA: g_PhysicsManager (0x083A4338)
extern float   DAT_00590af4;   // 3D sound sin component (updated by Sound_UpdateChannel3D_Tick)
extern float   DAT_00590af8;   // 3D sound cos component (updated by Sound_UpdateChannel3D_Tick)
extern float   DAT_00590afc;   // cloth wind Z
extern float   _DAT_00559068;  // gravedad 9.8
extern float   _DAT_0055906c;  // dt fijo 0.0025
extern DWORD   g_iRenderTextType;   // IDA: g_iRenderTextType (0x055CA044) — modo TextOut del registro
extern DWORD   DAT_00561740;
extern DWORD   DAT_00561744;
extern DWORD   DAT_00561748;
extern DWORD   DAT_0056174c;
extern DWORD   DAT_00561750;

// ── Chat.cpp missing globals ──────────────────────────────────────────────────
extern char    DAT_07db8714;         // input field buffer byte +2
extern DWORD   DAT_07db8718;         // input field buffer dword +8 (char-select PIN data base)
extern char    DAT_07d73104[20000];  // word-filter table (1000 × 20 bytes, Filter.bmd)

// ── GlobalText[] localized string pool ────────────────────────────────────────
// 1000 slots × 300 bytes, los carga al arranque OpenTextData() desde
// Data/Local/Text.bmd (XOR con la clave de 3 bytes FC CF AB vía BuxConvert_0).
// Entries referenced across the codebase as strings like "Saldrás del juego
// en %d segundos." (index 380), error messages, UI labels, etc.
// GlobalText — el Text.bmd del 0.97k tiene EXACTAMENTE 1000 filas de 300 bytes
// (300000 bytes, verificado sobre el archivo).  Pero nuestro port heredo ~217
// indices DISTINTOS >= 1000 (hasta el 3305) de versiones posteriores, que con
// el array declarado [1000][300] eran LECTURAS FUERA DE RANGO: hasta ~690 KB
// pasado el final, entregadas a snprintf como si fueran cadenas.
// Se agranda el array a 4000 filas y se dejan las de arriba en cero: esas
// entradas devuelven cadena vacia y las rutinas de tooltip las saltean solas
// (todas chequean text[0]).  El loader sigue leyendo solo las 1000 reales.
#define GLOBALTEXT_ROWS_FILE   1000    // filas que trae el Text.bmd
#define GLOBALTEXT_ROWS        4000    // filas reservadas (el resto queda vacio)
extern char    GlobalText[GLOBALTEXT_ROWS][300];
void __cdecl OpenTextData(void);
int  __cdecl LoadTextData_Bin(const char *FileName);
// DAT_07d4b4b0/5dc son GlobalText[457]/[458] (name-filter blocked words, cargados
// de Text.bmd): `&DAT_07d4b4b0` = GlobalText[457].
// DAT_07d29d24: base de la tabla de nombres de clase, recorrida con
// `&DAT_07d29d24 + i * 300`.  Es una fila de GlobalText -- sus dos lectores
// (UI_StatsPanel y SecondPassword) usan indices ~601-607, que en Text.bmd son
// los nombres de clase.
#define DAT_07d4b4b0 (GlobalText[457][0])
#define DAT_07d4b5dc (GlobalText[458][0])
extern int     DAT_07d78074;         // command table A count (Chat_DrawEntry)
extern int     DAT_07d78070;         // command table B count (Chat_DrawEntry)
extern char    DAT_07d27610[20000];  // name-filter table (1000 × 20 bytes, FilterName.bmd)

// ── Terrain map globals ───────────────────────────────────────────────────────
// La celda original 0x083A0218 pertenece a g_ObjectBucketGrid (ver arriba).

// ── Terrain tile pick / normal computation globals ────────────────────────────
// (DAT_07feb25c..DAT_07feb284 são macros — ver g_TilePickBuf arriba)
extern DWORD   SelectFlag;  // IDA: SelectFlag (0x07EAB1FC) - tile pick result flag
extern DWORD   SelectWall;   // IDA: SelectWall (0x07E11D44) — combat target select sub-mode

// ── Model slot index globals ──────────────────────────────────────────────────
extern DWORD   TextureBegin;   // model slot active flag  [IDA 0x083A4104]
extern DWORD   TextureCurrent;   // model slot current index  [IDA 0x083A4108]

// ── Terrain culling globals ───────────────────────────────────────────────────
extern short   _DAT_0838b60a;  // terrain horizon/clip value
extern DWORD   _DAT_0838b614;  // terrain view scale (float bits)
extern DWORD   _DAT_0838b710;  // terrain LOD scale (float bits)
// _DAT_005528a8 ya declarado arriba (1/1000, de ms a s, misma dirección)

// ── Model loader format strings ───────────────────────────────────────────────

// ── Item/Skill/Gate data loader globals ──────────────────────────────────────
extern BYTE*   DAT_07cf1ff0;       // Base de la tabla sombra de atributos de items.
extern BYTE*   DAT_07cf1ff8;       // Base de los registros sombra de skills.
extern GATE_ATTRIBUTE* GateAttribute; // IDA 0x07CF5600: base de 100 registros de nueve bytes
// ── g_DialogScript @ 0x07CF5608 — 200 entradas x 0x400 bytes ────────────────
// Layout verificado contra el disassembly de CSQuest::ShowDialogText (0x4017E0):
//   0x4017F1  shl eax, 0Ah            → stride de entrada = 0x400
//   0x4017F6  add eax, g_DialogScript → m_lpszText en +0x000
//   0x401834  cmp [ecx+0x07CF5734]    → m_iNumAnswer      en +0x12C
//   0x40184D  add eax, 0x07CF5788     → m_lpszAnswer      en +0x180, paso 0x40
// y contra los xrefs de sub_401AF0 / UI_InGameMenu para los otros dos campos:
//   0x07CF5738 = m_iLinkForAnswer   (+0x130)  — índice del diálogo siguiente
//   0x07CF5760 = m_iReturnForAnswer (+0x158)  — acción de la respuesta (1/2/3)
// Cierra exacto: 0x180 + 10*0x40 = 0x400, y 0x130 + 10*4 = 0x158 + 10*4 = 0x180.
#define DIALOG_SCRIPT_COUNT   200
struct DIALOG_SCRIPT {
    /*+0x000*/ char m_lpszText[300];
    /*+0x12C*/ int  m_iNumAnswer;
    /*+0x130*/ int  m_iLinkForAnswer[10];
    /*+0x158*/ int  m_iReturnForAnswer[10];
    /*+0x180*/ char m_lpszAnswer[10][0x40];
};
extern DWORD   DAT_07cf5608[];     // base real de la tabla (DIALOG_SCRIPT_COUNT * 0x400)
#define g_DialogScript  ((DIALOG_SCRIPT*)DAT_07cf5608)
static_assert(sizeof(DIALOG_SCRIPT)                        == 0x400, "DIALOG_SCRIPT: stride de entrada");
static_assert(offsetof(DIALOG_SCRIPT, m_iNumAnswer)        == 0x12C, "DIALOG_SCRIPT: m_iNumAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_iLinkForAnswer)    == 0x130, "DIALOG_SCRIPT: m_iLinkForAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_iReturnForAnswer)  == 0x158, "DIALOG_SCRIPT: m_iReturnForAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_lpszAnswer)        == 0x180, "DIALOG_SCRIPT: m_lpszAnswer");
// Los cuatro DAT_ viejos son los campos de la ENTRADA 0, no globals aparte.
// extern char    DAT_07d566d0;       // fallback char name string (no-char placeholder)   // -> alias a GlobalText, ver el final del archivo
extern int     EditMonsterNumber;       // NPC name count  [IDA 0x07D78078]
// DAT_07cf2000 / DAT_07cf2001: alias a MonsterScript (misma tabla 0x07CF2000).
// Ver nota en globals.cpp.  &DAT_07cf2000 = base de la tabla; DAT_07cf2001 =
// primer byte del campo Name de la entrada 0.
extern DWORD   DAT_01c5e200;       // item/skill BMD checksum seed A
extern DWORD   DAT_00b43000;       // item/skill BMD checksum seed B
extern char    DAT_00559bb4[3];    // XOR cipher key for BuxConvert_0 (0xFC,0xCF,0xAB)
// Error message format strings
// File open modes
extern char    DAT_005597d4[];  // "wb" (binary write mode for crt_fwrite callers)

// ── SecondPassword Screen2 entity list globals ────────────────────────────────
// 0x07E11E80 = Party (stride 0x24). Era un DWORD suelto.
#define DAT_07e11e80 (*(DWORD*)Party)
extern DWORD   DAT_07ea5b24;   // Screen2 panel origin X
extern DWORD   DAT_07ea5b28;   // Screen2 panel origin Y

// ── SecondPassword UI sub-handler globals (ServerTransfer_HitTest / 004e9050 / 004eb5d0 / 004e6550) ──
extern DWORD   DAT_07eaa0c8;   // SecondPassword dialog origin X (pixel)
extern DWORD   DAT_07eaa0cc;   // SecondPassword dialog origin Y (pixel)
extern DWORD   MixState;  // IDA: MixState (0x07EAA140) - La etiqueta
                               // vieja ("SecondPassword timeout") era falsa: IDA lo
                               // llama MixState y sus writers son ReceiveMix (0x4366C0),
                               // el envio del mix y ReceiveTalk.
extern DWORD   RepairEnable; // IDA: DAT_07eaa138 (0x07EAA138)
extern DWORD   TradeInventoryStartX;  // IDA: TradeInventoryStartX (0x07EA5290) - SecondPassword alt-panel origin X
extern DWORD   TradeInventoryStartY;  // IDA: TradeInventoryStartY (0x07EA528C) - SecondPassword alt-panel origin Y
// Entradas de la tabla de strings que usa el switch de ChaosMix_HitTest:
// extern char    DAT_07d544d4;   // error string for case 0 (second password wrong)   // -> alias a GlobalText, ver el final del archivo
extern char    DAT_07eaa1a0;   // UI message label A (UIChatLogWindow_AddText arg1)
// extern char    DAT_07d54600;   // error string for auth-fail case   // -> alias a GlobalText, ver el final del archivo
extern char    DAT_07eaa198;   // UI message label B
// extern char    DAT_07d55c44;   // error string for case 0xfffffff8/0xfffffffe   // -> alias a GlobalText, ver el final del archivo
extern char    DAT_07eaa19c;   // UI message label C
// DAT_0055a3f8 / DAT_0055a3fc — iVar1/iVar3 defaults for Inventory_HasFreeSpace non-1/7/0xb/8 cases
extern int     DAT_0055a3f8;   // auth mode param A
extern int     DAT_0055a3fc;   // auth mode param B

// ── SecondPassword Screen5/6/7 additional globals ────────────────────────────
extern "C" char    DAT_07eaa0dc;   // SecondPassword selected grid index (byte)
// Buffers del editor de creacion de GUILD (no "PIN entry"): el render los recorre
// como arrays (64 bytes de mark). Ver la nota en globals.cpp.
extern char    DAT_07ea51ec[33];   // GuildName (0.97.40: 32 bytes + NUL)
extern "C" char    DAT_07ea51f5[64];   // GuildMark (grilla 8x8, 1 byte por celda)
// extern char    DAT_07d6b724;   // Error message: "no item in slot"   // -> alias a GlobalText, ver el final del archivo
// extern char    DAT_07d685ec;   // Error message: "invalid slot" (EventEntrance_HitTest)   // -> alias a GlobalText, ver el final del archivo
// DAT_00559f60 / DAT_00559f64 SON m_iDevilSquareLimitLevel.
// Verificado con ida_get_function: m_iDevilSquareLimitLevel = 0x00559F60 y
// m_iBloodCastleLimitLevel = 0x00559F80 (32 bytes despues = 4 niveles x 2 int).
// El handler del 0x8E llena el array y `EventEntrance_HitTest` lee
// `(&DAT_00559f60)[i*2]`: tienen que ser la misma memoria.
// DAT_07ea7b88 — declared above as DWORD

// BMD_DrawMesh / BMD_DrawBoneSlot_Anim (BMD__RenderMesh / BMD__RenderMeshTranslate) buffers
// LightTransform pool (see globals.cpp). Backed by g_BoneLightBuf.
// DAT_060db65c es un lvalue char en el offset 0 del buffer; &DAT_060db65c + k avanza
// k bytes (aritmética de bytes) — lo exigen los accesos de BMD_DrawMesh.cpp.
extern char    g_BoneLightBuf[32 * 15000 * 12];
#define DAT_060db65c  (*(char*)(g_BoneLightBuf + 0))
// Chrome UV scratch — backed by g_ChromeUVBuf. &DAT_05828d5c = &g_ChromeUVBuf[0]
// (float*). (&DAT_05828d5c)[i] = g_ChromeUVBuf[i]; +1 float = V slot (old d60).
extern float   g_ChromeUVBuf[15000 * 2];
#define DAT_05828d5c  (g_ChromeUVBuf[0])
// Transformed-normal buffer — backed by g_BoneChromeNormalBuf. DAT_06f433bc = base
// (destino de escritura de VectorRotate), DAT_06f433c0 = base+4 (lectura del pre-loop de chrome; pfVar8[-1]
// = normal.x en base, [0] = normal.y en base+4, [1] = normal.z en base+8).
extern char    g_BoneChromeNormalBuf[32 * 15000 * 12];
#define DAT_06f433bc  (*(char*)(g_BoneChromeNormalBuf + 0))
#define DAT_06f433c0  (*(float*)(g_BoneChromeNormalBuf + 4))

// _DAT_ float constants missing from earlier declarations
extern float   _DAT_00552544;  // alpha threshold (alpha < this → use alpha channel path)
extern float   _DAT_005528c0;  // chrome U scale factor

// ── MoveEffect constants (0x005524a8–0x00552a20 range) ─────────
extern float   _DAT_005529d4;  // move effect float constant
extern char    DAT_00559b78[8];
#define DAT_00559b7f (DAT_00559b78 + 7)   // alias: mismo buffer, offset 7

// ── Map_LoadObjectModels (0x0050c4d0) ─────────────────────────────────────────
extern FILE*   ParserFileHandle;       // DAT_083A40FC — file handle for custom-map object list
extern char    ParserTokenString[256]; // DAT_083A3FF4 — ParseNextToken output buffer
extern int     ParserCurrentToken;     // DAT_083A40F4 — last token type code
extern float   ParserTokenNumber;      // DAT_083A40F8 — last numeric value
extern char    DAT_083a4100;          // flag: Lorencia object models already loaded
// DAT_0055e714 — format string for custom map object file path (zone index)
// DAT_005580ac — declared above (file open mode char)
// DAT_083a410c, DAT_0055a7c4, World, DAT_05828d58 — declared above

// ── RenderObjectScreen (0x004e13a0) ──────────────────────────────────────────
// Estos globals guardan la pose de render por frame del item/objeto que se está mostrando.
extern float   ObjectSelect_Angle;         // item render: rotation X offset  [IDA 0x07EA952C]
extern float   _DAT_07ea9530;         // item render: rotation Y
extern float   _DAT_07ea9534;         // item render: rotation Z
extern short   ObjectSelect_Type;         // item render: resolved model type index  [IDA 0x07EA9512]
extern float   ObjectSelect_HeadAngle;         // item render: HeadAngle[0] (used by BMD_Animation)  [IDA 0x07EA9538]
extern int     ObjectSelect_AnimationFrame;         // item render: state flag A  [IDA 0x07EA9618]
extern int     ObjectSelect_PriorAnimationFrame;         // item render: state flag B  [IDA 0x07EA961C]
extern short   ObjectSelect_PriorAction;          // item render: state flag C  [IDA 0x07EA9616]

// ── Filter / name-filter system globals ──────────────────────────────────────
extern DWORD   DAT_007cfa00;    // word-filter BMD checksum seed
extern DWORD   DAT_00578200;    // name-filter BMD checksum seed

// ── HeroTile — terrain tile type under the hero character ─────────────────────
// HeroTile ya declarado como DWORD cerca de la línea 546 (tipo de mundo/mapa)
// World ya está declarado arriba.
#ifndef World

// ── Pools de items y atributos de terreno ────────────────────────────────────
// Centralizadas acá (un único `extern` por global) para que mover funciones entre
// módulos no rompa la visibilidad de las globals.
//
// Los cuatro pools de items son grids de slots ITEM (stride 0x44). Ojo: su
// indice de celda es `slot - 12`; los 12 wear slots NO viven aca sino en
// `CharacterMachine + 536 + 68*slot`.
extern "C" BYTE  OffsetInventoryItems[];
extern "C" BYTE  OffsetTradeItems[];
extern "C" BYTE  OffsetWarehouseItems[];
extern "C" BYTE  OffsetMixItems[];
// 256x256 flags de tile: bit0 safezone, bit1 ocupado por entidad, bit3 no-caminable.
extern unsigned char TerrainWall[0x10000];   // IDA: TerrainWall (0x0838BC70) — atributos por casilla (TW_*)

#endif

// ── SkillAttribute @ 0x07D29D20 — 64 entradas × 40 bytes ─────────────────────
// IDA: SkillAttribute (0x07D29D20).  En el binario es un PUNTERO a un bloque
// `new(0xA00)` (`mov ecx, dword_7D29D20` en OpenSkillScript 0x47AC50).  Acá es
// la tabla estática; el port tenía además el puntero `DAT_07d29d20`, malloqueado
// en ceros: el cargador llenaba ésta y el tooltip de skills leía el otro.
// WinMain reserva 0xA00 (= 2560) bytes y OpenSkillScript / el anti-tamper copian
// de a 0x28 (40) bytes por entrada, así que la stride es 40 — no 300.  Los
// consumidores indexan por BYTE: `&SkillAttribute[8 * (5*Level + 150)]`
// (= entrada 30+Level), con el nombre en el offset 0 de la entrada.
struct _SkillAttrEntry { char Raw[2560]; };
extern _SkillAttrEntry SkillAttribute;  // base of table @ 0x07D29D20

// ── CharactersClient extra BMD heap buffer ────────────────────────────────────
extern LPVOID  DAT_07abf164;   // extra BMD data heap block for character models

// ── FloatingLabel pool ────────────────────────────────────────────────────────

// ── Water wave simulation buffers ─────────────────────────────────────────────
extern char    DAT_0814b2e0[0x80000];   // grass-wind/water-wave double buffer (2×0x40000, sub_4F98C0/4F9A30)

// ── Small-function batch globals ─────────────────────────────────────────────
// Base de la tabla de quests (el objeto CSQuest al que apunta g_csQuest) — IDA la
// trata como un buffer de ~0x1C900 bytes y los accesos llegan hasta cerca del final
// (p.ej. el flag del panel de quest en +0x1C87F que lee GetScreenWidth). Por eso es
// un array de BYTE del tamaño real y no un DWORD.
extern BYTE    DAT_00567500[0x1C900];   // Quest table base (was DWORD)
extern DWORD   DAT_00590b00;       // Sound device context
extern DWORD   DAT_055c9b78;       // RefCount / tick counter

extern int     PTR_LAB_00552460;   // vtable for FUN_00403310
extern int     PTR_LAB_005524e8;   // vtable for ClothNode_SetVTable
extern int     PTR_LAB_005527e4;   // vtable for FUN_0040f690
extern int     PTR_LAB_005524b8;   // vtable for Quest_SetVTable
extern int     PTR_FUN_005524c0;   // vtable for CWaveFile
extern int     PTR_FUN_005524c4;   // vtable for CErrorReport
extern int     PTR_FUN_005524d8;   // vtable for HashTable
extern int     PTR_LAB_00552810;   // vtable for StreamBase2
extern int     PTR_FUN_005527e0;   // vtable for Stream deinit
extern int     PTR_FUN_0055389c;   // vtable for GameGuard class

// Símbolos de vtable/literal-puntero off_XXXX de IDA, renombrados por port.py a DAT_xxxxxxxx.
// (en paralelo a los PTR_LAB_/PTR_FUN_ de arriba; port.py usa la forma DAT_)
extern DWORD   DAT_00552508;


extern HANDLE  hEvent;                  // GameGuard sync event
extern LPVOID  lpParameter;             // IDA: lpParameter (0x083BBAE8) — contexto de
                                        // GameGuard (0x34C bytes) que aloca el pre-init;
                                        // != 0 significa "ya inicializado".
extern char    g_GameGuardGameName[];   // IDA: aMu (0x0055910C) — el nombre de juego que
                                        // recibe PreInitNPGameMon: "Mu". NO es un nombre de
                                        // ventana: el cliente no crea ninguna ventana de
                                        // GameGuard.

extern uintptr_t g_csQuest;      // Dirección del objeto CSQuest (0 = inactivo).

// Match/DevilSquare info
extern BYTE    m_byMatchType;
extern int     m_iMatchTimeMax;
extern int     m_iMatchTime;      // 0x00559CCC (writers: SetMatchInfo / clearMatchInfo)
extern int     m_iMaxKillMonster;
extern int     m_iKillMonster;

// Sound system (DirectSound8 pipeline — companion-project layout, 420 slots)
extern BOOL    g_EnableSound;                         // DAT_00590ac8 — master on/off
extern bool    g_Enable3DSound;                       // DAT_00590AC9 — 3D audio enabled
extern LPDIRECTSOUND          g_lpDS;                 // DAT_00590AD0 — main device
extern LPDIRECTSOUND3DLISTENER g_lpDS3DListener;      // primary-buffer listener
extern void*   wavefile;                              // DAT_00590AD8 — CWaveFile* currently being loaded
extern DWORD   g_dwBufferBytes;                       // DAT_00583DA8 — last WAV data chunk size

// Tablas por slot (stride 4, 420 entradas cada una — contiguas en la sección .data original):
extern int     MaxBufferChannel[420];                 // 0x0058E1C4 — # active channels (>0 means "loaded")
extern int     BufferChannel[420];                    // rotating channel index (0..MaxBufferChannel-1)
extern bool    Enable3DSound[420];                    // 0x00590924 — per-slot 3D flag
extern int     SoundLoadCount;                        // 0x00590ACC — running count of loaded sounds
extern char    BufferName[420][0x40];                 // per-slot filename (64 bytes)

extern LPDIRECTSOUNDBUFFER   g_lpDSBuffer  [420][4];  // 0x0058C780 — secondary buffers (up to 4 instances per slot)
extern LPDIRECTSOUND3DBUFFER g_lpDS3DBuffer[420][4];  // 0x0058EEE4 — parallel 3D-buffer array
extern const OBJECT* Object3DSound[420][4];          // Entidad ligada a cada canal de audio 3D.

// Music

// Entity data
extern BYTE    MonsterScript[512 * 0x36];  // NPC/mob name table (stride 0x36; Type[0], Name[1..32])
extern char    WhisperRegistID[10][41]; // IDA: WhisperRegistID (0x07DB9310) — anillo de 10 nombres (0.97.40: 40 bytes + NUL)
extern int     WhisperID_Num;           // IDA: WhisperID_Num (0x07E11DB0) — proxima fila del anillo

// Blur/trail system
// DAT_07c608b8 — campo int en el +16 del slot[0] de g_RenderPool_07c608a8 (ver arriba).
#define DAT_07c608b8  (*(int*)(&g_RenderPool_07c608a8[16]))


// ── Batch 3 globals ──────────────────────────────────────────────────────────
// BST / std::map sentinel
extern void   *DAT_055c9b98;       // RB-tree sentinel node (NIL)

// Camera
extern float   CameraAngle[3];     // DAT_083a42e0 (3 floats)
extern float   CameraPosition[3];  // DAT_083a42d4 (3 floats)

// Quest/NPC window
// g_bEventChipDialogEnable es 0x07EAA128 (GoldenArcherOpenType): alias, para que
// el 0x94 (writer) y el panel del Golden Archer (reader) vean el mismo global.
#define _g_bEventChipDialogEnable (*(int*)&GoldenArcherOpenType)

// Los flags de los paneles de UI aliasan los bytes reales DAT_07eaa11x (per el
// Offsets.h del proyecto companion de IDA). Los flags reales están en
// CharacterOpened..11c (de un byte), no en 0x07e5ba84/88 (misidentificación de la
// época de Ghidra). Usamos #define para que tanto el código de toggle portado de
// IDA (escribe InventoryOpened) como los gates de render del HUD (leen
// `if (InventoryOpened)`) peguen en el mismo byte de memoria.

// Inventory/Trade panel origin coords. Mismo patrón de unificación que los flags
// *Opened de arriba — IDA escribe/lee vía InventoryStartY..5290 y los pases de render
// del HUD usan los nombres de C++: tienen que ser la misma memoria.

// SMD parser
extern FILE   *SMDFile;             // DAT_0055c0a0

// Codec / JPEG internal refs (function pointers stored as addresses)
// strID already declared above (line ~1036) as DWORD
// DAT_00560694 already declared above (line ~1616) as DWORD

// Missing globals (compilation fixes)
// IDA: RenderTerrainAlphaBitmaps usa g_PointerPool +0x24 y 100 slots.
// m_dwTextColor / m_dwBackColor SON DAT_00559c78 / SetBackgroundTextColor (mismo global en IDA:
// 0x559c78 / 0x559c80, verificado por disasm de sub_40D610 y sub_480980). El render de
// texto (CUIRenderText_RenderText) lee DAT_00559c78; sin esta unificación los seteos de m_dwTextColor
// (HUD passes + ChatListBox) se perdían → texto blanco. DAT_00559c78/80 declarados arriba.
#define m_dwBackColor  SetBackgroundTextColor
// g_iNumAnswer — alias de g_iNumAnswer (ver arriba).
// g_lpszDialogAnswer — declarado arriba.
// m_hFontDC (IDA 0x055C9FEC) y los bits del DIB del texto (0x055C9E4C) viven en
// CFont: gFont.GetTextDC() / gFont.GetTextBits().
// Las tres fuentes (IDA g_hFont 0x055CA00C, g_hFontBold 0x055CA010, g_hFontBig
// 0x055CA014) viven en CFont: gFont.GetFont(FONT_NORMAL / FONT_BOLD / FONT_BIG).

// Batch 18 — InitGame / ReceiveChat globals
// EnableUse declarado arriba junto a DAT_05826d1c.
extern int     SendGetItem; // IDA: DAT_07e11998 (0x07E11998)
// MouseUpdateTime already declared above (line ~554) as DWORD
extern int     DAT_07e11990;       // SelectedOperate
// DAT_07e1198c already declared above (line ~1003) as DWORD
// DAT_07e11984 already declared above (line ~1002) as DWORD
// m_bAutoAttack vive en 0x00559C5C (m_bAutoAttack, IDA InitGame L39).  El port
// tenia ademas DAT_07e11e18: el toggle del menu y el hover escribian uno y el
// combate / F3/30 leian el otro.  Ahora es un alias.
extern int     DAT_07e11d24;       // _CheckInventory
// LoadingWorld already declared above (line ~553) as DWORD
extern BYTE    DAT_00559c6d;       // UI alpha/state byte
// g_shEventChipCount es 0x07EAA12C (GoldenArcherItemCount); mismo caso que el de arriba.
#define DAT_07e11e1c (*(short*)&GoldenArcherItemCount)
extern short   DAT_07e11e20;       // g_shMutoNumber[0]
extern short   DAT_07e11e22;       // g_shMutoNumber[1]
extern short   DAT_07e11e24;       // g_shMutoNumber[2]

// Batch 19 — SendCheck globals
extern BYTE    g_byPacketSerialSend; // IDA: g_byPacketSerialSend (0x07DB8600)

// Batch 20 — OpenNpc, MoveCamera, RenderEquipment3D, RenderItems3D, Send_ActionRequest, LookAtTarget
// Sólo los globals realmente nuevos (los que no estaban declarados arriba):
extern float   DAT_083a7adc[];     // CurrentCameraWalkDelta[6] (pos delta + angle delta)
extern void*   g_LoginSceneObjects[9]; // exact CreateObject pointers for login bg: sky, 3 ships, 3 waves, banner, sun

// Batch 22 — AttackStage, CreateArrow
extern int     g_iLimitAttackTime;       // g_iLimitAttackTime (attack frame limit counter)  [IDA 0x00559858]
extern DWORD   CurrentSkill; // IDA: CurrentSkill (0x05826D10)

// Batch 21 — RenderSkillIcon, CheckMixRecipe, MoveObjects, CreateArrows, CollisionDetectLineToMesh
extern DWORD   DAT_083a3fec;       // visible object counter (MoveObjects)
extern DWORD   DAT_07eaa178;       // mix recipe: socket flag byte
extern DWORD   DAT_07eaa170;       // mix recipe: wing socket option value
extern int     DAT_07eaa174;       // mix recipe: combined value score (capped at 0x5a)
extern char    DAT_00559c6c;       // last rendered hotkey char

// ── UseSkillWizard dependencies ──────────────────────────────────────────────
extern DWORD   MovementSkillTarget;       // MovementSkillTarget (index into CharactersClient)  [IDA 0x07D780A0]
extern DWORD   DAT_07d7809c;       // skill slot index (current selected skill slot)
extern char    DAT_00559d94[8];    // GM name string "webzen" (anti-impersonation check 1)
extern char    DAT_00559d9c[8];    // GM name string "webzen" (anti-impersonation check 2)

// ── SkillElf dependencies ────────────────────────────────────────────────────
extern char    DAT_07e11dfc;       // chat log widget ID string (for AddText)
// extern char    DAT_07d4c89c;       // "Not enough mana" message string   // -> alias a GlobalText, ver el final del archivo

// MoveParticles drift vectors, independent of EarthQuake (0x083A0210).
// IDA 0x47709A..0x477193 indexes the first two components of each vector.
extern float g_ParticleDriftPosition[3]; // 0x07C800F8..0x07C80103
extern float g_ParticleDriftVelocity[2]; // 0x07C80104..0x07C8010B
#define DAT_07c800f8 (g_ParticleDriftPosition[0])
#define DAT_07c800fc (g_ParticleDriftPosition[1])
#define DAT_07c80100 (g_ParticleDriftPosition[2])

// ── MoveParticles float constants ────────────────────────────────────────────
// _DAT_00552acc — already declared above (line ~958)
// _DAT_00552adc — already declared above (line ~151)

// ── Additional math/render constants (added for CreateAngle/Terrain_RenderTexturedObject) ──

// ── CheckArrow / CharSelect chat message string globals ──
extern char    DAT_07e11df4;        // arrow-required error string ID
extern char    DAT_07e11df8;        // bolt-required error string ID

// ── Skill selection globals ──
extern char    DAT_07d78098;        // skill mode flag (0=direct, nonzero=from attribute)
extern DWORD   InputEnable;          // IDA: DAT_00559c84 (0x00559C84)

// ── CharSelect packet globals ──
extern DWORD   SelectedHero;         // IDA: DAT_005616ac (0x005616AC)

// ── Chat bubble pool ──
// Pool base = 0x07E016F8; stride 0x254 per entry; ~96 slots.
// Per-slot layout:
//   +0x000 char[24]  ID (sender name)
//   +0x018 char[12]  guild mark string  ("[GuildName]")
//   +0x024 char      color index
//   +0x025 char      guild team byte
//   +0x028 int       timer1 (tick counter for line 1)
//   +0x02c char[256] text line 1
//   +0x12c char[256] text line 2
//   +0x22c int       display timer 1
//   +0x230 int       display timer 2
//   +0x234 DWORD     owner CHARACTER* pointer
extern char    DAT_07e016f8[100 * (0x254 + sizeof(void*) - 4)];   // pool de burbujas: 100 CHAT (596 B en x86)
// unk_7E01720 = base + 40 (campo timer1). MoveChat arranca su walk ahi.

// ── HUD render globals (Phase-2 port) ────────────────────────────────────────
// Para el port de RenderPartyHP / RenderMainFrameWindow / RenderBooleans /
// Render_HotbarItems3D from IDA.
//
// Estado de party / soccer / guild war — binding mínimo para que los renderers
// sigan funcionando antes de que se porten los sistemas completos de party/guild.
// Los valores por defecto en 0 hacen que todos esos caminos tomen sus ramas de
// salida temprana, así que el juego corre igual con los ports incompletos.
extern int     PartyNumber;          // 0x07EAA0E0 — count of valid party slots
extern int     PartyKey;             // IDA: PartyKey (0x07EAA0E4) — reiniciado por InitPartyList
extern BYTE    Party[2048];          // 0x07E11E80 — Party struct array (stride 36, header 24)
extern int     EnableGuildWar;       // IDA: EnableGuildWar (0x05826D30)
extern int     EnableSoccer;         // IDA: EnableSoccer (0x05826D31) — modo fútbol de guerra de guild
extern int     GuildWarIndex;        // IDA: GuildWarIndex (0x00559684) — fila de tabla de marks de guild enemiga
extern int     HeroSoccerTeam;       // IDA: HeroSoccerTeam (0x05826D32) — equipo del héroe en el fútbol
extern int     GuildWarScore[2];     // [self, enemy]
extern char    GuildWarName[80];     // IDA: GuildWarName (0x05826BFC) — guild enemiga
extern char    SoccerTeamName[2][80];// team names

// Float-text floating-numbers entry pool (RenderBooleans iterator).
// Layout per entry (596 bytes = 149 dwords):
//   +0x000 (DWORD)  tipo/flag (se lee como `*((int*)v0 - 2)` y `*((int*)v0 - 131)`)
//   +0x008 (DWORD)  owner ptr (entity*)
//   +0x010 (3*float) world position fallback
//   +0x224 (DWORD)  extent de texto cx (lo setea sub_480C60)
//   +0x228 (DWORD)  text-extent cy
//   +0x22C..       text strings: at +44, +300, +24, +base
//   +0x22C (int)   countdown-1
//   +0x230 (int)   countdown-2
// Pool spans 0x07E01924..0x07E101F4 (~60 KB, ~100 entries).

// Globals de fuente / medición de texto que consumen sub_480C60 y el HUD
// renderers (HFONT object handles + DC + computed dimensions).
// FontHeight (0x07D78080): gFont.GetHeight().
extern SIZE    TextSize;             // shared scratch SIZE for text extent

// Anti-tamper hash-table state — already declared above as g_HashTableCtx[4]
// con las macros MAIN_HASH_CLASS/cc/d0/d4. Sólo referenciarlos; NO volver a hacerles extern.

// CharacterAttribute se declara mas arriba.

// DAT_07e11d6e (flag de UI sucia) ya está declarado en la línea 620 como `char`.


// ─── Filas de GlobalText que el port habia partido en globals sueltos ────────
// El pool de textos vive en `GlobalText[1000][300]` con base
// 0x07D29D24 (verificado: 0x07D4B4B0 == GlobalText[457]).  Estos simbolos caen
// EXACTAMENTE en multiplos de 300 desde esa base, o sea son filas del pool, no
// buffers propios.
//
// Mismo patron que DAT_081cb60c: un macro que proyecta dentro del array real,
// asi `&DAT_x` sigue siendo un `char*` a la fila.
#define DAT_07d3bfc4       (GlobalText[248][0])
#define DAT_07d3c0f0       (GlobalText[249][0])
#define DAT_07d3c6cc       (GlobalText[254][0])
#define DAT_07d3c924       (GlobalText[256][0])
#define DAT_07d3cb7c       (GlobalText[258][0])
#define DAT_07d3cca8       (GlobalText[259][0])
#define DAT_07d3cdd4       (GlobalText[260][0])
#define DAT_07d3d284       (GlobalText[264][0])
#define DAT_07d3d3b0       (GlobalText[265][0])
#define DAT_07d3d608       (GlobalText[267][0])
#define DAT_07d3d734       (GlobalText[268][0])
#define DAT_07d46e60       (GlobalText[397][0])
#define DAT_07d493e0       (GlobalText[429][0])
#define DAT_07d4b12c       (GlobalText[454][0])
#define DAT_07d4b258       (GlobalText[455][0])
#define DAT_07d4b384       (GlobalText[456][0])
#define DAT_07d4b708       (GlobalText[459][0])
#define DAT_07d4c89c       (GlobalText[474][0])
#define DAT_07d530e8       (GlobalText[563][0])
#define DAT_07d53214       (GlobalText[564][0])
#define DAT_07d544d4       (GlobalText[580][0])
#define DAT_07d54600       (GlobalText[581][0])
#define DAT_07d55c44       (GlobalText[600][0])
#define DAT_07d58ea8       (GlobalText[643][0])
#define DAT_07d6813c       (GlobalText[850][0])
