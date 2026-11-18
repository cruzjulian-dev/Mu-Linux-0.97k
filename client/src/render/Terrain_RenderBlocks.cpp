// Terrain_RenderBlocks.cpp
//
// Culling/render de bloques de terreno y la variante de luz dinámica clampeada.

#include "stdafx.h"
#include "render/DrawDistance.h"
#include "globals.h"
#include "functions.h"
// AddTerrainLightClip @ 0x004F7800 (~74 lines) — adds clamped light to terrain buffer
// Iterates a square region around (xf,yf). Per cell: falloff = (Range-dist)/Range.
// Adds Light * falloff to Buffer, clamps to [0.0, 1.0].
// AddTerrainLightClip (0x004F7800).
//
// No tiene callers en nuestro árbol y no es código muerto: en el binario la
// llama 0x4C0E59, dentro de una función todavía no portada; cuando se porte,
// debe llamar a esta y no a AddTerrainLight.
//
// Diferencia entre las dos: esta clampea a [0, 1]; 0x4F76C0 solo evita negativos
// y deja que la luz supere 1.0 (que es lo que produce el resplandor del fuego).
void __cdecl AddTerrainLightClip(float xf, float yf, float Light[3], int Range, float Buffer[3]) {
    // 0x004F7800 — Add clamped light to terrain light buffer.
    // Iterates a square region of radius Range around (xf,yf) in grid coords.
    // Per cell: falloff = (Range - dist) / Range. Adds Light * falloff to Buffer, clamps [0,1].
    // Buffer is a 256x256 grid of float[3] (RGB), stride 0xC per cell.
    // _DAT_00552594 = world-to-grid scale factor.

    float gridX = xf * 0.01f;
    float gridY = yf * 0.01f;
    int centerX = (int)gridX;
    int centerY = (int)gridY;

    int yStart = centerY - Range;
    int yEnd   = centerY + Range;

    if (yStart > yEnd) return;

    float fY = (float)yStart;
    int iy = yStart;
    do {
        int xStart = centerX - Range;
        int xEnd   = centerX + Range;
        float fX = (float)xStart;
        if (xStart <= xEnd) {
            int ix = xStart;
            do {
                float dx = gridX - fX;
                float dy = gridY - fY;
                float dist = (float)sqrt(dx * dx + dy * dy);
                float falloff = ((float)Range - dist) / (float)Range;

                if (0.0f < falloff) {
                    // Index into 256x256 terrain light buffer
                    int cellIdx = ((unsigned int)(iy) & 0xFF) * 0x100 + ((unsigned int)(ix) & 0xFF);
                    float* pCell = Buffer + cellIdx * 3;

                    for (int ch = 0; ch < 3; ch++) {
                        float val = falloff * Light[ch] + pCell[ch];
                        pCell[ch] = val;
                        if (val < 0.0f) {
                            pCell[ch] = 0.0f;
                        } else if (val > 1.0f) {
                            pCell[ch] = 1.0f;
                        }
                    }
                }

                ix++;
                fX += 1.0f;  // 1.0f
            } while (ix <= xEnd);
        }
        fY += 1.0f;  // 1.0f
        iy++;
    } while (iy <= yEnd);
}

// RenderTerrainBlock @ 0x004F9720 (~33 lines) — renders a 4x4 terrain tile block
void __cdecl RenderTerrainBlock(float xf, float yf, int xi, int yi, bool EditFlag) {
    // 0x004F9720 — renders a 4x4 terrain tile block
    // _DAT_00552504 = 0.5f (half-tile center), _DAT_0055256c = 1.0f (tile stride)
    // CameraTopViewEnable = CameraTopViewEnabled
    // RenderTerrainTile (declared with int params but actually takes floats via stack)
    // TestFrustrum2D = TestFrustrum2D
    float startX = xf;
    int row = 0;
    do {
        float centerY = yf + 0.5f;  // 0.5f
        int col = 0;
        xf = startX;
        do {
            unsigned short visible = TestFrustrum2D(xf + 0.5f, centerY, 0.0f);
            if (visible || CameraTopViewEnabled != 0) {
                // RenderTerrainTile(xf, yf, col+xi, row+yi, 1.0f, 1, EditFlag)
                // functions.h declares first 2 params as int; reinterpret float bits
                RenderTerrainTile(*(int*)&xf, *(int*)&yf, col + xi, row + yi, 1.0f, 1, (int)EditFlag);
            }
            xf = xf + 1.0f;  // 1.0f
            col++;
        } while (col < 4);
        yf = yf + 1.0f;
        row++;
    } while (row < 4);
}

// RenderTerrainFrustrum @ 0x004F97E0 (~42 lines) — iterates visible terrain blocks
void __cdecl RenderTerrainFrustrum(bool EditFlag) {
    // 0x004F97E0 — iterates visible terrain blocks in frustum bounds
    // FrustrumBoundMinX_1 = FrustrumBoundMinX_1, FrustrumBoundMinY_1 = FrustrumBoundMinY_1
    // FrustrumBoundMaxX_1 = FrustrumBoundMaxX_1, FrustrumBoundMaxY_1 = FrustrumBoundMaxY_1
    // _DAT_0055264c = 2.0f (block center offset), _DAT_00552650 = 4.0f (block stride)
    // TestFrustrum2D = TestFrustrum2D, CameraTopViewEnable = CameraTopViewEnabled
    int yi = (int)FrustrumBoundMinY_1;  // FrustrumBoundMinY_1
    if (yi <= (int)FrustrumBoundMaxY_1) {  // FrustrumBoundMaxY_1
        float blockY = (float)yi;
        do {
            int maxX = (int)FrustrumBoundMaxX_1;  // FrustrumBoundMaxX_1
            if ((int)FrustrumBoundMinX_1 <= maxX) {
                float centerY = blockY + 2.0f;  // 2.0f
                int xi = (int)FrustrumBoundMinX_1;  // FrustrumBoundMinX_1
                float blockX = (float)xi;
                do {
                    unsigned short visible = TestFrustrum2D(blockX + 2.0f, centerY, -40.0f);
                    // DESVIACION: distancia de dibujado; el picking del mouse
                    // (EditFlag) no prueba los bloques fuera del radio de detalle,
                    // que con el alcance ampliado multiplicaban su costo.
                    if (visible && EditFlag &&
                        !DrawDistance::Detailed((blockX + 2.0f) * 100.0f, centerY * 100.0f))
                        visible = 0;
                    if (visible || CameraTopViewEnabled != 0) {
                        RenderTerrainBlock(blockX, blockY, xi, yi, EditFlag);
                    }
                    blockX = blockX + 4.0f;  // 4.0f
                    xi += 4;
                    maxX = (int)FrustrumBoundMaxX_1;
                } while (xi <= maxX);
            }
            blockY = blockY + 4.0f;  // 4.0f
            yi += 4;
        } while (yi <= (int)FrustrumBoundMaxY_1);
    }
}
