// Scene_Login_ServerSelect.cpp
// Scene_Login_ServerSelect @ 0x0051F020   (375 lines decompiled)
//
// Renders the server-selection panel shown in Login sub-state 0.
// Called directly from Scene_Login when DAT_083a7c14 == 0.
//
// ─── SERVER LIST DATA ──────────────────────────────────────────────────────────
//
//  Base array : ServerList (SERVER_LIST_t[25], structs.h)
//  Stride     : 0x21e bytes per server-group entry
//  Count      : 24 entries  (0x83a7ac6 - 0x83a45d8) / 0x21e
//
//  Server group entry layout (offsets from entry base):
//    +0x00  char[20]  name          — display name of the server group
//    +0x14  byte      num_channels  — how many sub-servers/channels
//    +0x15  byte      pvp_flag      — 0 = Non-PVP column, 1 = PVP column
//                                    (derived in 2nd pass: name contains '2' → PVP)
//    +0x2c  ushort    channel_id    — first channel's ID (mod 20 + 1 = display index)
//    +0x2e  byte      load          — server load byte:
//                                      0x80+ = FULL  (red tint)
//                                      0..99 = LOW   (green tint)
//                                      100+  = NORMAL (white tint)
//
//  Sub-server (channel) stride within entry: 0x1a bytes
//  So channel[i] load is at: entry+0x2e + i*0x1a
//      channel[i] id  is at: entry+0x2c + i*0x1a  (ushort)
//
//
// ─── SELECTION STATE ───────────────────────────────────────────────────────────
//
//  ServerSelectHi  g_SelectedServer     — index of highlighted server group (-1 = none)
//  ServerSelectLo  g_SelectedChannel    — index of highlighted channel within group
//  InputTextWidth                       — set to 0x100 on entry (scissor/clip enable?)
//  DAT_00559c78                       — set to 0xffffffff (clip mask full)
//  SetBackgroundTextColor                       — set to 0 then 0x80000000
//
// ─── RENDERING LAYOUT (640×480 reference coords) ───────────────────────────────
//
//  PASS 1 — format server name strings into stack buffer (100 bytes each):
//    Uses crt_sprintf(buf, fmt) to build display strings like:
//      "%s %s"          → single channel entry (offline/maintenance)
//      "%s %d Non-PVP %s" → non-PVP channel with number and load status
//      "%s %d PVP %s"    → PVP channel with number and load status
//    Format string addresses encode load status:
//      s__s__s_00561914   = full  (server full)
//      s__s__s_0056191c   = normal
//      s__s__s_00561924   = low load
//      s__s__d__s_0056192c = PVP + channel_num + full
//      s__s__d__s_00561940 = PVP + channel_num + normal
//      s__s__d__s_00561954 = PVP + channel_num + low
//      s__s__d__s_00561968 = Non-PVP + channel_num + full
//      s__s__d__s_00561974 = Non-PVP + channel_num + normal
//      s__s__d__s_00561980 = Non-PVP + channel_num + low
//
//  PASS 2 — tag PVP servers:
//    For each entry: if server_name[X] == '2' → pvp_flag=1, else pvp_flag=0
//    Counts: iVar8 = num PVP servers, iVar10 = num Non-PVP servers
//
//  PASS 3 — draw server group list (two columns):
//    Non-PVP column  x=0x96 (150px),  row y advances +16 per entry
//    PVP    column   x=0x177 (375px), row y advances +16 per entry
//
//    Per entry:
//      If selected (iVar8 == ServerSelectHi):
//        Tex 0x12 quad at (0xa0, y, 160, 16)   Non-PVP selected
//        Tex 0x12 quad at (0x16d, y, 160, 16)  PVP selected
//        glColor3f(1.0, 1.0, 1.0)              — full brightness
//      Else:
//        Tex 0x12 quad at (0x96, y, 150, 16)   Non-PVP normal
//        Tex 0x12 quad at (0x177, y, 150, 16)  PVP normal
//        glColor3f(0.8, 0.8, 0.8)              — dimmed
//      Text via UI_RenderText: centered at column_x, y+2
//        Width from GetTextExtentPointA → used to center string
//
//  PASS 4 — draw channel sub-list (right panel, only if g_SelectedServer != -1):
//    Panel x=230, w=134, h=20 per channel row
//    Channel count = server[g_SelectedServer].num_channels
//    Max visible channels = (0x18 - num_channels) * 0x14  (scroll clamped)
//
//    Per channel row:
//      Color (R,G,B):
//        load & 0x80 → (1.0 * _DAT_00552928, 1.0 * _DAT_00552504)  = RED   (full)
//        load < 100  → (1.0, 1.0, var)                              = GREEN (low)
//          special: if channel_id%20==0 or ==0xe: B = _DAT_00552928 (teal)
//        else        → (_DAT_005528b4, _DAT_005528b4, _DAT_005528b4)= WHITE (normal)
//      Selected channel: same colors but at full brightness (fVar3=1.0)
//      Non-selected: fVar3=0.8 (dimmed)
//      Tex 0x12 quad at (230, y, 134, 20)
//      Text: UI_RenderText(0x129 - textWidth/2, y+1, channelNameBuf)
//
//    Load bar (for non-full servers, below channel text):
//      Position: x = _DAT_00552d48, y = channel_y + _DAT_00552d4c
//      Filled dots  (0..load/5): Tex 0xf6 at (x+i*5, y, 3, 3)
//        Color fades: brighter at right end (iStack00000020 counts 20→0 * fVar3 * _DAT_005524f4)
//      Empty dots (load/5..20): Tex 0xf6, gray (fVar3 * _DAT_005524f4)
//
//  PASS 5 — selected server IP display:
//    Compares selected server name vs GlobalText[559]
//    If match:
//      glColor3f(1.0, 0.3, 0.1)  — orange-red
//      UI_RenderText(0x96, y-0x2e, DAT_07d530e8)   — server IP line 1
//      UI_RenderText(0x96, y-0x1f, DAT_07d53214)   — server IP line 2
//
// ─── HELPER FUNCTIONS ──────────────────────────────────────────────────────────
//  __chkstk_probe (0x00541c10)
//                — MSVC CRT stack-probe stub, NOT game logic.
//                  Called automatically by the compiler because this function
//                  allocates ~48 KB of locals (24 groups × 20 channels × 100 chars).
//                  Loop: while(EAX > 0x1000) { probe page; EAX -= 0x1000; }
//                  Renamed __chkstk_probe in Ghidra.
//  crt_sprintf(buf,fmt) — sprintf-like into stack buffer
//  GL_DrawTexture(tex,x,y,w,h,u0,v0,u1,v1,f1,f2) — draw textured quad
//  UI_RenderText(x,y,str,sizeOut,center,shadow)   — draw text string
//  Packet_IsValidSockType(a,b)     — lookup/flag check (returns 0 or non-zero → PVP type)

