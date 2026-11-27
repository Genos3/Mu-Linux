// Terrain_RenderFace.cpp — port 1:1 desde IDA
// Cadena de dibujo de tiles texturados del terreno (eslabón #4-#5 de RenderTerrain):
//   RenderTerrainTile → RenderTerrainFace (RenderTerrainFace) →
//   FaceTexture (UVs) + RenderFace / RenderFaceAlpha / RenderFaceBlend (draw).
//
// Direcciones/buffers verificados por bytes de operando en IDA:
//   FaceTexture        0x004F7DF0   RenderFace        0x004F7A90
//   RenderFaceAlpha    0x004F7B80   RenderFaceBlend   0x004F7CE0
//   RenderTerrainFace  0x004F7FB0   BindTexture       0x00511480 (GL_BindTextureSlot)
//   Bitmaps base       0x083A7CA0 (stride 0x38, BITMAP_t: Width@0x20 Height@0x24)
//   TerrainMappingAlpha  = TerrainMappingAlpha (float[256²])   (mislabel "TerrainHeight")
//   TerrainMappingLayer1 = TerrainMappingLayer1 (BYTE[256²])
//   TerrainMappingLayer2 = TerrainMappingLayer2 (BYTE[256²])
//   PrimaryTerrainLight  = PrimaryTerrainLightStorage (float[256²][3])  ← 0x081CB608 (NO 0x07eab250;
//                          el macro PrimaryTerrainLight de structs.h apunta mal a
//                          0x07eab250 — buffer muerto. Se lee directo de PrimaryTerrainLightStorage,
//                          que Terrain_Water puebla per-frame desde BackTerrainLight 0x0828b608).
//   TerrainVertex        = g_TilePickBuf[12]   (4 corners contiguos)
//   TerrainTextureCoord  = g_TerrainTexCoord[8] (4 UV pairs)
//   TerrainGrassWind     = WaterWaveHeightTable (float[256²])     (mislabel "water-wave heights")
//   TerrainGrassTexture[(yi&0xFF)+1] = TerrainNoiseTable[yi&0xFF]
//   WaterMove=WaterMove  WaterTextureNumber=WaterTextureNumber  CurrentLayer=WaterRenderDisableFlag
//   TerrainFlag=TerrainFlag  0x0839BC86=TerrainNoDrawFlag  World
//   TerrainIndex1..4 = TerrainTileIdxTL/f0/f4/f8

#include "stdafx.h"
#include "Platform/Dreamcast/MUTrace.h"
#include "Render/BufferedDraw.h"
#include "Render/TerrainDraw.h"
#include "Render/Materials.h"
#include "Render/DrawDistance.h"
#include <map>
#include <optional>
#include <vector>


// ── Aliases locales que mapean los nombres IDA a nuestros globals ────────────
#define TER_FLAG    TerrainFlag
#define TER_IDX1    ((int)TerrainTileIdxTL)
#define TER_IDX2    ((int)TerrainTileIdxTR)
#define TER_IDX3    ((int)TerrainTileIdxBL)
#define TER_IDX4    ((int)TerrainTileIdxBR)
#define TER_ALPHA   TerrainMappingAlpha                          // TerrainMappingAlpha (float)
#define TER_L1      TerrainMappingLayer1                           // TerrainMappingLayer1 (BYTE)
#define TER_L2      TerrainMappingLayer2                           // TerrainMappingLayer2 (BYTE)
// Safe accessor: test-display mode may skip the water-wave table (cosmetic).
#define TER_WIND(idx) (WaterWaveHeightTable ? WaterWaveHeightTable[idx] : 0.0f)
#define TER_VERT    ((float(*)[3])&g_TilePickBuf[0])       // TerrainVertex[4][3]
#define TER_TEX     ((float(*)[2])&g_TerrainTexCoord[0])   // TerrainTextureCoord[4][2]
#define TER_LIGHT   ((float(*)[3])&PrimaryTerrainLightStorage[0])        // PrimaryTerrainLight[N][3]
#define TER_NODRAW  TerrainNoDrawFlag                           // 0x0839BC86 (skip-draw flag)

