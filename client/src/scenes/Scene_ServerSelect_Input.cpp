// Scene_ServerSelect_Input.cpp — CServerSelWin_UpdateWhileActive @ 0x0051e7e0
// Server-select UI: mouse hit-test for group buttons + individual server entries.
//
// Called each frame during server select to handle mouse clicks.
// Server list: ServerList, stride 0x21e, 25 entries.
//   +0x00: server name (null-terminated)
//   +0x14: active flag (ServerList[i].Number)
//   +0x15: group flag: 0=group1, 1=group2 (ServerList[i].extServer)
//
// Screen layout (X axis):
//   Group 1 button:  X = 0x96..0xdc  (150..220)
//   Group 2 button:  X = 0x177..0x1bd (375..445)
//   Server entries:  X = 0xe5..0x16c  (229..364), Y stacked by 0x14 per entry
//
// Mouse position: MouseX = cursor X, MouseY = cursor Y
// Click flag:     MouseLButtonPush — set by WndProc on left click, cleared here after handling
//
// On group button click:
//   Sends 4-byte packet [0xC1][0x04][0xF4][?] to select server group.
//   Saves selected group index to ServerSelectHi; resets ServerSelectLo = -1.
//   Calls PlayBuffer(0x19, 0, 0) (BGM_Play).
//
// On server entry click (if group already selected, ping < 100, no cooldown):
//   Saves channel = (port % 0x14 + 1) to ServerLocalSelect.
//   Sends 6-byte packet [0xC1][0x06][0xF4][?][portLo][portHi].
//   Sets connect timer: DAT_083a7ac8 = GetTickCount().
//   Calls UIChatLogWindow_AddText twice to copy IP/port into connection context.
//   Sets cooldown: DAT_083a7c44 = 0x32 frames.
//
// Packet buffer layout (stack, sent via send() with WOULDBLOCK fallback):
//   Group select (4 bytes):  { 0xC1, 0x04, 0xF4, subcode }
//   Server select (6 bytes): { 0xC1, 0x06, 0xF4, subcode, portLo, portHi }
//
// Related globals:
//   ErrorMessage / _7c28 — UI flash/cooldown counters
//   DAT_083a7c44          — server click cooldown (decremented per frame)
//   DAT_083a4320          — reset to 0 on server select
//   DAT_083a4328          — updated with selected port

#include "stdafx.h"
#include "platform/ITimer.h"
#include "globals.h"
#include "functions.h"
#include "net/Net.h"
#include "platform/PlatformHeaders.h"

// ─────────────────────────────────────────────────────────────────────────────

