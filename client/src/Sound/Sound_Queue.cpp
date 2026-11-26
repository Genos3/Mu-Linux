// Sound_Queue.cpp
// Render_DrawSpritePool @ 0x00479730
//
// Pese al nombre del archivo, acá no hay sonido: son RenderSprites
// (Render_DrawSpritePool) y RenderSprite (Render_DrawSprite), que recorren y
// dibujan el pool de sprites en EffectPoolArray (kSpritePoolSize slots, IDA: 1002;
// +0 flag activo, +4 modo de blend), más los timers de avisos y de chat.

#include "stdafx.h"
#include "Game/RenderInterpolation.h"
#include "Config/UserSettings.h"

// Sprites stay in EffectPoolArray across render-only frames: the renderer no
// longer consumes them (the original client drew once per simulation tick, so
// it could afford to clear on draw). Clear stale slots once at the start of
// each simulation step instead. This removes the 435 KB simulationSprites
// snapshot that existed only to bridge 25 fps SIM to 60 fps render.
void Sprite_ClearStep() {
    for (int i = 0; i < kSpritePoolSize; ++i) EffectPoolArray[i * OBJECT_STRIDE] = 0;
}

// Render_DrawSpritePool = RenderSprites. Recorre el effect pool
// y por cada slot activo:
//   - dispatch GL state según blend mode en +4 (GL_SetBlendAdditive/90/80)
//   - llama Render_DrawSprite (RenderSprite) para dibujar el quad
//   - clear active flag
// Es la función que dibuja todos los sprites/glows/sparkles del pool (glow +9
// set, wing FX, weapon FX, particles, etc.). Itera los kSpritePoolSize slots por índice.
// IDA: RenderSprites
extern "C" void DbgLogPublic(const char* msg);
void __cdecl Render_DrawSpritePool(void)
{
    char *pcVar2 = EffectPoolArray;
    for (int i = 0; i < kSpritePoolSize; ++i, pcVar2 += OBJECT_STRIDE) {
        if (*pcVar2 != '\0') {
            int blend = *(int*)(pcVar2 + 4);
            if      (blend == 0) GL_SetBlendAdditive();
            else if (blend == 1) GL_SetBlendSrcAlpha();
            else if (blend == 2) GL_SetBlendSrcOver('\x01');
            Render_DrawSprite(AsObject(pcVar2));
        }
    }
}


// Render_DrawSprite @ 0x00479670
//
// RenderSprite: dibuja un slot del pool de sprites como billboard cuadrado vía
// RenderSprite_0 (Sprite_DrawTexturedQuad).
//
// Primero ajusta el factor +0x108 según el flag +0x160:
//   0 — resta 0.1f por frame; piso 0.2 (0x3e4ccccd)
//   1 — suma  0.1f por frame; tope 1.0 (0x3f800000)
//
// Argumentos de RenderSprite_0:
//   textura  = *(short*)(param_1 + 2)
//   posición = (float*)(param_1 + 0x10)
//   ancho/alto = *(float*)(param_1 + 0xc) * factor * Bitmaps[tipo] (+0/+4)
//   luz      = (float*)(param_1 + 0xe8)
//   rotación = *(float*)(param_1 + 0x24)
//
// Globals:
//   0.1f  — paso del fade
//   0.2f  — piso del fade (0.2)
//   1.0f  — float 1.0
//   BitmapsWidth/cc4 — Bitmaps[tipo]: ancho/alto de la textura (stride 0x38)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

// IDA: RenderSprite
void __cdecl Render_DrawSprite(const OBJECT* object)

{
  const BYTE* param_1 = reinterpret_cast<const BYTE*>(object);
    // DESVIACION (DLL Antilag, ret en 0x00479670): sin efectos estáticos.
    if (gUserSettings.GetAntilag(ANTILAG_STATIC_EFFECTS)) return;
  float fVar1;
  int iVar2;

  // El slot es efímero: CreateSprite fija el factor en 1 y el pool se
  // consume en esta pasada. Calcular el ajuste sin alterar la muestra base.
  fVar1 = *(float *)(param_1 + EntityOffset(0x108));
  if (*(char *)(param_1 + EntityOffset(0x160)) == 0) {
    fVar1 -= 0.1f;
    if (fVar1 < 0.2f) fVar1 = 0.2f;
  } else {
    fVar1 += 0.1f;
    if (fVar1 > 1.0f) fVar1 = 1.0f;
  }
  fVar1 *= *(float *)(param_1 + 0xc);
  iVar2 = *(short *)(param_1 + 2) * TEXTURE_STRIDE;
  RenderSprite_0((int)*(short *)(param_1 + 2),(float *)(param_1 + 0x10),
               fVar1 * *(float *)((char*)&BitmapsWidth + iVar2),fVar1 * *(float *)((char*)&BitmapsHeight + iVar2),
               (float *)(param_1 + 0xe8),*(float *)(param_1 + 0x24),0.0,0.0,1.0,1.0);
  return;
}


// Chat_TickNoticeTimer @ 0x0047fcb0 — Sound_Countdown1
// Decrements counter NoticeTime each frame.
// When it underflows below 1, resets to 300 and calls UI_AddNotice (queue advance).
// IDA: MoveNotices
void Chat_TickNoticeTimer(void)
{
  bool bVar1;

  bVar1 = NoticeTime < 1;
  NoticeTime = NoticeTime + -1;
  if (bVar1) {
    NoticeTime = 300;
    UI_AddNotice((char*)PeriodicNoticeText, (unsigned char)0);   // ahora es char[256]
  }
  return;
}


// Chat_TickMessageTimer @ 0x00480950 — Sound_Countdown2
// Decrements counter ChatScrollTimer each frame.
// When it underflows below 1, resets to 0x96 (150) and calls UIChatLogWindow_AddText.
// IDA: FUN_00480950
void Chat_TickMessageTimer(void)
{
  bool bVar1;

  bVar1 = ChatScrollTimer < 1;
  ChatScrollTimer = ChatScrollTimer + -1;

  // El tick dispara cada 150 frames y agrega la línea vacía que hace
  // scrollear el historial superior izquierdo.
  if (bVar1) {
    ChatScrollTimer = 0x96;
      // Este es el ENVEJECEDOR del historial de chat, no un "mensaje periodico".
      //
      // IDA 0x480950 llama incondicionalmente con strText (0x07E11DD8) y
      // NoticeTitleArg (0x07E11DDC), y a esos dos globals NO LOS ESCRIBE NADIE en
      // todo el binario: tienen un unico xref cada uno, que es esta misma
      // lectura.  O sea son cadenas VACIAS siempre, y ese es el punto.
      //
      // El primer branch de ChatLB_AddText (sub_40C940) es justamente
      // `if (!*src && !*msg)`: recorre la lista y hace ++nodo[+0x114] en cada
      // entrada.  El render de la linea (slot 23) lee ese contador y empuja la
      // fila hacia arriba, dejando de dibujarla cuando pasa el tope de filas
      // visibles.  O sea la caducidad del historial la produce esta llamada,
      // cada 150 frames.  MoveNotices (0x47FCB0) es el mismo patron para los
      // avisos: CreateNotice(PeriodicNoticeText, 0) cada 300, con otro buffer que
      // tampoco escribe nadie.
      //
      // No condicionar la llamada al contenido de los buffers: con buffers vacíos
      // (el caso normal) el historial dejaría de avanzar.
    UIChatLogWindow_AddText(NoticeTitleArg, NoticeTextArg, 0);
  }
  return;
}