// The baked lightmap can contain strong per-channel differences (e.g. G=0 with
// R/B high) that show up as magenta through PVR's 8-bit vertex colours. Clamp
// the chroma while preserving brightness, the same way the legacy path does.
static void TerrainVertexLight(int index, float* out)
{
    if (index < 0 || index >= 256 * 256) {
        static int s_badIdx = 0;
        if (s_badIdx++ < 8)
            MU_TRACE("TLIGHT BADIDX: %d", index);
        out[0] = out[1] = out[2] = 1.0f;
        return;
    }
    float r = TER_LIGHT[index][0], g = TER_LIGHT[index][1], b = TER_LIGHT[index][2];
    int zeros = (r == 0.0f) + (g == 0.0f) + (b == 0.0f);
    if (zeros >= 2) { out[0] = out[1] = out[2] = 0.85f; return; }
    if (zeros == 1) {
        float avg = (r + g + b) * 0.5f;
        if (r == 0.0f) r = avg;
        if (g == 0.0f) g = avg;
        if (b == 0.0f) b = avg;
    }
    float mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
    float mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
    if (mx - mn > 0.4f) {
        float avg = (r + g + b) * (1.0f / 3.0f);
        out[0] = out[1] = out[2] = avg;
        return;
    }
    out[0] = r; out[1] = g; out[2] = b;
}

// DESVIACION R3a: replay de atributos/material final, sin tocar picking, RNG
// ni buffers de SIM. La captura sólo se habilita en RenderTerrain(EditFlag=0).
static TerrainDraw::Texture CaptureTexture(int texture) {
    const auto& bmp=Bitmaps[texture+35];
    return {bmp.TextureNumber,texture+35,(int)bmp.Width,(int)bmp.Height,(int)bmp.Components,bmp.Buffer};
}
static void CaptureUV(TerrainDraw::Tile& tile,bool upper) {
    for(int c=0;c<4;++c)for(int j=0;j<2;++j)tile.uv[c][(upper?2:0)+j]=TER_TEX[c][j];
}
// Test-display debug: force terrain onto a HUD texture slot known to render,
// to tell bad tile textures from a binding problem (0 = use real terrain slot).
static int TerrainForcedSlot() { return 0; }   // 0 = real terrain slot
static int TSlot(int slot) {
    if (TestSceneMapId < 0) return slot;
    return TerrainForcedSlot() ? TerrainForcedSlot() : slot;
}

// Test-display debug: flat material colours for the floor (like the PC
// DebugMaterials viewer) so we can tell whether terrain geometry is emitted.
static void TerrainDebugColor(int slot, float& r, float& g, float& b) {
    // Checkerboard by tile index so terrain geometry is unmistakable.
    const int x = TER_IDX1 & 0xff, y = (TER_IDX1 >> 8) & 0xff;
    const bool dark = ((x >> 2) ^ (y >> 2)) & 1;
    r = dark ? 0.10f : 0.20f + (float)(slot & 3) * 0.20f;
    g = dark ? 0.45f : 0.85f;
    b = dark ? 0.15f : 0.25f + (float)((slot >> 2) & 3) * 0.15f;
}
// Flat debug colours for the floor (like the PC DebugMaterials viewer).
static bool TerrainDebugFlat() { return false; }

