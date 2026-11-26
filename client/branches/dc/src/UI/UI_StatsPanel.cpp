#include <cstdint>
// UI_StatsPanel.cpp
// RenderErrorMessage @ 0x0051af50 — UI_StatsPanel_Render
#include "stdafx.h"
#include "Entity/EntityName.h"
#include "Render/Gfx.h"
#include "Platform/Text.h"
#include "UI/OptionsMenu.h"

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

  LegacyInputTextWidth = 0x100;
  gOptionsMenu.OnErrorMessageFrame();   // DESVIACION (DLL OptionsMenu, hook 0x0051AF65)
  GL_ResetState();
  Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
  gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_NORMAL));
  SetBackgroundTextColor = 0;
  m_dwTextColor = 0xffffffff;
  if (InGameUiState != 0) {
    if (InGameUiState == 0x6e) {
      int menuItemCount;
      if (SceneFlag == 5) {
        menuItemCount = 5;
      }
      else {
        menuItemCount = (SceneFlag == 4) + 3;
      }
      if (menuItemCount != 0) {
        local_dc = 0x3c;
        local_d8 = (unsigned int)menuItemCount;
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
      iVar3 = Platform::TextLength((LPCSTR)DlgText6E_0);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_0,iVar3,ptVar17);
      UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x41,
                   (LPCSTR)DlgText6E_0,(LPSIZE)0x0,'\0',0);
      if (SceneFlag == 5) {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_1);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_1,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)DlgText6E_1,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_2);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_2,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x7d,
                     (LPCSTR)DlgText6E_2,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_4);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_4,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x9b,
                     (LPCSTR)DlgText6E_4,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_3);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_3,iVar3,ptVar17);
        uVar10 = 0xb9;
      }
      else if (SceneFlag == 4) {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_1);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_1,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)DlgText6E_1,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_4);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_4,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x7d,
                     (LPCSTR)DlgText6E_4,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_3);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_3,iVar3,ptVar17);
        uVar10 = 0x9b;
      }
      else {
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_4);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_4,iVar3,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),0x5f,
                     (LPCSTR)DlgText6E_4,(LPSIZE)0x0,'\0',0);
        ptVar17 = &local_e4;
        iVar3 = Platform::TextLength((LPCSTR)DlgText6E_3);
        gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText6E_3,iVar3,ptVar17);
        uVar10 = 0x7d;
      }
      UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),uVar10,
                   (LPCSTR)DlgText6E_3,(LPSIZE)0x0,'\0',0);
    }
    else if (InGameUiState == 0x96) {
      // DESVIACION (DLL OptionsMenu, hook 0x0051B3CA): menú de opciones expandido
      // en lugar de los 4 botones del 0.97k (volver, ataque automático, susurros, cerrar).
      gOptionsMenu.Render();
    }
    else if ((((InGameUiState == 0x8b) || (InGameUiState == 0x8e)) || (InGameUiState == 0x8d)) ||
            ((InGameUiState == 0x8c || (InGameUiState == 0x9a)))) {
      local_d8 = 0x3c;
      GL_DrawTexture(0xfc,213.0,60.0,213.0,5.0,0.0,0.0,0.83203125,0.625,'\x01','\x01');
      local_dc = 0x41;
      iVar3 = 7;
      switch(InGameUiState) {
      case 0x8b:
      case 0x8d:
      case 0x9a:
switchD_0051bea9_caseD_8b:
        iVar3 = 4;
        break;
      case 0x8e:
        iVar3 = ((DialogAnswerCountA + MessageBoxLineCount) * 0x12 + 0x31) / 0x28;
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
      piVar7 = &DialogButton0Width;
      for (int btnIdx = 0; btnIdx < 2; ++btnIdx) {
        if (0 < piVar7[-3]) {
          // Ancho y alto salen del mismo descriptor de ints (CreateOkMessageBox
          // escribe `v1[3] = 70; v1[4] = 21;`): los dos se convierten con `(float)`,
          // no se reinterpretan los bits.
          local_d4 = (float)piVar7[1];
          float local_dc_f = (float)*piVar7;
          GL_DrawTexture(piVar7[-3] + 0xf0,(float)piVar7[-2] + StatsPanelXOffset,
                       (float)piVar7[-1] + 60.0f,local_dc_f,local_d4,0.0,0.0,
                       local_dc_f * 0.0078125f,local_d4 * 0.03125f,'\x01','\x01');
        }
        piVar7 = piVar7 + 5;
      }
      if (InGameUiState == 0x8d) {
        // El render consulta el clic; sólo la lógica del diálogo debe consumirlo.
        GL_SetBlendSrcOver('\x01');
        Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
        if (DialogPageNum == '\0') {
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
        if (DialogPageNum == DialogPageMax) {
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
    else if (InGameUiState == 0x8f) {
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
            (*(float*)&local_dc + EntityPanelBtnWidth < (float)MouseY)) ||
           (((float)MouseX < InvCursorBoundXMin || (395.0f < (float)MouseX)))) {
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
    else if ((InGameUiState == 0x97) || (InGameUiState == 0x99)) {
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
      piVar7 = &DialogButton0Width;
      for (int btnIdx = 0; btnIdx < 2; ++btnIdx) {
        iVar3 = piVar7[-3];
        if ((0 < iVar3) && (iVar3 < 5)) {
          { float _fdc = (float)piVar7[1]; local_dc = *(unsigned int*)&_fdc; }
          local_d4 = (float)*piVar7;
          GL_DrawTexture(iVar3 + 0xf0,(float)piVar7[-2] + StatsPanelXOffset,
                       (float)piVar7[-1] + GridToWorldScale,local_d4,*(float*)&local_dc,0.0,0.0,
                       local_d4 * 0.0078125f,*(float*)&local_dc * 0.03125f,'\x01','\x01');
        }
        piVar7 = piVar7 + 5;
      }
    }
    else {
      local_d8 = 0x3c;
      GL_DrawTexture(0xf0,213.0,60.0,213.0,64.0,0.0,0.0,0.83203125,1.0,'\x01','\x01');
      if (InGameUiState == 0x74) {
        LegacyInputTextWidth = 100;
        UI_RenderInputField(0x116,0x62,0);
        LegacyInputTextWidth = 0x100;
      }
      else if (((InGameUiState == 0x72) || (InGameUiState == 0x7e)) || (InGameUiState == 0x98)) {
        LegacyInputTextWidth = 100;
        UI_RenderInputField(0xf3,0x62,0);
        LegacyInputTextWidth = 0x100;
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
      else if ((((InGameUiState == 0x80) || (InGameUiState == 0x77)) || (InGameUiState == 0x78)) ||
              (InGameUiState == 0x79)) {
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
  switch(InGameUiState) {
  case 0x15:
  case 0x47:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText15);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText15,iVar3,ptVar17);
    ppCVar11 = DlgText15;
    break;
  case 0x16:
  case 0x3f:
  case 0x41:
  case 0x45:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText16);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText16,iVar3,ptVar17);
    ppCVar11 = DlgText16;
    break;
  case 0x17:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText17);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText17,iVar3,ptVar17);
    ppCVar11 = DlgText17;
    break;
  case 0x18:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText18);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText18,iVar3,ptVar17);
    ppCVar11 = DlgText18;
    break;
  case 0x19:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText19);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText19,iVar3,ptVar17);
    ppCVar11 = DlgText19;
    break;
  case 0x1a:
  case 0x70:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1A_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1A_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText1A_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1A_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1A_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText1A_1;
    break;
  case 0x1b:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1B);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1B,iVar3,ptVar17);
    ppCVar11 = DlgText1B;
    break;
  case 0x1c:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1C);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1C,iVar3,ptVar17);
    ppCVar11 = DlgText1C;
    break;
  case 0x1d:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1D);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1D,iVar3,ptVar17);
    ppCVar11 = DlgText1D;
    break;
  case 0x1e:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1E);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1E,iVar3,ptVar17);
    ppCVar11 = DlgText1E;
    break;
  case 0x1f:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText1F);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText1F,iVar3,ptVar17);
    ppCVar11 = DlgText1F;
    break;
  case 0x20:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText20);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText20,iVar3,ptVar17);
    ppCVar11 = DlgText20;
    break;
  default:
    return InGameUiState + -0x15;
  case 0x22:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText22);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText22,iVar3,ptVar17);
    ppCVar11 = DlgText22;
    break;
  case 0x23:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText23);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText23,iVar3,ptVar17);
    ppCVar11 = DlgText23;
    break;
  case 0x24:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText24);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText24,iVar3,ptVar17);
    ppCVar11 = DlgText24;
    break;
  case 0x25:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText25);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText25,iVar3,ptVar17);
    ppCVar11 = DlgText25;
    break;
  case 0x26:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText26);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText26,iVar3,ptVar17);
    ppCVar11 = DlgText26;
    break;
  case 0x36:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText36_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText36_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText36_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText36_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText36_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText36_1;
    goto LAB_0051ca70;
  case 0x37:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText37);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText37,iVar3,ptVar17);
    ppCVar11 = DlgText37;
    goto LAB_0051ca70;
  case 0x3a:
    if (DeleteCharResult == 0) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText3A_0);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText3A_0,iVar3,ptVar17);
      ppCVar11 = DlgText3A_0;
    }
    else if (DeleteCharResult == 3) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText3A_3);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText3A_3,iVar3,ptVar17);
      ppCVar11 = DlgText3A_3;
    }
    else {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText8A);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText8A,iVar3,ptVar17);
      ppCVar11 = DlgText8A;
    }
    goto LAB_0051ca70;
  case 100:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText64);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText64,iVar3,ptVar17);
    ppCVar11 = DlgText64;
    goto LAB_0051ca70;
  case 0x65:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText65);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText65,iVar3,ptVar17);
    ppCVar11 = DlgText65;
    goto LAB_0051ca70;
  case 0x71:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText71);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText71,iVar3,ptVar17);
    ppCVar11 = DlgText71;
    goto LAB_0051ca70;
  case 0x72:
    m_dwTextColor = 0xff0000ff;
    // IDA 0x51C36C: sprintf(String, GlobalText[397], CharactersClient + 916*SelectedHero + 0x1C1).
    // GlobalText[397] es "Si deseas eliminar a %s," y el %s es el nombre del
    // personaje seleccionado.  Sin ese argumento printf tomaba un slot cualquiera
    // de la pila como char* -> AV de lectura dentro de ucrtbase.
    crt_sprintf(local_c8,&LegacyLoginAccountFmt,
                EntityName::Get(CharactersClient + CHARACTER_STRIDE * SelectedHero));
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText72_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText72_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText72_1;
    break;
  case 0x73:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText73_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText73_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText73_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText73_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText73_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText73_1;
    break;
  case 0x74:
    if (StorageGoldFlag == 0) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText74_0);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText74_0,iVar3,ptVar17);
      ppCVar11 = DlgText74_0;
    }
    else if (StorageGoldFlag == 1) {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText74_1);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText74_1,iVar3,ptVar17);
      ppCVar11 = DlgText74_1;
    }
    else {
      ptVar17 = &local_e4;
      iVar3 = Platform::TextLength((LPCSTR)DlgText74_2);
      gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText74_2,iVar3,ptVar17);
      ppCVar11 = DlgText74_2;
    }
    goto LAB_0051ca70;
  case 0x75:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText75);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText75,iVar3,ptVar17);
    ppCVar11 = DlgText75;
    goto LAB_0051ca70;
  case 0x76:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText76);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText76,iVar3,ptVar17);
    ppCVar11 = DlgText76;
    goto LAB_0051ca70;
  case 0x77:
    // IDA 0x51CD64: el indice que devuelve FindCharacterIndex indexa
    // CharactersClient y el nombre (+0x1C1) es el %s del formato
    // (GlobalText[418] = "%s").  El port descartaba el indice y no pasaba el
    // argumento -> AV de lectura dentro de ucrtbase.
    {
    int nameIdx = FindCharacterIndex(GuildInviteKey);
    crt_sprintf(local_c8,(const char *)PartyRequestFormatA,
                EntityName::Get(CharactersClient + CHARACTER_STRIDE * nameIdx));
    }
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    crt_sprintf(local_c8,&LegacyDuelOfferFmt);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x15;
    goto LAB_0051ca70;
  case 0x78:
  {
    // 0051AF50 obtains the visible character slot from 0045AC80 before
    // expanding the localized invitation format.  The prior void stub lost
    // that return value, so the party prompt had no inviter name.
    const int characterIndex = FindCharacterIndex((int)PartyInviteKey);
    const char* inviterName = (characterIndex < 400 && CharactersClient)
        ? EntityName::Get(CharactersClient + characterIndex * CHARACTER_STRIDE)
        : "";
    Platform::TextFormatTruncated(local_c8, sizeof(local_c8), Platform::TruncateText,
                (const char*)&PartyRequestFormatA, inviterName);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)local_c8,(LPSIZE)0x0,'\0',0);
    Platform::TextFormatTruncated(local_c8, sizeof(local_c8), Platform::TruncateText,
                (const char*)&PartyRequestFormatB, inviterName);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)local_c8);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)local_c8,iVar3,ptVar17);
    ppCVar11 = local_c8;
    pCVar6 = pCVar2 + 0x15;
    goto LAB_0051ca70;
  }
  case 0x79:
    // IDA: RenderErrorMessage 0x51C940 — solicitud entrante de Trade.
    // ProtocolCore guarda el nombre exacto de diez bytes en TradePartnerName;
    // GlobalText[418] aporta el formato localizado y GlobalText[419] la línea
    // de confirmación. ErrorMessage 128 es guerra de guild, no Trade.
    crt_sprintf(local_c8, (const char*)PartyRequestFormatA, TradePartnerName);
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
  case 0x7a:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText7A);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText7A,iVar3,ptVar17);
    ppCVar11 = DlgText7A;
    goto LAB_0051ca70;
  case 0x7b:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText7B);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText7B,iVar3,ptVar17);
    ppCVar11 = DlgText7B;
    goto LAB_0051ca70;
  case 0x7e:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)DlgText7E_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText7E_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText7E_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText7E_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText7E_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText7E_1;
    break;
  case 0x7f:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText7F);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText7F,iVar3,ptVar17);
    ppCVar11 = DlgText7F;
    goto LAB_0051ca70;
  case 0x80:
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
  case 0x84:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)DlgText84_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText84_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText84_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText84_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText84_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText84_1;
    goto LAB_0051ca70;
  case 0x85:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText85);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText85,iVar3,ptVar17);
    ppCVar11 = DlgText85;
    break;
  case 0x86:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText86);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText86,iVar3,ptVar17);
    ppCVar11 = DlgText86;
    break;
  case 0x87:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText87);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText87,iVar3,ptVar17);
    ppCVar11 = DlgText87;
    break;
  case 0x88:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText88);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText88,iVar3,ptVar17);
    ppCVar11 = DlgText88;
    break;
  case 0x89:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText89);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText89,iVar3,ptVar17);
    ppCVar11 = DlgText89;
    break;
  case 0x8a:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText8A);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText8A,iVar3,ptVar17);
    ppCVar11 = DlgText8A;
    break;
  case 0x8b:
  case 0x8d:
  case 0x8e:
    iVar3 = (((7 - DialogAnswerCountA) - MessageBoxLineCount) * 0x12) / 2 + 0x4c;
    if ((InGameUiState == 0x8e) && (iVar3 < 0x46)) {
      iVar3 = 0x46;
    }
    local_dc = 0;
    if (0 < MessageBoxLineCount) {
      pCVar6 = (LPCSTR)&MessageBoxCustomBuf;
      do {
        ptVar17 = &local_e4;
        iVar4 = Platform::TextLength(pCVar6);
        gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
        UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),iVar3,pCVar6,
                     (LPSIZE)0x0,'\0',0);
        iVar3 = iVar3 + 0x12;
        local_dc = local_dc + 1;
        pCVar6 = pCVar6 + 0x26;
      } while ((int)local_dc < MessageBoxLineCount);
    }
    local_d4 = (float)((MouseY - iVar3) / 0x12);
    if (MouseY - iVar3 < 0) {
      local_d4 = -NAN;
    }
    if ((InGameUiState == 0x8e) && (local_dc = 0, 0 < DialogAnswerCountA)) {
      const char* answer = (const char*)DialogAnswerBuffer;
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
      } while ((int)local_dc < DialogAnswerCountA);
    }
    Gfx::Color3f(1.0f, 1.0f, 1.0f);  // 0x3f800000 = bits de 1.0f
    return 0;
  case 0x8c:
    iVar3 = RenderMatchScore();
    return iVar3;
  case 0x8f:
    m_dwTextColor = 0xff0080ff;
    gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText8F);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText8F,iVar3,ptVar17);
    ppCVar11 = DlgText8F;
    break;
  case 0x90:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText8A);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText8A,iVar3,ptVar17);
    ppCVar11 = DlgText8A;
    break;
  case 0x91:
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)&StatsPanelLabel);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)&StatsPanelLabel,iVar3,ptVar17);
    ppCVar11 = StatsPanelLabel;
    break;
  case 0x97:
  case 0x99:
    local_dc = local_d8 + 0x1e;
    local_d8 = 0x15e;
    if (InGameUiState == 0x99) goto LAB_0051d371;
    if (ItemActionMode != 2) {
      if (ItemActionMode != 3) {
        if (ItemActionMode != 4) goto LAB_0051d371;
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
    if (MessageBoxLineCount < 1) {
      return MessageBoxLineCount;
    }
    do {
      Gfx::Color3f(1.0f, 0.7f, 0.0f);
      gFont.SelectFont(gFont.GetTextDC(),(HGDIOBJ)gFont.GetFont(FONT_BOLD));
      if (iVar3 == 0) {
        if (ItemActionMode == 1) {
          uVar10 = 0;
LAB_0051d3cf:
          Gfx::Color3f(1.0f, *(float*)&uVar10, *(float*)&uVar10);
        }
      }
      else if ((iVar3 == 3) && (ItemActionMode == 3)) {
        uVar10 = 0x3dcccccd;
        goto LAB_0051d3cf;
      }
      if (ItemActionMode == 2) {
        Gfx::Color3f(0.9f, 0.9f, 1.0f);
      }
      ptVar17 = &local_d0;
      pCVar6 = MessageBoxCustomBuf + iVar3 * 0x26;
      iVar4 = Platform::TextLength(pCVar6);
      gFont.MeasureText(gFont.GetTextDC(),pCVar6,iVar4,ptVar17);
      pCVar2 = (LPCSTR)(uintptr_t)local_dc;
      UI_RenderText((int)local_d8 - ((uint)(local_d0.cx * 0x280) / gWindow.GetWidth() >> 1),local_dc,pCVar6
                   ,(LPSIZE)0x0,'\0',0);
      local_dc = (unsigned int)(uintptr_t)(pCVar2 + 0x12);
      iVar3 = iVar3 + 1;
      if (MessageBoxLineCount <= iVar3) {
        return MessageBoxLineCount;
      }
    } while( true );
  case 0x98:
    ptVar17 = &local_e4;
    m_dwTextColor = 0xff0000ff;
    iVar3 = Platform::TextLength((LPCSTR)DlgText98_0);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText98_0,iVar3,ptVar17);
    UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)(pCVar2 + 9),
                 (LPCSTR)DlgText98_0,(LPSIZE)0x0,'\0',0);
    ptVar17 = &local_e4;
    iVar3 = Platform::TextLength((LPCSTR)DlgText98_1);
    gFont.MeasureText(gFont.GetTextDC(),(LPCSTR)DlgText98_1,iVar3,ptVar17);
    pCVar6 = pCVar2 + 0x15;
    ppCVar11 = DlgText98_1;
LAB_0051ca70:
    uVar9 = UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)pCVar6,
                         (LPCSTR)ppCVar11,(LPSIZE)0x0,'\0',0);
    return (int)uVar9;
  case 0x9a:
    uVar9 = GuildOverview_Render();
    return (int)uVar9;
  }
  uVar9 = UI_RenderText(0x140 - ((uint)(local_e4.cx * 0x280) / gWindow.GetWidth() >> 1),(unsigned int)(uintptr_t)pCVar6,
                       (LPCSTR)ppCVar11,(LPSIZE)0x0,'\0',0);
  return (int)uVar9;
}