#include "stdafx.h"
#include "render/Gfx.h"
#include "platform/Text.h"
#include "Scene.h"


// Scene_Login_ServerSelect @ 0x0051F020
// Renders the server-group + channel selection panel inside the login screen.
// Server list: ServerList, stride 0x21e, 25 entries.
// Stack allocates ~48 KB of display strings (triggers __chkstk_probe).
int Scene_Login_ServerSelect(void)
{

    byte        bVar2;
    float       fVar3, fVar18, fVar20;
    // fVar4: "current Y of last drawn row" en Pass 3 (float-but-really-int).
    // Pass 4 la lee para posicionar la columna de canales. Se inicializa a 0xdd
    // (221, el tope que asigna iStartY cuando hay poca lista) para que, si
    // ServerSelectHi != -1, Pass 4 use un Y razonable. Pass 3 usa iYNonPvp/iYPvp
    // locales y sincroniza fVar4 al final.
    float       fVar4 = 0.0f;
    int         iVar5, iVar8, iVar10;
    uint        uVar6;
    undefined1 *puVar9;
    byte       *pbVar11, *pbVar14;
    LPCSTR      lpString;
    bool        bVar15;
    undefined8  uVar16;
    // float (en el decomp era undefined4): glColor3f recibe el valor, no el bit-pattern.
    float       uVar17;
    int         iStack00000004;
    float       fStack00000008, fStack0000000c, fStack00000014;
    int         iStack0000001c, iStack00000020;
    LPSIZE      ptVar19;
    undefined1 *puStack00000010;
    // Grupos de servidor x canales x 100 bytes por nombre.
    //
    // DESBORDE (2026-10-02): estaba en 24 grupos, pero el loop de Pass 1 recorre
    // 25 -- su cota es `ServerList + 0x34ee` y 0x34ee/0x21e = 25 (MAX_SERVER_HI).  El grupo 24 escribia en
    // chan_buf + 24*20*100 = 48000, o sea el primer byte DESPUES del buffer.
    // En Debug el padding del frame se lo comia; en Release reventaba la cookie
    // de /GS -- y como esta funcion se inlinea dentro de Scene_Login, la cookie
    // que saltaba era la de Scene_Login (de ahi el stack confuso).
    // El slot 24 lo usa el protocolo: Recv_ServerList manda ahi al grupo 12.
    enum { SRV_GROUPS = 25, SRV_CHANS = 20, SRV_NAMELEN = 100 };
    char        chan_buf[SRV_GROUPS * SRV_CHANS * SRV_NAMELEN];
    SIZE        text_size;

    /* __chkstk_probe(); */  // MSVC stack-frame allocator stub, not game logic

    iVar8  = 0;
    puVar9 = (undefined1*)chan_buf;
    InputTextWidth = 0x100;
    iStack00000004 = 0;

    // ── PASS 1: format channel name strings ──────────────────────────────────
    // IDA reference (0x0051F020):
    //   Single-ch : sprintf(buf, "%s %s", name, status)
    //   Multi NON-PVP channel: sprintf(buf, "%s-%d(Non-PVP) %s", name, chNum, status)
    //   Multi PVP channel:     sprintf(buf, "%s-%d %s", name, chNum, status)
    //   status = GlobalText[560/561/562]   (FULL/NORMAL/LOW)
    //
    // Desviación: status con strings fijos (acá no hay GlobalText[] cargado).
    {
        const char* STATUS_FULL   = "FULL";
        const char* STATUS_NORMAL = "NORMAL";
        const char* STATUS_LOW    = "LOW";

        auto statusOf = [&](BYTE load) {
            if ((load & 0x80) == 0x80) return STATUS_FULL;
            if ((load & 0x7f) < 100)   return STATUS_LOW;
            return STATUS_NORMAL;
        };
        for (int srvIdx = 0; srvIdx < MAX_SERVER_HI; ++srvIdx) {
            const SERVER_LIST_t& srv = ServerList[srvIdx];
            char* groupBuf = chan_buf + srvIdx * SRV_CHANS * SRV_NAMELEN;
            unsigned char numCh = srv.Number;
            // numCh sale crudo del paquete F4/02 (un byte, hasta 255) mientras
            // que chan_buf reserva SRV_CHANS por grupo: sin este clamp un server
            // que reporte mas canales escribe sobre el grupo siguiente y, en el
            // ultimo, fuera del buffer.
            if (numCh > SRV_CHANS) numCh = SRV_CHANS;

            if (numCh == 1) {
                crt_sprintf(groupBuf, "%s %s", srv.Name, statusOf(srv.Server[0].Percent));
            } else {
                for (int ch = 0; ch < numCh; ++ch) {
                    int chNum = (int)((unsigned)srv.Server[ch].Index % 0x14 + 1);
                    const char* status = statusOf(srv.Server[ch].Percent);
                    char* outBuf = groupBuf + ch * SRV_NAMELEN;
                    if (Packet_IsValidSockType(srvIdx, chNum) == 0)
                        crt_sprintf(outBuf, "%s-%d(Non-PVP) %s", srv.Name, chNum, status);  // Non-PVP
                    else
                        crt_sprintf(outBuf, "%s-%d %s", srv.Name, chNum, status);           // Open-PVP
                }
            }
        }
        // Pass 3 toma el cursor del buffer donde lo deja este recorrido.
        puVar9 = (undefined1*)(chan_buf + MAX_SERVER_HI * SRV_CHANS * SRV_NAMELEN);
    }

    // ── PASS 2: tag PVP/non-PVP groups, count each ───────────────────────────
    iVar8  = 0;   // PVP count
    iVar10 = 0;   // non-PVP count
    for (int i = 0; i < MAX_SERVER_HI; ++i) {
        SERVER_LIST_t& srv = ServerList[i];
        if (srv.Number == 0) continue;
        // IDA: el último carácter del nombre decide; '2' = servidor PVP.
        const char* name = srv.Name;
        if (name[strlen(name) - 1] == '2') {
            srv.extServer = true;
            iVar8++;
        } else {
            srv.extServer = false;
            iVar10++;
        }
    }

    // ── PASS 3: draw two-column server group list ─────────────────────────────
    puStack00000010 = puVar9;
    Gfx::Color3f(1.0f, 1.0f, 1.0f);

    // Compute base Y for non-PVP column
    // Los slots de Y son int32 (Ghidra los tipó float, pero el asm los usa como
    // int, bit-pattern 0xdd=221): Y_top = min(0x1bc - 16*N, 0xdd).
    int iYNonPvp;   // current non-PVP row Y (bumped +16 per entry)
    int iYPvp;      // current PVP row Y
    {
        int iStartY = iVar10 * -0x10 + 0x1bc;   // 444 - 16*non_pvp_count
        // IDA: v10 = 220 default; if (444-16*v8 <= 220) v10 = 444-16*v8.
        // Equivale a min(220, 444-16*v8). Cap a 0xdc=220, NO 0xdd=221.
        if (iStartY > 0xdc)                      // cap top at Y=220
            iStartY = 0xdc;
        iYNonPvp = iStartY;
        iYPvp    = ((iVar10 - iVar8) + -1) * 0x10 + iStartY;
        // Bridge: Pass 4/5 leen fStack00000008 como "PVP base Y" y fVar4 como
        // "non-PVP base Y" para posicionar la columna de canales. IDA usa v10
        // (non-PVP base Y) en `v21 = 16 * v23 - 10 * v20 + v10 + 8`.
        fStack00000008 = (float)iYPvp;
        fVar4          = (float)iStartY;   // non-PVP base Y, leído por Pass 4
    }

    m_dwTextColor = 0xffffffff;
    iVar8 = 0;
    SetBackgroundTextColor = 0;
    for (iVar8 = 0; iVar8 < MAX_SERVER_HI; ++iVar8) {
        lpString = ServerList[iVar8].Name;
        if (ServerList[iVar8].Number != 0) {
            int iDrawY;   // Y where this row's quad + text draw
            if (!ServerList[iVar8].extServer) {
                // Non-PVP entry
                iVar10 = 0x96;
                iDrawY = iYNonPvp;
                iYNonPvp += 0x10;
                iStack00000004 = 0x96;
                if (iVar8 != ServerSelectHi) goto LAB_0051f314;
                iVar10 = 0xa0;
                uVar17 = 1.0f;         // selected: full brightness
                iStack00000004 = 0xa0;
            } else {
                // PVP entry
                iVar10 = 0x177;
                iDrawY = iYPvp;
                iYPvp += 0x10;
                iStack00000004 = 0x177;
                if (iVar8 == ServerSelectHi) {
                    iVar10 = 0x16d;
                    uVar17 = 1.0f;         // selected
                    iStack00000004 = 0x16d;
                } else {
LAB_0051f314:
                    uVar17 = 0.8f;         // not selected: dimmed
                }
            }
            Gfx::Color3f(uVar17, uVar17, uVar17);
            GL_DrawTexture(0x12, (float)iStack00000004, (float)iDrawY,
                         70.0f, 16.0f, 0.0f, 0.0f, 0.5234375f, 0.9375f, '\x01', '\x01');
            ptVar19 = &text_size;
            iVar5 = Platform::TextLength(lpString);
            gFont.MeasureText(gFont.GetTextDC(), lpString, iVar5, ptVar19);
            UI_RenderText((iVar10 - ((uint)(text_size.cx * 0x280) / gWindow.GetWidth() >> 1)) + 0x23,
                         iDrawY + 2, lpString, (LPSIZE)0x0, '\0', 0);
        }
    }

    // ── PASS 4: draw channel sub-list for selected server ─────────────────────
    iVar8 = ServerSelectHi;
    if (ServerSelectHi != -1) {
        const SERVER_LIST_t& sel = ServerList[ServerSelectHi];
        // Fila del server elegido dentro de su columna: cuántos servers activos de
        // la misma columna hay antes del primero con el mismo nombre.
        iVar5 = 0;
        iStack00000004 = 0;
        for (int k = 0; k < MAX_SERVER_HI; ++k) {
            const SERVER_LIST_t& srv = ServerList[k];
            if (strcmp(srv.Name, sel.Name) == 0) { iVar5 = iStack00000004; break; }
            if (srv.Number != 0 && srv.extServer == sel.extServer)
                iStack00000004++;
        }
        uVar6 = sel.Number;
        iVar10 = iVar5 * 0x10 + uVar6 * -10 + 8
               + (sel.extServer ? (int)fStack00000008 : (int)fVar4);

        // Clamp scroll
        iVar5 = (0x18 - uVar6) * 0x14;
        if (iVar5 <= iVar10) iVar10 = iVar5;

        // Render each channel row
        iVar5 = 0;
        if (uVar6 != 0) {
            iStack00000004 = iVar10;
            do {
                fVar3 = 1.0f;
                if (iVar5 != ServerSelectLo)
                    fVar3 = 0.8f;   // not selected → dim

                // Color by load
                const SERVER_t& chan = sel.Server[iVar5];
                if ((chan.Percent & 0x80) == 0x80) {
                    // FULL: red-ish
                    fVar18 = fVar3 * 0.7f;
                    fVar20 = fVar3 * 0.5f;
                } else if ((chan.Percent & 0x7f) < 100) {
                    // LOW: greenish; special teal for channel_id % 20 == 0 or 14
                    fVar18 = fVar3;
                    if (((uint)chan.Index % 0x14 == 0) ||
                        (fVar20 = fVar3, (uint)chan.Index % 0x14 == 0xe))
                    {
                        fVar20 = fVar3 * 0.7f;
                    }
                } else {
                    // NORMAL: white
                    fStack00000008 = fVar3 * 0.4f;
                    fVar18 = fStack00000008; fVar20 = fStack00000008;
                }
                Gfx::Color3f(fVar3, fVar18, fVar20);

                // Channel background quad
                fVar18 = (float)iStack00000004;
                GL_DrawTexture(0x12, 230.0f, fVar18, 134.0f, 20.0f,
                             0.0f, 0.0f, 0.5234375f, 0.9375f, '\x01', '\x01');

                // Channel name text
                // El indice sale de ServerSelectHi (grupo elegido) y iVar5
                // (canal).  Con el flujo ConnectServer el server cae en el slot
                // 23, o sea pegado al borde del buffer, asi que se acota antes
                // de leer en vez de confiar en los dos valores.
                {
                    int chanIdx = iVar5 + ServerSelectHi * SRV_CHANS;
                    if (chanIdx < 0) chanIdx = 0;
                    if (chanIdx >= SRV_GROUPS * SRV_CHANS) chanIdx = SRV_GROUPS * SRV_CHANS - 1;
                    char* chanName = chan_buf + chanIdx * SRV_NAMELEN;

                    ptVar19 = &text_size;
                    iVar8 = Platform::TextLength(chanName);
                    gFont.MeasureText(gFont.GetTextDC(), chanName, iVar8, ptVar19);
                    UI_RenderText(0x129 - ((uint)(text_size.cx * 0x280) / gWindow.GetWidth() >> 1),
                                 iStack00000004 + 1, chanName, (LPSIZE)0x0, '\0', 0);
                }

                // Load bar (only for non-full servers)
                if ((chan.Percent & 0x80) != 0x80) {
                    uVar6 = chan.Percent & 0x7f;
                    if (100 < uVar6) uVar6 = 100;
                    uVar6 = uVar6 / 5;   // 0..20 dots
                    fStack00000008 = 0.0f;
                    if (uVar6 != 0) {
                        // Filled dots (gradient: darker on left, brighter on right)
                        fVar20 = fVar18 + 14.0f;
                        iStack0000001c = 0;
                        iStack00000020 = 0x14;
                        do {
                            iVar8 = (int)fStack00000008;
                            fStack0000000c = (float)iStack00000020 * fVar3 * 0.1f;
                            Gfx::Color3f((float)(int)fStack00000008 * fVar3 * 0.1f, fStack0000000c, fStack0000000c);
                            GL_DrawTexture(0xf6, (float)iStack0000001c + 248.0f,
                                         fVar20, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, 1.0f, '\x01', '\x01');
                            fStack00000008 = (float)(iVar8 + 1);
                            iStack00000020--;
                            iStack0000001c += 5;
                        } while ((int)fStack00000008 < (int)uVar6);
                    }
                    // Empty dots (gray)
                    fStack00000008 = fVar3 * 0.1f;
                    Gfx::Color3f(fStack00000008, fStack00000008, fStack00000008);
                    if (uVar6 < 0x14) {
                        fVar18 = fVar18 + 14.0f;
                        fVar3  = (float)(uVar6 * 5);
                        fStack0000000c = fVar18;
                        do {
                            GL_DrawTexture(0xf6, (float)(int)fVar3 + 248.0f,
                                         fVar18, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, 1.0f, '\x01', '\x01');
                            fVar3 = (float)((int)fVar3 + 5);
                        } while ((int)fVar3 < 100);
                    }
                }

                iVar5++;
                iStack00000004 += 0x14;
            } while (iVar5 < (int)sel.Number);
        }

        // ── PASS 5: selected server IP display ───────────────────────────────
        m_dwTextColor = 0xffffffff;
        SetBackgroundTextColor = 0x80000000;
        // Literales float (no bit-patterns 0x3fxxxxxx).
        Gfx::Color3f(1.0f, 0.2f, 0.1f);   // orange-red (IP text)

        fVar4   = (float)(int)(intptr_t)puStack00000010;
        pbVar14 = (byte*)GlobalText[559];
        pbVar11 = (byte*)sel.Name;
        do {
            bVar2 = *pbVar11; bVar15 = bVar2 < *pbVar14;
            if (bVar2 != *pbVar14) { iVar8 = (1 - (uint)bVar15) - (uint)(bVar15 != 0); goto LAB_0051f8b4; }
            if (bVar2 == 0) break;
            bVar2 = pbVar11[1]; bVar15 = bVar2 < pbVar14[1];
            if (bVar2 != pbVar14[1]) { iVar8 = (1 - (uint)bVar15) - (uint)(bVar15 != 0); goto LAB_0051f8b4; }
            pbVar11 += 2; pbVar14 += 2;
        } while (bVar2 != 0);
        iVar8 = 0;
LAB_0051f8b4:
        if (iVar8 == 0) {
            // Currently connected to this server → show IP
            UI_RenderText(0x96, (intptr_t)puStack00000010 - 0x2e, (char*)&DAT_07d530e8, (LPSIZE)0x0, '\0', 0);
            UI_RenderText(0x96, (int)fVar4 - 0x1f, (char*)&DAT_07d53214, (LPSIZE)0x0, '\0', 0);
            iVar8  = 0;
        }
    }
    return iVar8;
}