// DESVIACION R4j: declarar el par material/slot preservando la secuencia
// material primero de las caras y del replay (incluido draw=false).
static void SelectTerrainMaterial(Materials::Preset preset,int slot) {
    if (TerrainDebugFlat()) {
        // No texture: draw flat vertex colours so geometry is unmistakable.
        Gfx::Disable(Gfx::Cap::Texture2D); Texture2DEnableCache = 0;
        Gfx::Disable(Gfx::Cap::Blend);     BlendModeCache = 0;
        Gfx::Disable(Gfx::Cap::AlphaTest); AlphaTestEnableCache = 0;
        return;
    }
    const Materials::TexturedMaterial material{{slot},Materials::Describe(preset),Materials::BindingOrder::MaterialFirst};
    if(Materials::ApplyTextured(material))return;
    if(preset==Materials::Preset::Opaque)GL_ResetState();
    else if(preset==Materials::Preset::Alpha)GL_SetBlendSrcOver('\x01');
    else GL_SetBlendAdditive();
    GL_BindTextureSlot(slot);
}
// DESVIACION: agrupar los quads del terreno por capa y textura. El 0.97k
// cambia de textura en casi cada tile y cada cambio corta el lote: con la
// distancia de dibujado ampliada eran ~19.000 envíos por cuadro. Las capas son
// coplanares (depth LEQUAL), así que dibujar todas las bases, después todas
// las capas con alfa y después el agua da la misma imagen. Modo 3: el pasto
// (sólo liga la textura; la mezcla la fija el caller).
namespace {
struct TerrainBatch {
    bool active=false;
    std::map<std::pair<int,int>,std::vector<BufferedDraw::Vertex>> buckets;   // (modo, slot)
} batch;
bool Batched(int mode,int slot,const BufferedDraw::Vertex* corners) {
    if(!batch.active)return false;
    auto& quads=batch.buckets[{mode,slot}];
    quads.insert(quads.end(),corners,corners+4);
    return true;
}
}
void Terrain_BeginBatch() {
    batch.active=true;
    for(auto& entry:batch.buckets)entry.second.clear();
}
void Terrain_EndBatch() {
    if(!batch.active)return;
    batch.active=false;
    if (TerrainDebugFlat()) {
        static int s_batchLog = 0;
        if (s_batchLog < 3) {
            ++s_batchLog;
            int total = 0;
            for (auto& e : batch.buckets) total += (int)e.second.size() / 4;
            MU_TRACE("TERRAIN_BATCH[%d]: buckets=%u quads=%d", s_batchLog,
                     (unsigned)batch.buckets.size(), total);
        }
    }
    for(auto& [key,quads]:batch.buckets) {
        if(quads.empty())continue;
        const int mode=key.first,slot=key.second;
        if(mode==3)GL_BindTextureSlot(slot);
        else SelectTerrainMaterial(mode==0?Materials::Preset::Opaque:mode==1?Materials::Preset::Alpha:Materials::Preset::Add,slot);
        if (TestSceneMapId >= 0) {
            // SelectTerrainMaterial/GL_ResetState re-enables depth and culling;
            // keep both off so textured terrain quads are not culled by PVR
            // (their winding is the opposite of the ship meshes).
            Gfx::Disable(Gfx::Cap::DepthTest);  DepthTestEnableCache  = 0;
            Gfx::Disable(Gfx::Cap::CullFace);   CullFaceEnableCache   = 0;
        }
        for(std::size_t i=0;i+3<quads.size();i+=4)BufferedDraw::Quad(&quads[i],mode!=3);
    }
}
static void ReplayTerrain(const TerrainDraw::Tile& tile,const TerrainDraw::FrameData& f,bool draw) {
    const auto pass=[&](int mode) {
        SelectTerrainMaterial(mode==0?Materials::Preset::Opaque:mode==1?Materials::Preset::Alpha:Materials::Preset::Add,
            mode==0?tile.base.slot:tile.upper.slot);
        BufferedDraw::Vertex corners[4];
        for(int c=0;c<4;++c) {
            const int i=tile.indices[c];const float a=f.alpha[i];const float* l=f.light+i*3;
            corners[c]={{tile.positions[c][0],tile.positions[c][1],tile.positions[c][2]},
                {mode==2?a:l[0],mode==2?a:l[1],mode==2?a:l[2],mode==1?a:1},
                {tile.uv[c][mode?2:0],tile.uv[c][mode?3:1],0,1}};
        }
        if(draw)BufferedDraw::Quad(corners,true);
        else { const auto& v=corners[3];BufferedDraw::NativeColor(v.color[0],v.color[1],v.color[2],v.color[3]);BufferedDraw::NativeUV(v.uv[0],v.uv[1]); }
    };
    if(draw||tile.layer==TerrainDraw::Layer::None)pass(0);
    if(tile.layer!=TerrainDraw::Layer::None)pass(tile.layer==TerrainDraw::Layer::Water?2:1);
}
void Terrain_BeginBufferedPass() {
    TerrainDraw::FrameData f;
    f.map=gMapManager.GetCurrentMap();f.scale=GridToWorldScale;
    f.heights=(const float*)&BackTerrainHeight[0];f.light=(const float*)&PrimaryTerrainLightStorage[0];
    f.alpha=TerrainMappingAlpha;f.fogPolicy=FogEnable!=0;
    TerrainDraw::Begin(f,ReplayTerrain);
}

