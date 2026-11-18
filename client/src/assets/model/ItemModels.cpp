// IDA: OpenItems @ 0x005079d0 — Model_LoadItemMeshes
// Carga los modelos de los items: [models] de Content/item/models.toml.
//
// Se sacó la rama que cargaba los .smd sueltos de Data2/Item (sólo corría con
// DAT_0055a7c4 == 0, que a esta altura vale 1, igual que en el binario) y los
// `DAT_0055a7c4 = 1` intercalados, que no cambiaban el valor.
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenItems
void __cdecl Model_LoadItemMeshes(void)
{
    AssetList_Load("item/models.toml", "models");

    // NoneBlendMesh flags.
    // Marca el mesh del FILO (emisivo) de estas armas como NoneBlendMesh=1 (mesh+0),
    // que BMD_DrawMesh saltea en el path chrome/oil/blur (línea "if (*pcVar1) return").
    // Efecto: el glow +N chrome NO se pinta sobre el filo (que conserva su luz propia);
    // va al hilt/mango. Sin esto, el chrome cubría el filo → "barra dorada".
    //
    // Fiel a IDA OpenItems (0x005079D0 L245-249). Los offsets son BYTES dentro del
    // array de modelos (Models = DAT_05828d58, stride 0xBC, campo Meshs en +0x28), NO
    // direcciones absolutas ni índices ×4.
    // `Models + 0x16c68` = Model[496].Meshs; `+0x28` = mesh[1].
    //   0x16c68 = 496*0xBC+0x28 → Light Spear (MODEL_SPEAR)    mesh[1]
    //   0x12d40 = 410*0xBC+0x28 → Light Saber (MODEL_SWORD+10) mesh[1]
    //   0x19fd0 = 566*0xBC+0x28 → Staff+6                      mesh[2] (+0x50)
    //   0x15950 = 470*0xBC+0x28 → Mace+6                       mesh[1]
    //   0x2be38 = 956*0xBC+0x28 → Event+9                      mesh[1]
    {
        BYTE* m;
        m = AsModel(Models + BMD_STRIDE * 496)->Meshs; if (m) m[sizeof(ClothModelMesh) * 1] = 1;
        m = AsModel(Models + BMD_STRIDE * 410)->Meshs; if (m) m[sizeof(ClothModelMesh) * 1] = 1;
        m = AsModel(Models + BMD_STRIDE * 566)->Meshs; if (m) m[sizeof(ClothModelMesh) * 2] = 1;
        m = AsModel(Models + BMD_STRIDE * 470)->Meshs; if (m) m[sizeof(ClothModelMesh) * 1] = 1;
        m = AsModel(Models + BMD_STRIDE * 956)->Meshs; if (m) m[sizeof(ClothModelMesh) * 1] = 1;
    }
}