// IDA: CServerSelWin::UpdateWhileActive (0x0051E7E0)
void CServerSelWin_UpdateWhileActive(void)
{
    // Re-enabled: la lista estática poblada en WinMain (entry "MuServer") hace
    // que el loop tenga algo que procesar. Sin populate el Pass 1 no encontraba
    // entries y el `do...while ((int)local_830 < 0x83a7adb);` podía divergir —
    // por eso el AUTO-SKIP original. Ahora que hay ≥1 entry, el loop termina
    // correctamente en 24 iteraciones.
    // ── Pass 1: classify servers into group1 / group2; compute button Y coords ─
    int iVar8  = 0;   // group1 count
    int iVar10 = 0;   // group2 count

    // IDA: for ( i = 0; i < 13550; i += 542 )  — los 25 grupos de ServerList.
    for (int i = 0; i < MAX_SERVER_HI; ++i) {
        SERVER_LIST_t& srv = ServerList[i];
        if (srv.Number != 0) {
            // Check if server name ends with '2' → group 2
            // IDA: unk_83A45D7 + i + strlen(&ServerList[i]) == 50   (50 = '2')
            const char *name = srv.Name;
            int len = 0;
            while (name[len]) len++;
            if (len > 0 && name[len - 1] == '2') {
                srv.extServer = true;
                iVar10++;
            } else {
                srv.extServer = false;
                iVar8++;
            }
        }
    }

    // Y position base for each group column
    int iVar12 = iVar8 * -0x10 + 0x1bc;
    if (iVar12 > 0xdc) iVar12 = 0xdc;
    int iVar4 = ((iVar8 - iVar10) - 1) * 0x10 + iVar12;

    // Y base guardada ANTES de que Pass 2 mute iVar12/iVar4.
    // IDA: v38 = v3 (non-PVP base), v40 = v7 (PVP base) — usadas en Pass 3.
    const int iYNonPvpBase = iVar12;
    const int iYPvpBase    = iVar4;

    int local_834 = 0;
    int local_820 = iVar12;
    int local_824 = iVar4;

    // ── Pass 2: hit-test group buttons ───────────────────────────────────────
    do {
        if (ServerList[local_834].Number != 0) {
            int iVar9, iVar3, iVar6;
            if (!ServerList[local_834].extServer) {
                // Group 1 entry
                iVar6 = 0x96;
                iVar9 = iVar12;
                iVar3 = iVar12 + 0x10;
                iVar12 = iVar3;
                local_820 = iVar3;
            } else {
                // Group 2 entry
                iVar6 = 0x177;
                iVar9 = iVar4;
                iVar3 = iVar4 + 0x10;
                iVar4 = iVar3;
                local_824 = iVar3;
            }

            // Check mouse in button rect and click pending
            if (iVar6 <= (int)MouseX && (int)MouseX < iVar6 + 0x46 &&
                iVar9 <= (int)MouseY && (int)MouseY < iVar3 &&
                MouseLButtonPush != '\0')
            {
                CErrorReport_Write(&DAT_055c9bf0, "> Server group selected");
                MouseLButtonPush = '\0';

                // Build and send 4-byte group-select packet
                // IDA: buf[2]=-63(0xC1), buf[3]=4(len), v49=-12(0xF4), v50=2(subcode)
                // Ghidra malinterpretó v50 como 0xC1 (era 2). F4/02 = server list req.
                char pkt[4];
                pkt[0] = (char)0xC1;
                pkt[1] = 4;
                pkt[2] = (char)0xF4;
                pkt[3] = 2;            // subcode 2 (F4/02 = request server list)

                gNetwork.SendToConnectServer((const BYTE*)pkt, 4);

                ServerSelectHi = local_834;
                ServerSelectLo = -1;
                PlayBuffer(0x19, 0, 0);
                iVar12 = local_820;
                iVar4  = local_824;
            }
        }
        local_834++;
    } while (local_834 < MAX_SERVER_HI);   // IDA: 25 entradas de 0x21e

    // ── Pass 3: hit-test individual server entries in selected group ──────────
    if ((int)ServerSelectHi == -1) goto done;

    {
        SERVER_LIST_t& sel = ServerList[ServerSelectHi];
        int iVar8b;

        // IDA Pass 3 uses v38 (non-PVP base) / v40 (PVP base), NOT the mutated
        // v4/v7. Mismatch de 16px hacía que ciertas zonas del hit-area no
        // coincidieran con el render.
        unsigned char numCh = sel.Number;
        if (!sel.extServer) {
            // Non-PVP server: cnt = posición en la lista non-PVP (antes de este)
            // Para single-entry, cnt=0. Scan strcmp omitido (solo relevante con >1 server).
            int cnt = 0;
            iVar8b = cnt * 0x10 + (int)numCh * -10 + 8 + iYNonPvpBase;
        } else {
            // PVP server
            int cnt = 0;
            iVar8b = cnt * 0x10 + (int)numCh * -10 + 8 + iYPvpBase;
        }

        int server_count = (int)numCh;   // reuse byte-correct numCh from above
        if (server_count == 0) goto done;

        // Max Y extent check
        int iVar12b = (0x18 - server_count) * 0x14;
        if (iVar12b <= iVar8b) iVar8b = iVar12b;

        ServerSelectLo = -1;
        int local_834b = 0;

        do {
            // Check mouse in entry rect (X=0xe5..0x16c) and server not full (flag & 0x80 == 0)
            if (0xe5 <= (int)MouseX && (int)MouseX < 0x16c &&
                iVar8b <= (int)MouseY && (int)MouseY < iVar8b + 0x14)
            {
                ServerLocalSelect = (unsigned int)sel.Server[local_834b].Index % 0x14 + 1;
                ServerSelectLo = local_834b;

                bool full = (sel.Server[local_834b].Percent & 0x80) == 0x80;
                bool ping_ok = (sel.Server[local_834b].Percent & 0x7f) < 100;

                if (!full && MouseLButtonPush != '\0') {
                    MouseLButtonPush = '\0';
                    PlayBuffer(0x19, 0, 0);

                    if (ping_ok && DAT_083a7c44 == 0) {
                        ServerLocalSelect = (unsigned int)sel.Server[local_834b].Index % 0x14 + 1;
                        CErrorReport_Write(&DAT_055c9bf0, "> Server selected");

                        DAT_083a7c44 = 0x32;
                        ErrorMessage = NextErrorMessage;
                        NextErrorMessage = MESSAGE_NONE;
                        DAT_083a7ac8 = Platform::GetTimer().Ticks();
                        DAT_083a4320 = 0;

                        // Selected server's ServerCode (channel_id, +0x2c field).
                        unsigned short serverCode = sel.Server[local_834b].Index;
                        DAT_083a4328 = (DAT_083a4328 & 0xffff0000) | serverCode;

                        if (gNetwork.IsConnectServerMode()) {
                            // ── Flujo ConnectServer (fiel al binario original) ──
                            // Mandar F4/03 (server-info request) al ConnectServer.
                            // Responde F4/03 con IP:port del GameServer →
                            // Recv_Redirect (Net_Process.cpp) desconecta del CS,
                            // conecta al GameServer y arranca el login (JoinServer).
                            // IDA CServerSelWin: v43[2]=0xC1, v43[3]=6, v44=0xF4,
                            // v45=3, v46=LOWORD(ServerCode).
                            BYTE pkt6[6] = { 0xC1, 0x06, 0xF4, 0x03,
                                             (BYTE)(serverCode & 0xff),
                                             (BYTE)(serverCode >> 8) };
                            gNetwork.SendToConnectServer(pkt6, 6);
                        } else {
                            // ── Flujo directo (sin ConnectServer) ──────────────
                            // No hay ConnectServer: conectar directo al GameServer
                            // de ServerConfig.h. La transición a state 1 (Connecting)
                            // arranca el progress bar + espera JoinServer.
                            extern void CreateSocket(const char *server, unsigned int port);
                            CreateSocket(szServerIpAddress,
                                         (unsigned int)g_ServerPort);
                        }
                        LoginSubState = 1;   // state = Connecting

                        // IDA 0x0051E7E0 L315-316 (CServerSelWin): these are
                        // GlobalText[470]/[471], not the standalone buffers
                        // DAT_07d4c3ec / GlobalText[471] that Ghidra split.
                        UIChatLogWindow_AddText(DAT_083a7c64, GlobalText[470], 1);
                        UIChatLogWindow_AddText((const char*)&DAT_083a7c68, GlobalText[471], 1);

                    } else if (ping_ok) {
                        // Already has cooldown — flash counters
                        if (ErrorMessage == MESSAGE_NONE) ErrorMessage = MESSAGE_LOGIN_SERVER_FULL;
                        else                    NextErrorMessage = MESSAGE_LOGIN_SERVER_FULL;
                    }
                }
            }

            local_834b++;
            iVar8b    += 0x14;
        } while (local_834b < server_count);
    }

done:
    // Decrement per-frame cooldown
    if (DAT_083a7c44 > 0)
        DAT_083a7c44--;
}