// ── FaceTexture (FUN_004F7DF0) — computa UVs del tile en TerrainTextureCoord ──
void __cdecl FaceTexture(int Texture, float xf, float yf, char Water, char Scale)
{
    const int slot = TSlot(Texture + 35);
    if (Water) {
        static int s_waterLog = 0;
        if (s_waterLog < 4) {
            ++s_waterLog;
            MU_TRACE("WATERFACE: tex=%d slot=%d handle=%u uv=(%.2f,%.2f) size=%.0fx%.0f",
                     Texture, slot, Bitmaps[slot].TextureNumber, xf, yf,
                     Bitmaps[slot].Width, Bitmaps[slot].Height);
        }
    }
    BITMAP_t* bmp = &Bitmaps[slot];
    float Width, dHeight;
    if (Scale) { Width = 16.0f / bmp->Width; dHeight = 16.0f; }
    else       { Width = 64.0f / bmp->Width; dHeight = 64.0f; }
    float Height = dHeight / bmp->Height;
    float suf = Width * xf;
    float svf = Height * yf;
    float (*tc)[2] = TER_TEX;

    if (Water) {
        float k = Scale ? 0.0080000004f : 0.0020000001f;
        float w3 = TER_WIND(TER_IDX1) * k;
        float w4 = TER_WIND(TER_IDX2) * k;
        float yfa = w3 + svf;
        float xfb = WaterMove + suf;          // WaterMove + suf
        tc[0][0] = xfb;          tc[0][1] = yfa;
        float ub = w4 + svf;
        float va = xfb + Width;
        tc[1][0] = va;           tc[1][1] = ub;
        tc[2][0] = va;           tc[2][1] = ub + Height;
        tc[3][0] = xfb;          tc[3][1] = yfa + Height;
    } else {
        tc[0][0] = suf;          tc[0][1] = svf;
        float u = suf + Width;
        tc[1][0] = u;            tc[1][1] = svf;
        tc[2][0] = u;            tc[2][1] = svf + Height;
        tc[3][0] = suf;          tc[3][1] = svf + Height;
    }
}

// ── RenderFace (FUN_004F7A90) — quad opaco (base) ────────────────────────────
void __cdecl RenderFace(int Texture)
{
    if (TestSceneMapId >= 0) {
        static int s_faceLog = 0;
        ++s_faceLog;
        if (s_faceLog == 1 || s_faceLog == 300 || s_faceLog == 900) {
            MU_TRACE("RENDER_FACE[%d]: tex=%d slot=%d handle=%u idx=%d xy=(%d,%d) light=(%.2f,%.2f,%.2f) uv=(%.2f,%.2f)",
                     s_faceLog, Texture, Texture+35, Bitmaps[Texture+35].TextureNumber,
                     TER_IDX1, (int)TerrainTileIdxTL & 0xff, (int)TerrainTileIdxTL >> 8,
                     TER_LIGHT[TER_IDX1][0], TER_LIGHT[TER_IDX1][1], TER_LIGHT[TER_IDX1][2],
                     TER_TEX[0][0], TER_TEX[0][1]);
        }
    }
    if(!batch.active)SelectTerrainMaterial(Materials::Preset::Opaque,TSlot(Texture + 35));
    if (TER_NODRAW) return;
    float (*v)[3]  = TER_VERT;
    float (*tc)[2] = TER_TEX;
    float (*L)[3]  = TER_LIGHT;
    // DESVIACION: envío directo con los mismos atributos y orden de esquinas.
    const int indices[4]{TER_IDX1,TER_IDX2,TER_IDX3,TER_IDX4};
    float dr,dg,db;
    if (TerrainDebugFlat()) TerrainDebugColor(Texture,dr,dg,db);
    BufferedDraw::Vertex corners[4];
    for (int i=0;i<4;++i) {
        const int index=indices[i];
        float lit[3];
        if (TerrainDebugFlat()) { lit[0]=dr; lit[1]=dg; lit[2]=db; }
        else TerrainVertexLight(index, lit);
        corners[i]={{v[i][0],v[i][1],v[i][2]},
            {lit[0],lit[1],lit[2],1},
            {tc[i][0],tc[i][1],0,1}};
    }
    if(!Batched(0,TSlot(Texture + 35),corners))BufferedDraw::Quad(corners,true);
}

