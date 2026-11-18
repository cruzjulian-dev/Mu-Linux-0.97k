#include <cstdint>
// UI_StatsPanel.cpp
// RenderErrorMessage @ 0x0051af50 — UI_StatsPanel_Render
#include "stdafx.h"
#include "game/entity/EntityName.h"
#include "render/Gfx.h"
#include "platform/Text.h"
#include "ui/OptionsMenu.h"

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int RenderErrorMessage(void)

{
  // NOTE: quitado el `return 0` auto-skip de Ghidra. Sin él, el menú ESC
  // (state 0x6e) no se dibujaba. Los únicos loops con end-bound absoluto
  // (`while ((int)piVar7 < 0x83a432c)` en estados 0x8b/0x97) son de 2 iters
  // con guards internos — seguros aunque el pool no esté inicializado.
  float fVar1;
  LPCSTR pCVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  LPCSTR pCVar6;
  int *piVar7;
  longlong lVar8;
  undefined8 uVar9;
  undefined4 uVar10;
  char *ppCVar11;
  LPSIZE ptVar12;
  char cVar13;
  char *pcVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  tagSIZE *ptVar17;
  byte *pbVar18;
  tagSIZE local_e4;
  unsigned int local_dc;  // dual-use: integer, float bits, or pointer (Ghidra artifact)
  unsigned int local_d8;
  float local_d4;
  tagSIZE local_d0;
  char   local_c8 [100];  // text format buffer (was LPCSTR[25], corrected)
  CHAR local_64 [100];

  InputTextWidth = 0x100;
  gOptionsMenu.OnErrorMessageFrame();   // DESVIACION (DLL OptionsMenu, hook 0x0051AF65)
  GL_ResetState();
  Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
  gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_NORMAL));
  SetBackgroundTextColor = 0;
  m_dwTextColor = 0xffffffff;
  if (ErrorMessage != MESSAGE_NONE) {
    if (ErrorMessage == MESSAGE_LOG_OUT) {
      if (SceneFlag == 5) {
        pCVar6 = (LPCSTR)0x5;
      }
      else {
        pCVar6 = (LPCSTR)((SceneFlag == 4) + 3);
      }
      if (pCVar6 != (LPCSTR)0x0) {
        local_dc = 0x3c;
        local_d8 = (unsigned int)(uintptr_t)pCVar6;
        do {
          fVar1 = (float)(int)local_dc;
          local_e4.cx = (LONG)fVar1;
          GL_DrawTexture(0xf0,(float)gWindow.PanelOrigin(Ui::Menu),fVar1,120.0,22.0,0.0,0.0,0.83203125,1.0,'\x01','\x01');
          if ((((gWindow.PanelOrigin(Ui::Menu) - 1 < MouseX) && (MouseX < gWindow.PanelOrigin(Ui::Menu) + Ui::Menu.width)) && ((int)local_dc <= MouseY)
              ) && (MouseY < (int)local_dc + 0x16)) {
            Gfx::Color3f(0.8f, 0.6f, 0.4f);  // bits de IDA → float
            GL_SetBlendAdditive();
            GL_DrawTexture(0xf0,(float)gWindow.PanelOrigin(Ui::Menu),fVar1,120.0,22.0,0.0,0.0,0.83203125,1.0,'\x01','\x01');
            Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
            GL_ResetState();
          }
          local_dc = (int)local_dc + 0x1e;
          local_d8 = local_d8 + -1;
        } while (local_d8 != 0);
      }
      GL_SetBlendSrcOver('\x01');
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[381]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[381],iVar3,ptVar17);
      UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x41,
                   (LPCSTR)GlobalText[381],(LPSIZE)0x0,'\0',0);
      if (SceneFlag == 5) {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[382]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[382],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)GlobalText[382],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[383]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[383],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x7d,
                     (LPCSTR)GlobalText[383],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[385]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[385],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x9b,
                     (LPCSTR)GlobalText[385],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[384]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[384],iVar3,ptVar17);
        uVar10 = 0xb9;
      }
      else if (SceneFlag == 4) {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[382]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[382],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)GlobalText[382],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[385]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[385],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x7d,
                     (LPCSTR)GlobalText[385],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[384]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[384],iVar3,ptVar17);
        uVar10 = 0x9b;
      }
      else {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[385]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[385],iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)GlobalText[385],(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)GlobalText[384]);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[384],iVar3,ptVar17);
        uVar10 = 0x7d;
      }
      UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),uVar10,
                   (LPCSTR)GlobalText[384],(LPSIZE)0x0,'\0',0);
    }
    else if (ErrorMessage == MESSAGE_OPTIONS) {
      // DESVIACION (DLL OptionsMenu, hook 0x0051B3CA): menú de opciones expandido
      // en lugar de los 4 botones del 0.97k (volver, ataque automático, susurros, cerrar).
      gOptionsMenu.Render();
    }
    else if ((((ErrorMessage == MESSAGE_CUSTOM) || (ErrorMessage == MESSAGE_DIALOG)) || (ErrorMessage == MESSAGE_DIALOG_INTERFACE)) ||
            ((ErrorMessage == MESSAGE_DEVIL_SQUARE_RANK || (ErrorMessage == MESSAGE_BLOOD_CASTLE_RESULT)))) {
      local_d8 = 0x3c;
      GL_DrawTexture(0xfc,213.0,60.0,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01');
      local_dc = 0x41;
      iVar3 = 7;
      switch(ErrorMessage) {
      case MESSAGE_CUSTOM:
      case MESSAGE_DIALOG_INTERFACE:
      case MESSAGE_BLOOD_CASTLE_RESULT:
switchD_0051bea9_caseD_8b:
        iVar3 = 4;
        break;
      case MESSAGE_DIALOG:
        iVar3 = ((g_iNumAnswer + g_iNumLineMessageBoxCustom) * 0x12 + 0x31) / 0x28;
        if (iVar3 < 9) {
          if (iVar3 < 4) goto switchD_0051bea9_caseD_8b;
          if (iVar3 < 9) break;
        }
        iVar3 = 9;
      }
      if (0 < iVar3) {
        do {
          GL_DrawTexture(0xfb,213.0,(float)(int)local_dc,213.0,40.0,0.0,0.0,0.83203125,0.625,'\x01',
                       '\x01');
          local_dc = (int)local_dc + 0x28;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
      GL_DrawTexture(0xfc,213.0,(float)(int)local_dc,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01')
      ;
      // 2 iteraciones (rects de botón, stride 5 ints): IDA acota con
      // 0x83a432c - 0x83a4304 = 0x28 = 2 × 20 bytes.
      piVar7 = &DAT_083a4304;
      for (int btnIdx = 0; btnIdx < 2; ++btnIdx) {
        if (0 < piVar7[-3]) {
          // Ancho y alto salen del mismo descriptor de ints (CreateOkMessageBox
          // escribe `v1[3] = 70; v1[4] = 21;`): los dos se convierten con `(float)`,
          // no se reinterpretan los bits.
          local_d4 = (float)piVar7[1];
          float local_dc_f = (float)*piVar7;
          GL_DrawTexture(piVar7[-3] + 0xf0,(float)piVar7[-2] + 213.0f,
                       (float)piVar7[-1] + 60.0f,local_dc_f,local_d4,0.0,0.0,
                       local_dc_f * 0.0078125f,local_d4 * 0.03125f,'\x01','\x01');
        }
        piVar7 = piVar7 + 5;
      }
      if (ErrorMessage == MESSAGE_DIALOG_INTERFACE) {
        // El render consulta el clic; sólo la lógica del diálogo debe consumirlo.
        GL_SetBlendSrcOver('\x01');
        Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
        if (DAT_083a7c08 == '\0') {
          uVar16 = 0x3e4ccccd;
          uVar15 = 0x3e4ccccd;
          uVar10 = 0x3e4ccccd;
LAB_0051c08c:
          Gfx::Color3f(*(float*)&uVar10, *(float*)&uVar15, *(float*)&uVar16);
        }
        else if (((0xf8 < MouseX) && (0xc9 < MouseY)) &&
                ((MouseX < 0x109 && (MouseY < 0xdf)))) {
          if (MouseLButtonPush == '\0') {
            uVar16 = 0x3f333333;
            uVar15 = 0x3f333333;
            uVar10 = 0x3f333333;
            goto LAB_0051c08c;
          }
          Gfx::Color3f(0.5f, 0.5f, 0.5f);  // bits de IDA → float
        }
        GL_DrawRotatedRect(9,253.0,210.0,25.0,16.0,0x43870000);
        Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
        if (DAT_083a7c08 == DAT_083a7c09) {
          uVar16 = 0x3e4ccccd;
          uVar15 = 0x3e4ccccd;
          uVar10 = 0x3e4ccccd;
LAB_0051c13d:
          Gfx::Color3f(*(float*)&uVar10, *(float*)&uVar15, *(float*)&uVar16);
        }
        else if ((((0x178 < MouseX) && (0xc9 < MouseY)) && (MouseX < 0x189)) &&
                (MouseY < 0xdf)) {
          if (MouseLButtonPush == '\0') {
            uVar16 = 0x3f333333;
            uVar15 = 0x3f333333;
            uVar10 = 0x3f333333;
            goto LAB_0051c13d;
          }
          Gfx::Color3f(0.5f, 0.5f, 0.5f);  // bits de IDA → float
        }
        GL_DrawRotatedRect(9,383.0,210.0,25.0,16.0,0x42b40000);
        Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
      }
    }
    else if (ErrorMessage == MESSAGE_SELECT_CHAOS) {
      Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
      local_d8 = 0x78;
      GL_DrawTexture(0xfc,213.0,120.0,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01');
      local_dc = 0x7d;
      iVar3 = 5;
      do {
        GL_DrawTexture(0xfb,213.0,(float)(int)local_dc,213.0,40.0,0.0,0.0,0.83203125,0.625,'\x01',
                     '\x01');
        local_dc = (int)local_dc + 0x28;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      GL_DrawTexture(0xfc,213.0,(float)(int)local_dc,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01')
      ;
      local_dc = 0x43340000u;
      iVar3 = 0;
      do {
        if ((((float)MouseY < *(float*)&local_dc) ||
            (*(float*)&local_dc + 35.0f < (float)MouseY)) ||
           (((float)MouseX < 245.0f || (395.0f < (float)MouseX)))) {
          uVar16 = 0x3f333333;
          uVar15 = 0x3f333333;
          uVar10 = 0x3f333333;
        }
        else {
          uVar16 = 0x3f800000;
          uVar15 = 0x3f800000;
          uVar10 = 0x3f800000;
        }
			// IDA stores the hover tint as float bit patterns (0.7f/1.0f).
			// Preserve that representation when forwarding it to OpenGL.
			Gfx::Color3f(*(float*)&uVar10, *(float*)&uVar15, *(float*)&uVar16);
        GL_DrawTexture(0xf0,245.0,*(float*)&local_dc,150.0,35.0,0.0,0.0,0.83203125,1.0,'\x01','\x01');
        gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
        m_dwTextColor = 0xfffff0c8;
        ptVar17 = &local_e4;
        // IDA RenderErrorMessage (0x51AF50) indexes the text table directly:
        // button 0 = GlobalText[735], button 1 = GlobalText[736].  These were
        // mistakenly routed through unpopulated legacy buffers, so the selector
        // frame rendered but its labels were blank.
        pCVar6 = GlobalText[735 + iVar3];
        iVar4 = Platform::TextLength(pCVar6);
        gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
        uVar10 = 0;
        cVar13 = '\0';
        ptVar12 = (LPSIZE)0x0;
        lVar8 = (longlong)(*(float*)&local_dc + 12.0f);   // IDA RenderErrorMessage case 143: (std::int64_t)(Height + 12.0)
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(int)lVar8,pCVar6,
                     ptVar12,cVar13,uVar10);
        { float _fdc = *(float*)&local_dc + 85.0f; local_dc = *(unsigned int*)&_fdc; }
        gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_NORMAL));
        m_dwTextColor = 0xffffffff;
        if (iVar3 == 0) {
          // General: the two explanatory lines immediately above button 0.
          UI_RenderText(0xf8,0x96,GlobalText[872],(LPSIZE)0x0,'\0',0);
          pCVar6 = GlobalText[873];
          uVar10 = 0xa0;
        }
        else {
          // Chaos Weapon: the two explanatory lines immediately above button 1.
          UI_RenderText(0xf8,0xf0,GlobalText[870],(LPSIZE)0x0,'\0',0);
          pCVar6 = GlobalText[871];
          uVar10 = 0xfa;
        }
        UI_RenderText(0xf8,uVar10,pCVar6,(LPSIZE)0x0,'\0',0);
        iVar3 = iVar3 + 1;
      } while (iVar3 < 2);
      Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
    }
    else if ((ErrorMessage == MESSAGE_ITEM_CHECK) || (ErrorMessage == MESSAGE_USE_STATE)) {
      local_d8 = 0x64;
      GL_DrawTexture(0xfc,213.0,100.0,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01');
      local_dc = 0x69;
      iVar3 = 3;
      do {
        GL_DrawTexture(0xfb,213.0,(float)(int)local_dc,213.0,40.0,0.0,0.0,0.83203125,0.625,'\x01',
                     '\x01');
        local_dc = (int)local_dc + 0x28;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      GL_DrawTexture(0xfc,213.0,(float)(int)local_dc,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01')
      ;
      // Mismo bound que arriba: 2 rects de botón (stride 5 ints, 0x28 / 0x14 = 2).
      piVar7 = &DAT_083a4304;
      for (int btnIdx = 0; btnIdx < 2; ++btnIdx) {
        iVar3 = piVar7[-3];
        if ((0 < iVar3) && (iVar3 < 5)) {
          { float _fdc = (float)piVar7[1]; local_dc = *(unsigned int*)&_fdc; }
          local_d4 = (float)*piVar7;
          GL_DrawTexture(iVar3 + 0xf0,(float)piVar7[-2] + 213.0f,
                       (float)piVar7[-1] + 100.0f,local_d4,*(float*)&local_dc,0.0,0.0,
                       local_d4 * 0.0078125f,*(float*)&local_dc * 0.03125f,'\x01','\x01');
        }
        piVar7 = piVar7 + 5;
      }
    }
    else {
      local_d8 = 0x3c;
      GL_DrawTexture(0xf0,213.0,60.0,213.0,64.0,0.0,0.0,0.83203125,1.0,'\x01','\x01');
      if (ErrorMessage == MESSAGE_INPUT_GOLD) {
        InputTextWidth = 100;
        UI_RenderInputField(0x116,0x62,0);
        InputTextWidth = 0x100;
      }
      else if (((ErrorMessage == MESSAGE_DELETE_CHARACTER) || (ErrorMessage == MESSAGE_DELETE_GUILD)) || (ErrorMessage == MESSAGE_LOCK_STORAGE)) {
        InputTextWidth = 100;
        UI_RenderInputField(0xf3,0x62,0);
        InputTextWidth = 0x100;
        if (((MouseX < 0x143) || (0x16a < MouseX)) ||
           ((MouseY < 0x62 || (0x76 < MouseY)))) {
          iVar3 = 0xf1;
        }
        else {
          iVar3 = 0xf2;
        }
        GL_DrawTexture(iVar3,323.0,98.0,40.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        if (((MouseX < 0x175) || (0x19c < MouseX)) ||
           ((MouseY < 0x62 || (0x76 < MouseY)))) {
          GL_DrawTexture(0xf3,373.0,98.0,40.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        }
        else {
          GL_DrawTexture(0xf4,373.0,98.0,40.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        }
      }
      else if ((((ErrorMessage == MESSAGE_GUILD_WAR) || (ErrorMessage == MESSAGE_GUILD)) || (ErrorMessage == MESSAGE_PARTY)) ||
              (ErrorMessage == MESSAGE_TRADE)) {
        if (((MouseX < 0xea) || (0x12f < MouseX)) ||
           ((MouseY < 0x62 || (0x76 < MouseY)))) {
          iVar3 = 0xf1;
        }
        else {
          iVar3 = 0xf2;
        }
        GL_DrawTexture(iVar3,234.0,98.0,70.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        if (((MouseX < 0x14e) || (0x193 < MouseX)) ||
           ((MouseY < 0x62 || (0x76 < MouseY)))) {
          GL_DrawTexture(0xf3,334.0,98.0,70.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        }
        else {
          GL_DrawTexture(0xf4,334.0,98.0,70.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
        }
      }
      else if ((((MouseX < 0x11c) || (0x161 < MouseX)) || (MouseY < 0x62)) ||
              (0x76 < MouseY)) {
        GL_DrawTexture(0xf1,284.0,98.0,70.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
      }
      else {
        GL_DrawTexture(0xf2,284.0,98.0,70.0,21.0,0.0,0.0,0.546875,0.65625,'\x01','\x01');
      }
    }
  }
  GL_SetBlendSrcOver('\x01');
  pCVar2 = (LPCSTR)(uintptr_t)local_d8;
  pCVar6 = (LPCSTR)(uintptr_t)(local_d8 + 0xf);
  switch(ErrorMessage) {
  case MESSAGE_LOGIN_WRONG_PASSWORD:
  case MESSAGE_RECOVERY_WRONG_PASSWORD:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[407]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[407],iVar3,ptVar17);
    ppCVar11 = GlobalText[407];
    break;
  case MESSAGE_LOGIN_INVALID_ACCOUNT:
  case MESSAGE_RECOVERY_INVALID_ACCOUNT:
  case MESSAGE_RECOVERY_INVALID_ACCOUNT_2:
  case MESSAGE_RECOVERY_INVALID_ACCOUNT_3:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[414]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[414],iVar3,ptVar17);
    ppCVar11 = GlobalText[414];
    break;
  case MESSAGE_LOGIN_ALREADY_CONNECTED:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[415]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[415],iVar3,ptVar17);
    ppCVar11 = GlobalText[415];
    break;
  case MESSAGE_LOGIN_SERVER_FULL:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[416]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[416],iVar3,ptVar17);
    ppCVar11 = GlobalText[416];
    break;
  case MESSAGE_LOGIN_BLOCKED:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[417]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[417],iVar3,ptVar17);
    ppCVar11 = GlobalText[417];
    break;
  case MESSAGE_LOGIN_VERSION:
  case MESSAGE_VERSION:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[405]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[405],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[405],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[406]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[406],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[406];
    break;
  case MESSAGE_LOGIN_CONNECTION_ERROR:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[408]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[408],iVar3,ptVar17);
    ppCVar11 = GlobalText[408];
    break;
  case MESSAGE_LOGIN_TOO_MANY_ATTEMPTS:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[409]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[409],iVar3,ptVar17);
    ppCVar11 = GlobalText[409];
    break;
  case MESSAGE_LOGIN_SUBSCRIPTION_TERM:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[410]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[410],iVar3,ptVar17);
    ppCVar11 = GlobalText[410];
    break;
  case MESSAGE_LOGIN_SUBSCRIPTION_TIME:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[411]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[411],iVar3,ptVar17);
    ppCVar11 = GlobalText[411];
    break;
  case MESSAGE_LOGIN_IP_TERM:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[412]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[412],iVar3,ptVar17);
    ppCVar11 = GlobalText[412];
    break;
  case MESSAGE_LOGIN_IP_TIME:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[413]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[413],iVar3,ptVar17);
    ppCVar11 = GlobalText[413];
    break;
  default:
    return ErrorMessage + -0x15;
  case MESSAGE_LOGIN_POINT_NO_DATES:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[597]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[597],iVar3,ptVar17);
    ppCVar11 = GlobalText[597];
    break;
  case MESSAGE_LOGIN_POINT_NO_POINTS:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[598]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[598],iVar3,ptVar17);
    ppCVar11 = GlobalText[598];
    break;
  case MESSAGE_LOGIN_IP_NOT_ALLOWED:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[599]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[599],iVar3,ptVar17);
    ppCVar11 = GlobalText[599];
    break;
  case MESSAGE_LOGIN_NO_BILLING:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[433]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[433],iVar3,ptVar17);
    ppCVar11 = GlobalText[433];
    break;
  case MESSAGE_LOGIN_AGE_RESTRICTED:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[435]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[435],iVar3,ptVar17);
    ppCVar11 = GlobalText[435];
    break;
  case MESSAGE_CHARACTER_INVALID_NAME:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[394]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[394],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[394],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[395]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[395],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[395];
    goto LAB_0051ca70;
  case MESSAGE_CHARACTER_SLOTS_FULL:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[396]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[396],iVar3,ptVar17);
    ppCVar11 = GlobalText[396];
    goto LAB_0051ca70;
  case MESSAGE_CHARACTER_DELETE_FAILED:
    if (DAT_05826d20 == 0) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[438]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[438],iVar3,ptVar17);
      ppCVar11 = GlobalText[438];
    }
    else if (DAT_05826d20 == 3) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[439]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[439],iVar3,ptVar17);
      ppCVar11 = GlobalText[439];
    }
    else {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[401]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[401],iVar3,ptVar17);
      ppCVar11 = GlobalText[401];
    }
    goto LAB_0051ca70;
  case MESSAGE_INPUT_ID:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[403]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[403],iVar3,ptVar17);
    ppCVar11 = GlobalText[403];
    goto LAB_0051ca70;
  case MESSAGE_INPUT_PASSWORD:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[404]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[404],iVar3,ptVar17);
    ppCVar11 = GlobalText[404];
    goto LAB_0051ca70;
  case MESSAGE_SERVER_LOST:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[402]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[402],iVar3,ptVar17);
    ppCVar11 = GlobalText[402];
    goto LAB_0051ca70;
  case MESSAGE_DELETE_CHARACTER:
    m_dwTextColor = 0xff0000ff;
    // IDA 0x51C36C: sprintf(String, GlobalText[397], CharactersClient + 916*SelectedHero + 0x1C1).
    // GlobalText[397] es "Si deseas eliminar a %s," y el %s es el nombre del
    // personaje seleccionado.  Sin ese argumento printf tomaba un slot cualquiera
    // de la pila como char* -> AV de lectura dentro de ucrtbase.
    crt_sprintf(local_c8,&DAT_07d46e60,
                EntityName::Get(CharactersClient + CHARACTER_STRIDE * SelectedHero));
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[398]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[398],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[398];
    break;
  case MESSAGE_RESTRICTED_WORDS:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[392]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[392],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[392],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[393]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[393],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[393];
    break;
  case MESSAGE_INPUT_GOLD:
    if (StorageGoldFlag == 0) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[420]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[420],iVar3,ptVar17);
      ppCVar11 = GlobalText[420];
    }
    else if (StorageGoldFlag == 1) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[421]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[421],iVar3,ptVar17);
      ppCVar11 = GlobalText[421];
    }
    else {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)GlobalText[422]);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[422],iVar3,ptVar17);
      ppCVar11 = GlobalText[422];
    }
    goto LAB_0051ca70;
  case MESSAGE_NOT_ENOUGH_GOLD:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[423]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[423],iVar3,ptVar17);
    ppCVar11 = GlobalText[423];
    goto LAB_0051ca70;
  case MESSAGE_OVERFLOW_GOLD:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[424]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[424],iVar3,ptVar17);
    ppCVar11 = GlobalText[424];
    goto LAB_0051ca70;
  case MESSAGE_GUILD:
    // IDA 0x51CD64: el indice que devuelve FindCharacterIndex indexa
    // CharactersClient y el nombre (+0x1C1) es el %s del formato
    // (GlobalText[418] = "%s").  El port descartaba el indice y no pasaba el
    // argumento -> AV de lectura dentro de ucrtbase.
    {
    int nameIdx = FindCharacterIndex(GuildPlayerKey);
    crt_sprintf(local_c8,(const char *)GlobalText[418],
                EntityName::Get(CharactersClient + CHARACTER_STRIDE * nameIdx));
    }
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    crt_sprintf(local_c8,&DAT_07d493e0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x15;
    goto LAB_0051ca70;
  case MESSAGE_PARTY:
  {
    // 0051AF50 obtains the visible character slot from 0045AC80 before
    // expanding the localized invitation format.  The prior void stub lost
    // that return value, so the party prompt had no inviter name.
    const int characterIndex = FindCharacterIndex(PartyKey);
    const char* inviterName = (characterIndex < 400 && CharactersClient)
        ? EntityName::Get(CharactersClient + characterIndex * CHARACTER_STRIDE)
        : "";
    Platform::TextFormatTruncated(local_c8, sizeof(local_c8), Platform::TruncateText,
                (const char*)&GlobalText[418], inviterName);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    Platform::TextFormatTruncated(local_c8, sizeof(local_c8), Platform::TruncateText,
                (const char*)&GlobalText[425], inviterName);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x15;
    goto LAB_0051ca70;
  }
  case MESSAGE_TRADE:
    // IDA: RenderErrorMessage 0x51C940 — solicitud entrante de Trade.
    // ProtocolCore guarda el nombre exacto de diez bytes en DAT_07EA9834;
    // GlobalText[418] aporta el formato localizado y GlobalText[419] la línea
    // de confirmación. ErrorMessage 128 es guerra de guild, no Trade.
    crt_sprintf(local_c8, (const char*)GlobalText[418], DAT_07ea9834);
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BIG));
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 4),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_NORMAL));
    crt_sprintf(local_c8, GlobalText[419]);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x1a;
    goto LAB_0051ca70;
  case MESSAGE_MIN_LENGTH:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[390]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[390],iVar3,ptVar17);
    ppCVar11 = GlobalText[390];
    goto LAB_0051ca70;
  case MESSAGE_SPECIAL_NAME:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[391]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[391],iVar3,ptVar17);
    ppCVar11 = GlobalText[391];
    goto LAB_0051ca70;
  case MESSAGE_DELETE_GUILD:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[427]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[427],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[427],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[428]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[428],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[428];
    break;
  case MESSAGE_NO_GUILD_MARK:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[426]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[426],iVar3,ptVar17);
    ppCVar11 = GlobalText[426];
    goto LAB_0051ca70;
  case MESSAGE_GUILD_WAR:
    // IDA: RenderErrorMessage 0x51CE0D — invitación a guerra de guild.
    crt_sprintf(local_c8, GlobalText[430], GuildWarName);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    crt_sprintf(local_c8, GlobalText[EnableSoccer ? 432 : 431]);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x15;
    break;
  case MESSAGE_DELETE_CHARACTER_WARNING:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[399]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[399],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[399],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[400]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[400],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[400];
    goto LAB_0051ca70;
  case MESSAGE_BLOCKED_CHARACTER:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[434]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[434],iVar3,ptVar17);
    ppCVar11 = GlobalText[434];
    break;
  case MESSAGE_STORAGE_PASSWORD_WRONG:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[440]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[440],iVar3,ptVar17);
    ppCVar11 = GlobalText[440];
    break;
  case MESSAGE_STORAGE_ALREADY_LOCKED:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[441]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[441],iVar3,ptVar17);
    ppCVar11 = GlobalText[441];
    break;
  case MESSAGE_STORAGE_PASSWORD_TOO_SIMPLE:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[442]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[442],iVar3,ptVar17);
    ppCVar11 = GlobalText[442];
    break;
  case MESSAGE_STORAGE_PASSWORD_NOT_MATCH:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[445]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[445],iVar3,ptVar17);
    ppCVar11 = GlobalText[445];
    break;
  case MESSAGE_STORAGE_RESIDENT_WRONG:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[401]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[401],iVar3,ptVar17);
    ppCVar11 = GlobalText[401];
    break;
  case MESSAGE_CUSTOM:
  case MESSAGE_DIALOG_INTERFACE:
  case MESSAGE_DIALOG:
    iVar3 = (((7 - g_iNumAnswer) - g_iNumLineMessageBoxCustom) * 0x12) / 2 + 0x4c;
    if ((ErrorMessage == MESSAGE_DIALOG) && (iVar3 < 0x46)) {
      iVar3 = 0x46;
    }
    local_dc = 0;
    if (0 < g_iNumLineMessageBoxCustom) {
      pCVar6 = (LPCSTR)g_lpszMessageBoxCustom;
      do {
        ptVar17 = &local_e4;
        iVar4 = Platform::TextLength(pCVar6);
        gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),iVar3,pCVar6,
                     (LPSIZE)0x0,'\0',0);
        iVar3 = iVar3 + 0x12;
        local_dc = local_dc + 1;
        pCVar6 = pCVar6 + 0x26;
      } while ((int)local_dc < g_iNumLineMessageBoxCustom);
    }
    local_d4 = (float)((MouseY - iVar3) / 0x12);
    if (MouseY - iVar3 < 0) {
      local_d4 = -NAN;
    }
    if ((ErrorMessage == MESSAGE_DIALOG) && (local_dc = 0, 0 < g_iNumAnswer)) {
      const char* answer = (const char*)g_lpszDialogAnswer;
      do {
        if (((int)local_d4 == (int)local_dc) &&
           (uVar5 = (int)(0x13fU - MouseX) >> 0x1f,
           (int)((0x13fU - MouseX ^ uVar5) - uVar5) < 0x6b)) {
          uVar10 = 0;
        }
        else {
          uVar10 = 0x3f800000;
        }
        Gfx::Color3f(1.0f, *(float*)&uVar10, 0.0f);
        local_e4.cx = 0;
        pCVar6 = answer;
        do {
          if (*pCVar6 == '\0') break;
          ptVar17 = &local_d0;
          iVar4 = Platform::TextLength(pCVar6);
          gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
          UI_RenderText(0x140 - ((uint)(local_d0.cx * 0x280) / gWindow.GetWidth() >> 1),iVar3,pCVar6,
                       (LPSIZE)0x0,'\0',0);
          iVar3 = iVar3 + 0x12;
          local_e4.cx = local_e4.cx + 1;
          pCVar6 = pCVar6 + 0x26;
        } while (local_e4.cx < 1);
        local_dc = local_dc + 1;
        answer += 0x26;
      } while ((int)local_dc < g_iNumAnswer);
    }
    Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
    return 0;
  case MESSAGE_DEVIL_SQUARE_RANK:
    iVar3 = RenderMatchScore();
    return iVar3;
  case MESSAGE_SELECT_CHAOS:
    m_dwTextColor = 0xff0080ff;
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[734]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[734],iVar3,ptVar17);
    ppCVar11 = GlobalText[734];
    break;
  case MESSAGE_SERVER_IMMIGRATION_RESIDENT:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[401]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[401],iVar3,ptVar17);
    ppCVar11 = GlobalText[401];
    break;
  case MESSAGE_SERVER_IMMIGRATION_DB:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)&szServerMigrationError);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)&szServerMigrationError,iVar3,ptVar17);
    ppCVar11 = szServerMigrationError;
    break;
  case MESSAGE_ITEM_CHECK:
  case MESSAGE_USE_STATE:
    local_dc = local_d8 + 0x1e;
    local_d8 = 0x15e;
    if (ErrorMessage == MESSAGE_USE_STATE) goto LAB_0051d371;
    if (DAT_07eaa13c != 2) {
      if (DAT_07eaa13c != 3) {
        if (DAT_07eaa13c != 4) goto LAB_0051d371;
        pCVar6 = pCVar2 + 0x2d;
      }
      local_d8 = 0x140;
      local_dc = (unsigned int)(uintptr_t)pCVar6;
      goto LAB_0051d371;
    }
    local_d8 = 0x140;
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
    Gfx::Color3f(1.0f, 1.0f, 0.0f);
    uVar5 = ((int)MixType < 0) - 1 & MixType;
    if (MixType == 0xb) {
      uVar5 = 7;
LAB_0051d2dd:
      iVar3 = uVar5 + 0x25f;
    }
    else {
      iVar3 = uVar5 + 0x259;
      if (4 < (int)uVar5) goto LAB_0051d2dd;
    }
    ptVar17 = &local_d0;
    pCVar6 = GlobalText[iVar3];
    iVar3 = Platform::TextLength(pCVar6);
    gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_d0.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 0x19),pCVar6,
                 (LPSIZE)0x0,'\0',0);
    local_dc = (unsigned int)(uintptr_t)(pCVar2 + 0x3b);
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_NORMAL));
LAB_0051d371:
    iVar3 = 0;
    if (g_iNumLineMessageBoxCustom < 1) {
      return g_iNumLineMessageBoxCustom;
    }
    do {
      Gfx::Color3f(1.0f, 0.7f, 0.0f);
      gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
      if (iVar3 == 0) {
        if (DAT_07eaa13c == 1) {
          uVar10 = 0;
LAB_0051d3cf:
          Gfx::Color3f(1.0f, *(float*)&uVar10, *(float*)&uVar10);
        }
      }
      else if ((iVar3 == 3) && (DAT_07eaa13c == 3)) {
        uVar10 = 0x3dcccccd;
        goto LAB_0051d3cf;
      }
      if (DAT_07eaa13c == 2) {
        Gfx::Color3f(0.9f, 0.9f, 1.0f);
      }
      ptVar17 = &local_d0;
      pCVar6 = g_lpszMessageBoxCustom[iVar3];
      iVar4 = Platform::TextLength(pCVar6);
      gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
      pCVar2 = (LPCSTR)(uintptr_t)local_dc;
      UI_RenderText((int)local_d8 - ((uint)(local_d0.cx * 0x280) / gWindow.GetWidth() >> 1),local_dc,pCVar6
                   ,(LPSIZE)0x0,'\0',0);
      local_dc = (unsigned int)(uintptr_t)(pCVar2 + 0x12);
      iVar3 = iVar3 + 1;
      if (g_iNumLineMessageBoxCustom <= iVar3) {
        return g_iNumLineMessageBoxCustom;
      }
    } while( true );
  case MESSAGE_LOCK_STORAGE:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[443]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[443],iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)GlobalText[443],(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)GlobalText[444]);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)GlobalText[444],iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = GlobalText[444];
LAB_0051ca70:
    uVar9 = UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)pCVar6,
                         (LPCSTR)ppCVar11,(LPSIZE)0x0,'\0',0);
    return (int)uVar9;
  case MESSAGE_BLOOD_CASTLE_RESULT:
    uVar9 = GuildOverview_Render();
    return (int)uVar9;
  }
  uVar9 = UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)pCVar6,
                       (LPCSTR)ppCVar11,(LPSIZE)0x0,'\0',0);
  return (int)uVar9;
}
