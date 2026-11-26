// HUD_Render.cpp
// Render_GameFrame   @ 0x004BBFB0  (56 lines, completo)
// Render_CharInfoPanel @ 0x004BC220 (234 lines, completo)
// Render_HPBars      @ ~0x004BC900  (87 lines, completo)
// Render_BottomHUD   @ ~0x004BD350  (200 lines, completo)
// Render_ChatBox     @ ~0x004BD600  (54 lines, completo)
// Render_MacroTimer  @ ~0x004BBD50  (70 lines, completo)
// Render_FloatingText @ ~0x004BC000 (85 lines, completo)
// Render_QuickButtons @ ~0x004BE000 (90 lines, completo)
// Render_Scene3D     @ ~0x004BF000  (54 lines, completo)
// Render_CharPartyInfo @ 0x004BEC00  (278 lines, COMPLETO)
// Render_CharNameTags  @ 0x004BE710  (264 lines, COMPLETO)
// Render_MapLoadText   @ 0x004BF2D0  (142 lines, COMPLETO)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_GameFrame @ 0x004BBFB0  (56 líneas, COMPLETO)
// ══════════════════════════════════════════════════════════════════════════════
//
//   Orden completo de llamadas HUD 2D:
//
//   void Render_GameFrame(void)
//   {
//     // World == 8: map-loading screen scrolling textures
//     if (World == 8) {
//       GL_SetBlendSrcOver('\x01');
//       glColor4f(1.0, 1.0, 1.0, 0.5);
//       GL_SetBlendAdditive();                 // Frame_UpdateTimer()
//       glColor3f(0.3, 0.3, 0.25);
//       // Scroll layer 1: tex 0x494 fullscreen, UV scrolled by ftol()%100000 * scale
//       FUN_005128c0(0x494, 0.0, 0.0, 640.0, 435.0, (float)(ftol()%100000)*ScrollWaveFreq, 0,2,3);
//       // Scroll layer 2: tex 0x495 fullscreen, UV scrolled diferente rate
//       FUN_005128c0(0x495, 0.0, 0.0, 640.0, 435.0, (float)(ftol()%100000)*SinFreqWorldTime,  0,3,2);
//     }
//     glColor3f(1.0, 1.0, 1.0);
//
//     // HUD principal (orden exacto)
//     Render_CharInfoPanel();      // estado de personaje (hp, nombre, buffs)
//     Render_HPBars();             // barras HP de mobs en pantalla
//     AntiTamper_HashMaintain_A(); // anti-tamper, no game logic
//     iVar1 = Render_CharPartyInfo(); // grupo/party info → devuelve Y offset
//     Render_CharNameTags(iVar1);  // name tags de jugadores en pantalla
//     Render_MacroTimer();         // timer de macro + notificaciones
//     Render_MapLoadText();        // texto de carga de mapa (sub 10..16)
//     Render_FloatingText();       // floating damage/heal numbers
//     Render_QuickButtons();       // botones de acción rápida
//     (**(code**)(*ChatHistoryWidget + 0x10))(); // vtable call (menu/inventory?)
//     Render_BottomHUD();          // barra inferior (HP/MP/SP + hotbar)
//     Render_ChatBox();            // caja de chat
//     AntiTamper_HashMaintain_B(); // anti-tamper
//     AntiTamper_HashMaintain_C(); // anti-tamper
//     AntiTamper_HashMaintain_D(); // anti-tamper
//     Render_Scene3D();            // 3D pass final (skills desde 2D ortho → back to 3D)
//     FUN_004f6050();              // — post-render 1
//     FUN_004eb070();              // — post-render 2
//   }
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_CharInfoPanel @ 0x004BC220  (234 líneas, COMPLETO)
// ══════════════════════════════════════════════════════════════════════════════
//
//   Renderiza el panel de información del personaje (nombre, buffs, estado).
//
//   void Render_CharInfoPanel(void)
//   {
//     GL_ResetState();   // GL_ResetBlend()
//     SetBackgroundTextColor = 0x80000000;
//
//     // BLOQUE 1: buff activo (LegacyTradeActive != '\0' && entity[0x1da] != -1)
//     if (LegacyTradeActive != '\0' && *(short*)(Hero+0x1da) != -1) {
//       GL_SetBlendSrcOver('\x01');   // GL_SetMode(1)
//       iVar3 = GetScreenWidth(); // Screen_GetWidth()
//       fVar2 = (float)iVar3 * 0.5f - 20.0f;   // X centrado
//       gFont.SelectFont(DC, font_small);
//       // Color según LegacyTradeParam:
//       m_dwTextColor = LegacyTradeParam ? 0x005901 : 0xffc301;   // verde o amarillo
//       CreateGuildMark((int)*(short*)(Hero+0x1da), '\x01'); // compone el mark propio
//       Texture_Draw2D(0x22, fVar2, 4.0, 8.0, 8.0, 0,0,1,1, '\x01', '\x01');
//       crt_sprintf(local_64, &BuffNameFmt);   // format buff name
//       DrawText(ftol()+0xd, 4, local_64, 0, 1, 0);
//       // Buscar nombre del buff en tabla GuildInfoTable (stride 0x50)
//       lpString = &GuildInfoTable + *(short*)(Hero+0x1da) * 0x50;
//       DrawText(ftol()+0x1e, 4, lpString, 0, 1, 0);
//
//       // Buscar icono del buff (search en tabla &GuildInfoTable..0x7ea51ec stride 0x50)
//       // comparando con &GuildNameCacheA
//       // Color hover: 0xff00a6ff o 0x9600
//       CreateGuildMark(found_idx, '\x01');   // compone el mark encontrado
//       Texture_Draw2D(0x22, fVar2, 19.0, 8.0, 8.0, ...);
//       crt_sprintf(local_64, &BuffStatFmt2);   // segundo stat
//       DrawText(ftol()+0xd, 0x13, local_64, 0, 1, 0);
//       DrawText(ftol()+0x1e, 0x13, &GuildNameCacheA, 0, 1, 0);
//     }
//
//     // BLOQUE 2: segundo buff (SoccerObserver != '\0')
//     if (SoccerObserver != '\0') {
//       // color 0xff003cff (rojo)
//       // buscar &GuildNameCacheB en tabla, mostrar similar a bloque 1
//       // BuffStatFmt3/ef4/efc → formatos para tercer/cuarto stat
//       // &GuildNameCacheC → nombre del buff 2
//     }
//
//     // BLOQUE 3: kill counter (SoccerTime > 0)
//     if (0 < SoccerTime) {
//       crt_sprintf(local_64, "%d/%d");   // Fmt_IntSlashInt
//       m_dwTextColor = 0xff968cff;        // lavanda
//       // Dibujar en (x, y+100): &lpString_00559f08
//       // Dibujar en (x, y+0xd): local_64
//     }
//   }
//
//   Entity offsets relevantes:
//   entity + 0x1da  → short: buff slot activo (-1 = ninguno)
//   GuildInfoTable    → tabla de buffs (stride 0x50 bytes)
//   GuildNameCacheA → nombre buff actual
//   GuildNameCacheB → nombre buff 2
//   GuildNameCacheC → nombre buff 2b
//   LegacyTradeActive..d33 → flags de buff activo
//   SoccerTime    → kill counter
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_HPBars @ 87 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Muestra barras de HP sobre mobs visibles en pantalla.
//
//   void Render_HPBars(void)
//   {
//     if (PartyNumber <= 0) return;
//     // Array: &PartyHPBarsTable, stride 0x24, count=PartyNumber
//     // Offset +4: entity index (-1 = vacío)
//     // Offset +0: HP percentage (0..10, capped a 10)
//     for each entry (entity_idx != -1):
//       pos = entity[entity_idx][+0x10..+0x18] (world pos)
//       z_offset = entity[entity_idx][+300] + entity[entity_idx][+0x18] + GridToWorldScale
//       Camera_ProjectWorldToScreen(&pos, &screenX, &screenY);  // World_ToScreen
//       screenX -= 0x13;
//       // Tooltip si mouse sobre barra:
//       if (screenX <= mouse_x < screenX+bar_w && screenY-2 <= mouse_y < screenY+6):
//         crt_sprintf(local_64, "HP [  %d0]");
//         LegacySetTextColor0 = 0xffd2e6ff;
//         DrawText(screenX, screenY-6, local_64, ...)
//       // Dibuja barra:
//       GL_SetMode(1);
//       glColor4f(0,0,0,0.5);      // sombra
//       GL_DrawRect(sx+1, sy+1, 42, 5);    // DrawRect background
//       GL_SetBlendAdditive();             // Frame_UpdateTimer()
//       glColor3f(0.2,0,0);        // rojo oscuro (vacío)
//       GL_DrawRect(sx, sy, 42, 5);
//       glColor3f(0.196,0.032,0);  // segmento oscuro
//       GL_DrawRect(sx+2, sy+2, 38, 1);
//       // Segmentos HP: up to 10 segments de 3px × 2px
//       hp_segs = min(entry[0], 10);
//       glColor3f(0.992, 0.032, 0); // verde-amarillo
//       for i in range(hp_segs):
//         GL_DrawRect(sx+2+i*4, sy+2, 3, 2);
//       GL_ResetBlend();
//
//     GL_ResetBlend();
//     glColor3f(1,1,1);
//   }
//
//   Globals:
//   PartyNumber    — count de barras activas
//   &PartyHPBarsTable   — array de barras (stride 0x24)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_BottomHUD @ 200 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Renderiza la barra inferior: esquinas, slots de hotbar.
//
//   void Render_BottomHUD(void)
//   {
//     GL_SetMode(1);
//     HudTextVisibleFlag = 0;
//     m_dwTextColor = 0xffffffff;
//     SetBackgroundTextColor = 0xff000000;
//     // Esquinas del HUD inferior
//     Texture_Draw2D(0xe9, 0,   387, 108, 45, 0,0,0.844,0.703, '\x01', '\x01');  // izq
//     Texture_Draw2D(0xe9, 532, 387, 108, 45, 0.844,0,-0.844,0.703, '\x01', '\x01'); // der (flip)
//     GL_ResetBlend();
//     // Barras de HP/MP/SP
//     Texture_Draw2D(0xe6, 0,   432, 256, 48, 0,0,1.0,0.75, '\x01', '\x01');  // left panel
//     // [HashTable anti-tamper block 1: ~60 líneas omitidas]
//     crt_sprintf(local_68, "%d/%d");   // Fmt_HpSlashMaxHp (HP/MaxHP)
//     // [HashTable anti-tamper block 2: ~40 líneas omitidas]
//     Texture_Draw2D(0xe7, 256, 432, 128, 48, 0,0,1.0,0.75, '\x01', '\x01');  // center panel
//     Texture_Draw2D(0xe8, 384, 432, 256, 48, 0,0,1.0,0.75, '\x01', '\x01');  // right panel
//     GL_ResetBlend();
//   }
//
//   Texturas del panel inferior:
//   0xe6 → left (HP bar area)   256×48 en (0, 432)
//   0xe7 → center               128×48 en (256, 432)
//   0xe8 → right (skills/mini)  256×48 en (384, 432)
//   0xe9 → corner piece         108×45 en (0/532, 387) — mirrored
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_ChatBox @ 54 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Renderiza la caja de chat (si InputEnable || ImeModeFlag).
//
//   void Render_ChatBox(void)
//   {
//     glColor3f(1,1,1);
//     gFont.SelectFont(DC, font_normal);
//     if (InputEnable != '\0' || ImeModeFlag != '\0') {
//       GL_ResetBlend();
//       Texture_Draw2D(0xf8, 186, 415, 134, 29, 0,0,0.520,0.906, ...); // input bg left
//       Texture_Draw2D(0xf9, 320, 415, 134, 29, 0,0,0.520,0.906, ...); // input bg right
//       GL_SetMode(1);
//       // Font size proporcional al ancho de pantalla
//       LegacyInputTextWidth = min((DAT_0056156c * 180) / 640, 0x100);
//       DrawInputCaret(0xbf, 0x1a6, 0);   // cursor en el input box de chat
//       LegacyInputTextWidth = (DAT_0056156c * 50) / 640;
//       DrawInputCaret(0x178, 0x1a6, 1);
//       LegacyInputTextWidth = 0x100;
//       m_dwTextColor = 0xffc8c8c8;   // gris claro
//       SetBackgroundTextColor = 0x64000000;   // transparente
//       // Iterar historial de chat (array en &ChatHistoryBuffer, stride 0x100)
//       iVar2 = 0;
//       lpString = &ChatHistoryBuffer;
//       iVar1 = ChatHistoryIndex;   // offset del mensaje más reciente
//       do {
//         if (iVar2 != iVar1):
//           DrawText(0x178, (iVar2-iVar1)*15 + 0x1a6, lpString, 0,1,0)
//         lpString += 0x100;
//         iVar2++;
//       } while (lpString < 0x7e118e4);
//       // Ícono de susurros activados (if m_bBlockWhisper = WhisperMuteFlag)
//       if (WhisperMuteFlag != '\0'):
//         Texture_Draw2D(0xef, 432, 420, 15, 16, 0,0,0.9375,1.0, ...);
//     }
//   }
//
//   Globals:
//   InputEnable   — chat input visible flag
//   ImeModeFlag   — chat overlay flag
//   ChatHistoryBuffer   — chat history buffer (stride 0x100, ~10 entradas hasta 0x7e118e4)
//   ChatHistoryIndex   — índice del mensaje más reciente
//   WhisperMuteFlag   — m_bBlockWhisper: interruptor de susurros (F3)
//   0xf8/0xf9     → background del chat input box
//   0xef          → ícono de desconexión
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_MacroTimer @ 70 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Muestra timer de macro activo y notificaciones de skills con cooldown.
//
//   void Render_MacroTimer(void)
//   {
//     // Barra de macro activo (MacroCooldown > 0)
//     if (0 < MacroCooldown) {
//       iVar2 = Screen_GetWidth();
//       iVar5 = MacroCooldown * 50;   // progress en pixels
//       fVar1 = ((float)iVar2 - 50.0f) * 0.5f;
//       GL_SetMode(1);
//       DrawText(ftol(), 0x188, "Macro Time", 0,1,0);   // Txt_MacroTime
//       FUN_004bbdd0(fVar1, 404.0, 50.0, 2.0, (float)(iVar5/100), '\0', '\x01');
//       // FUN_004bbdd0 = DrawProgressBar(x, y, max_w, h, pct, ?, filled)
//     }
//     glColor3f(1,1,1);
//
//     // Notificación de skill (SkillNotifyType > 0)
//     if (0 < SkillNotifyType) {
//       DVar3 = GetTickCount();
//       if (DVar3 - SkillNotifyStartTick > 30000):   // expirado (30s)
//         SkillNotifyType = 0; return;
//       GL_ResetBlend();
//       GL_SetMode('\0');
//       SetBackgroundTextColor = 0x80000000;
//       m_dwTextColor = 0xffff8080;   // rojo claro
//       // Skill name por índice (0..3 y 8+: &DAT_07d589f8, 4..7: &DAT_07d65e14, stride 300)
//       if (SkillNotifyType < 4 || SkillNotifyType > 7):
//         pCVar4 = &DAT_07d589f8 + SkillNotifyType * 300;
//       else:
//         pCVar4 = &DAT_07d65e14 + SkillNotifyType * 300;
//       // Mostrar "SkillName (%d)" con segundos restantes
//       Platform::TextFormat(local_100, pCVar4, 30 - (DVar3-SkillNotifyStartTick)/1000);
//       DrawText(10, 0x174, local_100, 0,1,0);
//     }
//   }
//
//   Globals:
//   MacroCooldown   — macro timer restante (decrementado en Game_MainLoop)
//   SkillNotifyType   — skill notification type (0=ninguno)
//   SkillNotifyStartTick   — tick cuando empezó la notificación
//   DAT_07d589f8   — tabla de nombres skill (stride 300, grupos 0-3 y 8+)
//   DAT_07d65e14   — tabla de nombres skill (stride 300, grupos 4-7)
//   FUN_004bbdd0   → DrawProgressBar(x,y,max_w,h,pct,?,filled)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_FloatingText @ 85 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Muestra números flotantes de daño/curación sobre entidades.
//   Dos pasadas: primero calcula posición 2D, luego resuelve colisiones, luego dibuja.
//
//   void Render_FloatingText(void)
//   {
//     gFont.SelectFont(DC, font_normal);
//     // Array en &DAT_07e0192c, stride 0x95*4 = 0x254
//     // Pasada 1: project world→screen
//     piVar7 = &DAT_07e0192c;
//     do {
//       if (piVar7[-0x83] > 0 || piVar7[-2] > 0):   // activo
//         iVar2 = *piVar7;   // entity ptr
//         if (iVar2 == 0):
//           pfVar5 = (float*)(piVar7+5);   // posición fija
//         else:
//           pos.x = entity[+0x10]; pos.y = entity[+0x14];
//           pos.z = entity[+300] + entity[+0x18] + 60.0f;
//         Camera_ProjectWorldToScreen(pfVar5, &screenX, &screenY);  // World_ToScreen
//         FloatingLabel_MeasureText((LPCSTR)(piVar7-0x8d));       // MeasureText(str)
//         piVar7[1] = screenX - (piVar7[3]*640/screen_w)/2;  // X centrado
//         piVar7[2] = screenY - 0x24;                         // Y elevado
//       piVar7 += 0x95;
//     } while (piVar7 < 0x7e101fc);
//
//     // Pasada 2: resolver colisiones entre labels (push-apart)
//     // dos loops anidados: si overlap horizontal y vertical → mover el más nuevo
//
//     // Pasada 3: dibujar
//     piVar7 = &LegacyBubblePoolOff22c;
//     do {
//       if (piVar7[-0x81] > 0 || *piVar7 > 0):
//         FUN_00480e00(piVar7[3], piVar7[4], (byte*)(piVar7-0x8b));  // DrawFloatLabel(x,y,str)
//       piVar7 += 0x95;
//     } while (piVar7 < 0x7e101f4);
//   }
//
//   Pool de labels:
//   &DAT_07e0192c   — array (stride 0x95*4), base index +5 para pos fija
//   Offsets: [-0x83]=timer activo, [-2]=?, [0]=entity ptr, [1]=screenX, [2]=screenY
//            [3]=text width, [4]=display Y (ajustado), [5..7]=pos fija
//            [-0x8d]=string label, [-0x8b]=display string
//   FloatingLabel_MeasureText   → MeasureText(str) → escribe en piVar7[3]
//   FUN_00480e00   → DrawFloatLabel(x, y, str)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_QuickButtons @ 90 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Botones de acción rápida (2 en esquina, 2 pequeños con tooltip, + skill slots).
//
//   void Render_QuickButtons(void)
//   {
//     glColor3f(1,1,1);
//     GL_ResetBlend();
//     m_dwTextColor = 0xffff8080;
//
//     // Botón 1 (GuildCreatorOpened != '\0')
//     if (GuildCreatorOpened != '\0') {
//       FUN_004f0480(0x1c2, 0);   // PrepareButton1(state)
//       glColor3f(1,1,1);
//       fVar1 = (float)HudBtn1PosX + 20.0f;
//       fVar2 = (float)HudBtn1PosY + 350.0f;
//       Texture_Draw2D(0x118, fVar1, fVar2, 70, 21, 0,0,2.1875,0.656, ...); // background
//       // Hit-test mouse → tex 0xf1 (hover) o 0xf2 (normal)
//       iVar4 = mouse_in_rect(fVar1,fVar2,70,21) ? 0xf2 : 0xf1;
//       Texture_Draw2D(iVar4, fVar1, fVar2, 70, 21, 0,0,0.547,0.656, ...);
//       // Botón offset alternativo
//       fVar1 += GridToWorldScale;
//       Texture_Draw2D(0x118, fVar1, fVar2, 70, 21, ...);
//       iVar4 = mouse_in_rect(...) ? 0xf4 : 0xf3;
//       Texture_Draw2D(iVar4, ...);
//     }
//
//     // Botón ícono pequeño 1 (GuildOpened != '\0')
//     if (GuildOpened != '\0') {
//       FUN_004f0810(0x1c2, 0);   // PrepareIconButton1(state)
//       glColor3f(1,1,1);
//       fVar1 = (float)GuildListStartXStorage + 25.0f;
//       fVar2 = (float)GuildListStartYStorage + 395.0f;
//       Texture_Draw2D(0x118, fVar1, fVar2, 24, 24, 0,0,0.75,0.75, ...);
//       // Tooltip si hover: DAT_07d39ef4, posición (ftol()-13, ftol())
//       if (mouse_in_rect(fVar1,fVar2,24,24)):
//         DrawText(ftol(), ftol()-13, &DAT_07d39ef4, ...);
//     }
//
//     // Botón ícono pequeño 2 (CharacterOpened != '\0')
//     if (CharacterOpened != '\0') {
//       FUN_004ecc60(0x1c2, 0);   // PrepareIconButton2(state)
//       // similar al anterior pero con CharacterInfoStartXStorage/9830 y tooltip DAT_07d3a14c
//     }
//
//     FUN_004ef160(0x1c2, 0);   // RenderSkillSlot_0(state)
//     FUN_004f0a50();           // RenderSkillSlot_1()
//     FUN_004f1270();           // RenderSkillSlot_2()
//     FUN_004f1f50();           // RenderSkillSlot_3()
//     FUN_004f27f0();           // RenderSkillSlot_4()
//     FUN_004f3170();           // RenderSkillSlot_5()
//     FUN_004f3c50();           // RenderSkillSlot_6()
//     FUN_004f46a0();           // RenderSkillSlot_7()
//     Quest_RenderDispatch(g_csQuest); // — (vtable?)
//     FUN_004f5570();           // RenderSkillSlot_8?
//   }
//
//   Globals:
//   GuildCreatorOpened   — flag botón 1 visible
//   GuildOpened   — flag icono 1 visible
//   CharacterOpened   — flag icono 2 visible
//   HudBtn1PosX/20 — posición X/Y botón 1
//   GuildListStartXStorage/84 — posición X/Y icono 1
//   CharacterInfoStartXStorage/30 — posición X/Y icono 2
//   DAT_07d39ef4   — tooltip string icono 1
//   DAT_07d3a14c   — tooltip string icono 2
//   0x118         → textura background botones
//   0xf1/f2/f3/f4 → texturas botones (normal/hover)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_Scene3D @ 54 líneas, COMPLETO
// ══════════════════════════════════════════════════════════════════════════════
//
//   Vuelve al modo 3D para dibujar skill effects sobre el HUD 2D ya renderizado.
//   Configura proyección + view matrix desde los datos de cámara precalculados.
//
//   void Render_Scene3D(void)
//   {
//     GL_End2D();        // GL_End2D()
//     glMatrixMode(GL_PROJECTION);
//     glPushMatrix();
//     glLoadIdentity();
//     GL_SetViewport(0, 0, DAT_0056156c, DAT_00561570);    // gluPerspective(fov, aspect, ...)
//     GL_SetPerspective(1.0, DAT_0056156c/DAT_00561570, CameraViewNearStorage, CameraViewFarStorage); // near/far
//     glMatrixMode(GL_MODELVIEW);
//     glPushMatrix();
//     glLoadIdentity();
//     GL_GetModelViewMatrix(&CameraMatrix);   // LoadCameraMatrix(mat_4x4)
//     GL_EnableDepthTest();                // EnableDepthTest()
//     GL_EnableDepthWrites();                // EnableDepthWrite()
//     Quest_RenderItemList(g_csQuest, '\x01', '\0');  // ObjPool_SetFlag(pool, true, false)
//
//     // Según modo de vista (GoldenArcherOpenType):
//     if (GoldenArcherOpenType == 0 || GoldenArcherOpenType == 3):
//       Skill_RenderEffects();    // Skill_RenderEffects()
//       if (ItemPickedFlag > 0 && ItemActionMode == 0):
//         TeleportEffect_Render();  // TeleportEffect_Render()
//       // Renderizar skill hotbar (items con cooldown):
//       for i in 0..N:
//         iVar1 = Item_FindQuickSlotByCategory(i);   // GetHotbarItem(i)
//         if iVar1 != -1:
//           RenderItem3D((float)local_14, 454.0, 20.0, 20.0,
//                        (&LegacyInventoryItems)[iVar1*0x22], (&LegacyInventoryLevels)[iVar1*0x11], 0, '\0');
//                        // DrawItemIcon(x, y, w, h, item_id, count, ?, ?)
//         local_14 += 0x1f;   // siguiente slot (X+31)
//       // X inicial = 0xd0 (208)
//     else:
//       FUN_004f54b0();    // PvP_SkillRender() o similar
//
//     // Reset camera para 2D
//     glLoadIdentity();
//     glTranslatef(-CameraPositionX, -CameraPositionY, -CameraPositionZ);
//     GL_GetModelViewMatrix(&CameraMatrix);
//     Camera_BuildMouseRay(100, 100, local_c);   // Camera_SetupHUD(x,y,out)
//     glPopMatrix();
//     glPopMatrix();
//     GL_Begin2D();   // GL_SetupOrtho2D()
//   }
//
//   Globals:
//   CameraMatrix   — camera matrix (4×4 floats)
//   DAT_0056156c   — screen_width
//   DAT_00561570   — screen_height
//   CameraViewNearStorage   — near_clip
//   CameraViewFarStorage   — far_clip
//   CameraPositionXBits/d8/dc — camera world position (X/Y/Z)
//   GoldenArcherOpenType   — view mode (0=normal, 3=?, otras=PvP?)
//   ItemPickedFlag   — teleport effect active
//   ItemActionMode   — teleport flag
//   LegacyInventoryItems   — hotbar item array (stride 0x22*2 = item_id)
//   LegacyInventoryLevels   — hotbar count array (stride 0x11*4)
//   RenderItem3D   → DrawItemIcon(x,y,w,h,item_id,count,?,shadow)
//   Skill_RenderEffects   → Skill_RenderEffects()
//   TeleportEffect_Render   → TeleportEffect_Render()
//   Item_FindQuickSlotByCategory   → GetHotbarItem(slot_idx) → item_idx
//   GL_GetModelViewMatrix   → LoadCameraMatrix(mat_ptr)
//   GL_EnableDepthTest   → EnableDepthTest()
//   GL_EnableDepthWrites   → EnableDepthWrite()
//   Camera_BuildMouseRay   → Camera_SetupHUD(x,y,out)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_CharPartyInfo @ 0x004BEC00  (278 líneas, COMPLETO)
// ══════════════════════════════════════════════════════════════════════════════
//
//   Renderiza el panel de party/grupo. Retorna Y offset para Render_CharNameTags.
//
//   int Render_CharPartyInfo(void)
//   {
//     int   retY  = 0xf;
//     float hpOff = (PartyNumber > 0) ? 50.0f : 0.0f;  // margen si hay HP bars
//
//     // Guard: jugador en modo party  (entity[0x2b8] ∈ 0x330..0x333)
//     if (0x32f < *(short*)(Hero + 0x2b8) < 0x334) {
//
//       int   sw     = GetScreenWidth();   // Screen_GetWidth()
//       float base_x = ((float)sw - 50.0f) - hpOff - 15.0f;
//
//       // [HashTable anti-tamper ~60 líneas — omitido]
//
//       byte bHP = *(byte*)(CharacterMachinePtr + 0x452);   // HP% del líder (0..255)
//       GL_SetBlendSrcOver('\x01');   // SetBlendMode(alpha)
//
//       // Icono/nombre del líder según sub-modo de party:
//       int subMode = *(short*)(Hero + 0x2b8);
//       char* pcName = (subMode == 0x332) ? &DAT_07d43d28 :
//                      (subMode == 0x333) ? &DAT_07d43bfc :
//                      (char*)((subMode - 400) * 0x40 + ItemAttributeBase);  // sprite
//
//       // Platform::TextLength(name) + GetTextExtentPointA(g_hDC, name, len, &sz)
//       // Text_MeasureBox(ftol()+0x32, ftol(), name, 0, '\0', 0)
//       //   → DrawText(x+50, y, name, shadow=0, italic=0, flags=0)
//
//       // FUN_004bbdd0(base_x, (bHP*50/255)+12.0f, 50.0, 2.0, pct, 0, 1)
//       //   → DrawHPBar(x, y_scaled, w=50, h=2, fill%, mirror=0, colored=1)
//
//       retY = (int)lVar18 + 0x17;   // Y = base + 23 (para name tags encima)
//     }
//
//     // Bloque secundario: barra de party global (SummonLife = nro de miembros)
//     if (SummonLife != 0) {
//       int sw2     = GetScreenWidth();
//       int bar_w   = SummonLife * 0x32;   // member_count × 50
//       float base2 = ((float)sw2 - 50.0f) - 150.0f;
//       GL_SetBlendSrcOver('\x01');
//       // Text_MeasureBox(ftol()+0x32, 4, &DAT_07d43e54, 0, '\0', 0)  // label "Party"
//       // FUN_004bbdd0(base2, 16.0, 50.0, 2.0, bar_w/100, 0, 1)    // barra HP party
//     }
//     return retY;
//   }
//
//   Globals:
//   Hero          — player entity ptr (slot 0)
//   PartyNumber          — HP bars visibles (>0 → agregar offset)
//   CharacterMachinePtr + 0x452  — HP byte del líder de party (0..255)
//   SummonLife          — cantidad de miembros de party
//   50.0f         — ancho del panel
//   15.0f         — margen extra panel
//   12.0f         — Y base de la barra HP
//   150.0f         — margen X barra secundaria
//   DAT_07d43d28          — nombre para modo guild (subMode 0x332)
//   DAT_07d43bfc          — nombre para modo trade (subMode 0x333)
//   DAT_07d43e54          — label "Party"
//   GetScreenWidth          → Screen_GetWidth()
//   FUN_004bbdd0          → DrawHPBar(x, y, w, h, fill_pct, mirror, color)
//   Text_MeasureBox          → DrawText(x, y, str, unk, italic, flags)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_CharNameTags @ 0x004BE710  (264 líneas, COMPLETO)
// ══════════════════════════════════════════════════════════════════════════════
//
//   Dibuja etiquetas de nombre flotante sobre jugadores visibles en pantalla.
//   param_1 = Y offset de Render_CharPartyInfo() (inicio Y de los textos).
//
//   void __cdecl Render_CharNameTags(int param_1)
//   {
//     // [Anti-tamper HashTable sobre TradeOpened — ~25 líneas, omitido]
//     char antiFlag = TradeOpened;
//
//     if (antiFlag != '\x01') {   // '\x01' = bloqueado por anti-tamper
//
//       float hpOff  = (PartyNumber > 0) ? 50.0f : 0.0f;
//       glColor3f(1.0, 1.0, 1.0);
//       GL_SetBlendSrcOver('\x01');        // SetBlendMode(alpha)
//       int sw      = GetScreenWidth();
//       float rightX = (float)sw - hpOff - 10.0f;  // límite X derecho
//       float idx    = 0.0f;    // offset de byte en lista on-screen (stride 0x44)
//       int   textY  = 0;       // desplazamiento Y acumulado entre textos
//
//       // Loop sobre las 12 ranuras on-screen (0x330 / 0x44 = 12 entidades máx)
//       do {
//         // [Anti-tamper HashTable por ranura — omitido]
//         // array on-screen: CharacterMachinePtr + 0x218  (stride 0x44)
//         short entType = *(short*)((int)idx + 0x218 + (int)CharacterMachinePtr);
//
//         if (entType == -1) {
//           // Ranura vacía: HashTable cleanup, continúa
//         } else {
//           // Excluir: 0x87=NPC-A, 0x8f=NPC-B, 0x1a0..0x1a3=map objects
//           if (entType != 0x87 && entType != 0x8f &&
//               !(0x1a0 <= entType && entType <= 0x1a3)) {
//
//             int level = CalcMaxDurability(psVar1, entType*0x40+ItemAttributeBase, field>>3&0xf);
//             level &= 0xffff;
//             if (*psVar1 == 0x1aa) level = 200;  // GM: nivel forzado a 200
//
//             // Color del name tag según HP ratio vs nivel:
//             byte hp = entity[+0xd];
//             if (hp == 0) {
//               entity[+0x20] = 8;           // muerto
//               m_dwTextColor  = 0x800a0aff;  // rojo oscuro
//               SetBackgroundTextColor  = 0x800a0aff;
//             } else {
//               float hpF = (float)hp;
//               if      (hpF > (float)level * 0.2f) { entity[+0x20]=7; m_dwTextColor=0xff0a0aff; }  // rojo
//               else if (hpF > (float)level * 0.3f) { entity[+0x20]=6; m_dwTextColor=0xff009cff; }  // naranja
//               else if (hpF > (float)level * HalfScale) { entity[+0x20]=5; m_dwTextColor=0xff00e4ff; }  // verde
//               // else hp muy alto → sin cambio de color
//             }
//
//             // Formatear nombre + HP:  crt_sprintf(buf, "%s %d/%d", name, hp, maxhp)
//             // HudTextVisibleFlag = 1  (flag texto visible)
//             // Text_MeasureBox(ftol()+textY, ftol(), buf, 0, '\0', 0)
//             textY += 0x0c;   // siguiente nombre 12px más abajo
//           }
//         }
//         idx = (float)((int)idx + 0x44);   // siguiente ranura
//       } while ((int)idx < 0x330);
//
//       SetBackgroundTextColor = 0x80000000;   // reset blend
//     }
//   }
//
//   Globals:
//   TradeOpened          — anti-tamper flag ('\x01' → skip toda la función)
//   PartyNumber          — count HP bars (>0 → offset 50px desde derecha)
//   CharacterMachinePtr + 0x218  — lista entidades on-screen (stride 0x44, 12 slots)
//   entity[+0xd]          — HP byte actual (0 = muerto)
//   entity[+0x20]         — estado visual del name tag (5..8)
//   HudTextVisibleFlag          — flag "texto visible"
//   m_dwTextColor          — color texto ABGR
//   SetBackgroundTextColor          — blend color
//   10.0f         — margen X derecho
//   0.2f/0.3f/HalfScale — umbrales HP (alto/medio/bajo)
//   Tipos excluidos de name tags: 0x87=NPC-A, 0x8f=NPC-B, 0x1a0..0x1a3=map objects
//   GetScreenWidth          → Screen_GetWidth()
//   CalcMaxDurability          → GetEntityLevel(ptr, spriteData, field) → int
//   Text_MeasureBox          → DrawText(x, y, str, unk, italic, flags)
//
// ══════════════════════════════════════════════════════════════════════════════
// Render_MapLoadText @ 0x004BF2D0  (142 líneas, COMPLETO)
// ══════════════════════════════════════════════════════════════════════════════
//
//   Overlay de carga de mapa/dungeon, visible cuando World ∈ [10..16].
//   Retorna DAT_07e11d88 (tipo de texto activo, 0=off).
//
//   uint Render_MapLoadText(void)
//   {
//     uint type = DAT_07e11d88;  // 0=off, 1=dungeon, 2=PvP, 5=instancia especial
//
//     if (DAT_07e11d88 != 0 && 10 < World && World < 0x11) {
//
//       GL_ResetState();            // GL_ResetBlend()
//       GL_SetBlendSrcOver('\0');        // SetBlendMode(none)
//       glColor3f(1.0, 1.0, 1.0);
//       SetBackgroundTextColor = 0;
//       m_dwTextColor = 0xff0096ff; // azul inicial
//
//       // Para tipos 1 (dungeon), 2 (PvP) y 5 (especial):
//       if ((DAT_07e11d88 < 3 || DAT_07e11d88 == 5) && LegacyMatchTime > 0) {
//
//         // Línea 1: "jugadores actuales / máximo" (solo si hay límite)
//         if (LegacyMaxKillMonster != 0xffff) {
//           LPCSTR fmt = (DAT_07e11d88 == 5) ? &DAT_07d693fc : &DAT_07d691a4;
//           Platform::TextFormat(local_100, fmt, LegacyKillMonster, LegacyMaxKillMonster);
//           gFont.SelectFont(g_hDC, DAT_055ca010);        // fuente bold
//           FUN_00514270(0x23a, 0x159, local_100);    // DrawTextCenter(570, 345, str)
//         }
//
//         // Línea 2: timer total (vía ftol())
//         gFont.SelectFont(g_hDC, DAT_055ca010);
//         Platform::TextFormat(local_100, &param_2_07d692d0);
//         FUN_00514270(0x23a, (int)__ftol(), local_100);
//
//         // Calcular y construir string "MM:SS" mediante strcat inline:
//         int mins = LegacyMatchTime / 60;
//         int secs = LegacyMatchTime - mins * 60;
//         Platform::TextFormat(local_100, Txt_DashPadR, mins);   // "%2d:"
//         if (secs >= 0) {
//           Platform::TextFormat(local_10c, Txt_Dash, secs);   // "%2d"
//           // strcat(local_100, local_10c) — inline strlen+memcpy
//         }
//
//         if (mins < 5)  m_dwTextColor = 0xff2020ff;    // rojo (urgente, <5 min)
//
//         if (mins < 15) {
//           // Parpadeo de segundos: (int)(ftol() % 60)
//           // Platform::TextFormat(local_10c, ":%2d", blink_sec)
//           // strcat inline → local_100 += local_10c
//         }
//
//         // Línea final: "MM:SS" con fuente grande
//         gFont.SelectFont(g_hDC, DAT_055ca014);            // fuente bold2
//         FUN_00514270(0x23a, (int)__ftol(), local_100);
//       }
//     }
//     return type;
//   }
//
//   Globals:
//   DAT_07e11d88   — tipo de overlay (0=off, 1=dungeon, 2=PvP, 5=especial)
//   World   — World (10..16 = loading overlay activo)
//   LegacyMatchTime   — segundos restantes (int)
//   LegacyMaxKillMonster   — límite jugadores (0xffff = sin límite)
//   LegacyKillMonster   — jugadores actuales
//   DAT_07d691a4   — format "current/max" para tipos 1/2
//   DAT_07d693fc   — format para tipo 5
//   DAT_07d692d0   — format timer total
//   DAT_055ca010   — HFONT bold
//   DAT_055ca014   — HFONT bold2 (grande, para MM:SS final)
//   Txt_DashPadR — "%2d:"  (minutos)
//   Txt_Dash   — "%2d"   (segundos)
//   Txt_DashPadL — ":%2d"  (segundos parpadeo)
//   FUN_00514270   → DrawTextCenter(x, y, str)
//   GL_ResetState   → GL_ResetBlend()
//   GL_SetBlendSrcOver   → SetBlendMode(mode)

#include "stdafx.h"
#include "Platform/Text.h"
#include "Render/HUD_Render.h"