// ── RenderFaceAlpha (FUN_004F7B80) — overlay capa2 con alpha por vértice ──────
void __cdecl RenderFaceAlpha(int Texture)
{
    if(!batch.active)SelectTerrainMaterial(Materials::Preset::Alpha,TSlot(Texture + 35));
    if (TER_NODRAW) return;
    float (*v)[3]  = TER_VERT;
    float (*tc)[2] = TER_TEX;
    float (*L)[3]  = TER_LIGHT;
    // DESVIACION: envío directo con los mismos atributos y orden de esquinas.
    const int indices[4]{TER_IDX1,TER_IDX2,TER_IDX3,TER_IDX4};
    float dr,dg,db;
    if (TerrainDebugFlat()) TerrainDebugColor(Texture,dr,dg,db);
    BufferedDraw::Vertex corners[4];
    for (int i=0;i<4;++i) {
        const int index=indices[i];
        float lit[3];
        if (TerrainDebugFlat()) { lit[0]=dr; lit[1]=dg; lit[2]=db; }
        else TerrainVertexLight(index, lit);
        corners[i]={{v[i][0],v[i][1],v[i][2]},
            {lit[0],lit[1],lit[2],TER_ALPHA[index]},
            {tc[i][0],tc[i][1],0,1}};
    }
    if(!Batched(1,TSlot(Texture + 35),corners))BufferedDraw::Quad(corners,true);
}

// ── RenderFaceBlend (FUN_004F7CE0) — overlay de agua (modula por alpha gris) ──
void __cdecl RenderFaceBlend(int Texture)
{
    if(!batch.active)SelectTerrainMaterial(Materials::Preset::Add,TSlot(Texture + 35));
    if (TER_NODRAW) return;
    float (*v)[3]  = TER_VERT;
    float (*tc)[2] = TER_TEX;
    // DESVIACION: envío directo con los mismos atributos y orden de esquinas.
    const int indices[4]{TER_IDX1,TER_IDX2,TER_IDX3,TER_IDX4};
    float dr,dg,db;
    if (TerrainDebugFlat()) TerrainDebugColor(Texture+30,dr,dg,db);
    BufferedDraw::Vertex corners[4];
    for (int i=0;i<4;++i) {
        const int index=indices[i];
        const float a=TER_ALPHA[index];
        corners[i]={{v[i][0],v[i][1],v[i][2]},
            {TerrainDebugFlat()?dr:a,TerrainDebugFlat()?dg:a,TerrainDebugFlat()?db:a,1},{tc[i][0],tc[i][1],0,1}};
    }
    if(!Batched(2,TSlot(Texture + 35),corners))BufferedDraw::Quad(corners,true);
}

// ── RenderTerrainFace (FUN_004F7FB0) — dispatcher de capas/overlays ──────────
void __cdecl RenderTerrainFace(float xf, float yf, int xi, int yi, float lodf)
{
    (void)xi; (void)yi; (void)lodf;   // los TerrainVertex/Index ya los fijó RenderTerrainTile

    if (TER_FLAG == 2) {
        // ── capa de billboards de pasto/arena, movida por el viento ────────
        // Depende de 0x0055A76C (TerrainOverlayGate): nadie lo escribe, pero es una
        // constante de .data que en el binario vale 1.
        if (TER_ALPHA[TER_IDX1] <= 0.0f && TER_ALPHA[TER_IDX2] <= 0.0f &&
            TER_ALPHA[TER_IDX3] <= 0.0f && TER_ALPHA[TER_IDX4] <= 0.0f &&
            !WaterRenderDisableFlag && (gMapManager.GetCurrentMap() < 11 || gMapManager.GetCurrentMap() > 16))   // CurrentLayer
        {
            int slot = (int)(unsigned char)TER_L1[TER_IDX1] + 50;
            BITMAP_t* bmp = &Bitmaps[slot];
            // DESVIACION: distancia de dibujado; lejos no va el pasto.
            if (bmp->Buffer && DrawDistance::Detailed((xi + 0.5f) * GridToWorldScale, (yi + 0.5f) * GridToWorldScale)) {
                if(!batch.active)GL_BindTextureSlot(slot);     // BindTexture
                float yfa = bmp->Height + bmp->Height;
                float u = xf * 0.25f + (TerrainNoiseTable ? TerrainNoiseTable[(unsigned char)yi] : 0.0f); // TerrainGrassTexture[(yi&0xFF)+1]
                float (*tc)[2] = TER_TEX;
                tc[0][0] = u;          tc[0][1] = 0.0f;
                tc[1][0] = u + 0.25f;  tc[1][1] = 0.0f;
                tc[2][0] = u + 0.25f;  tc[2][1] = 1.0f;
                tc[3][0] = u;          tc[3][1] = 1.0f;
                float (*v)[3] = TER_VERT;
                v[3][0] = v[0][0];  v[3][1] = v[0][1];    // copy corner0 x,y → corner3
                float oldZ0 = v[0][2];
                v[0][2] = v[0][2] + yfa;
                v[3][2] = oldZ0;
                v[1][2] = v[2][2] + yfa;
                v[0][0] = v[0][0] - 50.0f;
                v[1][0] = v[2][0] - 50.0f;
                v[0][1] = v[0][1] + TER_WIND(TER_IDX1);
                v[1][1] = v[2][1] + TER_WIND(TER_IDX2);
                float (*L)[3] = TER_LIGHT;
                // DESVIACION: envío directo con los mismos atributos y orden de esquinas.
                const int indices[4]{TER_IDX1,TER_IDX2,TER_IDX3,TER_IDX4};
                BufferedDraw::Vertex corners[4];
                for (int i=0;i<4;++i) {
                    const int index=indices[i];
                    corners[i]={{v[i][0],v[i][1],v[i][2]},
                        {L[index][0],L[index][1],L[index][2],1},{tc[i][0],tc[i][1],0,1}};
                }
                if(!Batched(3,slot,corners))BufferedDraw::Quad(corners,false);
            }
        }
        return;
    }

    // ── main pass (TerrainFlag 0) ──
    char water = 0;
    char hasL2;
    int tex;
    if (TER_ALPHA[TER_IDX1] < 1.0f || TER_ALPHA[TER_IDX2] < 1.0f ||
        TER_ALPHA[TER_IDX3] < 1.0f || TER_ALPHA[TER_IDX4] < 1.0f)
    {
        unsigned char l1 = TER_L1[TER_IDX1];
        hasL2 = 1;
        tex = l1;
        if (l1 == 5) water = 1;
    } else {
        hasL2 = 0;
        tex = (unsigned char)TER_L2[TER_IDX1];
    }
    FaceTexture(tex, xf, yf, water, 0);     // FaceTexture
    const bool capture=TerrainDraw::Collecting() && !TER_NODRAW;
    std::optional<TerrainDraw::Tile> captured;
    if(capture) {
        auto& tile=captured.emplace();
        tile.x=xi;tile.y=yi;tile.base=CaptureTexture(tex);
        const int ids[]{TER_IDX1,TER_IDX2,TER_IDX3,TER_IDX4};
        for(int c=0;c<4;++c){tile.indices[c]=ids[c];for(int j=0;j<3;++j)tile.positions[c][j]=TER_VERT[c][j];}
        CaptureUV(tile,false);
    } else RenderFace(tex);                // RenderFace

    if (gMapManager.GetCurrentMap() == 7 && (unsigned char)TER_L2[TER_IDX1] == 5 &&
        (TER_ALPHA[TER_IDX1] > 0.0f || TER_ALPHA[TER_IDX2] > 0.0f ||
         TER_ALPHA[TER_IDX3] > 0.0f || TER_ALPHA[TER_IDX4] > 0.0f))
    {
        int t = (int)WaterTextureNumber + 30;      // WaterTextureNumber + 30
        FaceTexture(t, xf, yf, 0, 1);       // FaceTexture
        if(capture){captured->upper=CaptureTexture(t);captured->layer=TerrainDraw::Layer::Water;CaptureUV(*captured,true);}
        else RenderFaceBlend(t);                // RenderFaceBlend
    }
    else if (hasL2 &&
             (TER_ALPHA[TER_IDX1] > 0.0f || TER_ALPHA[TER_IDX2] > 0.0f ||
              TER_ALPHA[TER_IDX3] > 0.0f || TER_ALPHA[TER_IDX4] > 0.0f))
    {
        int t2 = (unsigned char)TER_L2[TER_IDX1];
        if (t2 != 5) water = 0;
        FaceTexture(t2, xf, yf, water, 0);  // FaceTexture
        if(capture){captured->upper=CaptureTexture(t2);captured->layer=TerrainDraw::Layer::Alpha;CaptureUV(*captured,true);}
        else RenderFaceAlpha(t2);               // RenderFaceAlpha
    }
    if(capture)TerrainDraw::Submit(*captured);
}
