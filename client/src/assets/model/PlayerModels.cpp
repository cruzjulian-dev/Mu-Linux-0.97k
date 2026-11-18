// IDA: OpenPlayers @ 0x00506170 — Model_LoadPlayerAndItemMeshes
// Arma el pool de modelos, carga el cuerpo del jugador (slot 0x186), las
// armaduras de cada clase y la sombra (Content/player/models.toml), y fija las
// velocidades de animación del jugador.  Termina con SetAttackSpeed.
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "render/ResourceTables.h"
#include "render/Skinning.h"
#include "game/core/RenderDerivedState.h"
#include "game/item/ItemDefines.h"
#include "globals.h"
#include "functions.h"
#include "assets/model/Model_CustomAnim.h"

// Model_LoadItems — loads player body + all equipment model slots.
// Called once from OpenWorld (Map_LoadResources) during startup.
// IDA: OpenPlayers
void __cdecl Model_LoadPlayerAndItemMeshes(void)
{
    // DESVIACION: base estable y páginas bajo demanda. Conservar la muestra
    // RNG del offset original para no alterar la secuencia de simulación.
    (void)rand();
    // La base ya no cambia: invalidar explícitamente snapshots de la carga anterior.
    RenderDerivedState_Cancel();Skinning::Invalidate();
    ResourceTables::ResetModels();
    ModelsDump=ResourceTables::ModelBase();
    Models = reinterpret_cast<std::uintptr_t>(ModelsDump);

    // Modelos del cuerpo, las armaduras de cada clase y la sombra:
    // [models] de Content/player/models.toml.  Sale la rama que cargaba los .smd
    // de Data2/Player: sólo corría con DAT_0055a7c4 == 0, que a esta altura vale 1.
    AssetList_Load("player/models.toml", "models");

    // Velocidades de animación del jugador: IDA OpenPlayers, 0x5073AB..0x507622.
    // BMD::Open no inicializa PlaySpeed. Los bucles originales incrementan el
    // offset ANTES de escribir (offset - 12), incluida la última entrada. Se usan
    // índices de acción para respetar esos límites, sobre todo la pose de apoyo
    // de la elfa (140).
    // Modelo del jugador = Models + 188 * 390; Actions en +48, stride 16, speed en +4.
    {
        BYTE* playerModelBase = (BYTE*)(uintptr_t)Models;
        // Aux fields del struct player (no son acciones; offsets internos del modelo)
        *(DWORD*)(playerModelBase + BMD_STRIDE * 390 + 0x54 + BMD_FIELD_SHIFT) = 20;
        *(DWORD*)(playerModelBase + BMD_STRIDE * 390 + 0x58 + BMD_FIELD_SHIFT) = 6;
        *(DWORD*)(playerModelBase + BMD_STRIDE * 390 + 0x5c + BMD_FIELD_SHIFT) = 13;

        BYTE* actions = AsModel(playerModelBase + BMD_STRIDE * 390)->Actions;
        if (actions) {
            BYTE* A = actions;

            // action[1..12].speed = 0.28f  (idle/stop / walk variants)
            for (int action = 1; action <= 12; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3E8F5C29;  // 0.28f
            }
            // Spot speeds for stop/walk hands variants
            *(DWORD*)(A + ModelActionByte(52)) = 0x3E851EB8;  // 0.26f  action[3]
            *(DWORD*)(A + ModelActionByte(68)) = 0x3E75C28F;  // 0.24f  action[4]
            *(DWORD*)(A + ModelActionByte(84)) = 0x3E75C28F;  // 0.24f  action[5]
            *(DWORD*)(A + ModelActionByte(116)) = 0x3E6147AE;  // 0.22f  action[7]
            *(DWORD*)(A + ModelActionByte(132)) = 0x3E6147AE;  // 0.22f  action[8]

            // action[13..33].speed = 0.30f
            for (int action = 13; action <= 33; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3E99999A;
            }
            *(DWORD*)(A + ModelActionByte(340)) = 0x3EB33333;  // 0.35f action[21]
            *(DWORD*)(A + ModelActionByte(468)) = 0x3EB33333;  // 0.35f action[29]

            // action[92..130].speed = 0.32f
            for (int action = 92; action <= 130; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3EA3D70A;
            }
            // action[68..73]: gestos originales de 5.2 (HUSTLE, PROVOCATION, LOOK_AROUND,
            // CHEERS, RUSH1 largo, COME_UP) en los huecos de player.bmd. 5.2 les da 0.32f
            // (rango DEFENSE1..SHOCK de ZzzOpenData.cpp). Sin bandera de no-loop: terminan solos.
            for (int action = 68; action <= 73; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3EA3D70A;  // 0.32f
            }
            // action[131..132].speed = 0.45f
            for (int action = 131; action <= 132; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3EE66666;
            }
            // action[133..140].speed = 0.40f, incluidas las dos poses de apoyo.
            for (int action = 133; action <= 140; ++action) {
                *(DWORD*)(A + ModelActionByte(16 * action + 4)) = 0x3ECCCCCD;
            }

            // Per-action overrides
            *(DWORD*)(A + ModelActionByte(2084)) = 0x3ECCCCCD;  // 0.40f
            *(DWORD*)(A + ModelActionByte(1780)) = 0x3E8F5C29;  // 0.28f
            *(DWORD*)(A + ModelActionByte(1796)) = 0x3E8F5C29;  // 0.28f
            *(DWORD*)(A + ModelActionByte(2196)) = 0x3E4CCCCD;  // 0.20f
            *(DWORD*)(A + ModelActionByte(2212)) = 0x3E4CCCCD;  // 0.20f
            *(DWORD*)(A + ModelActionByte(1028)) = 0x3E4CCCCD;  // 0.20f
            *(DWORD*)(A + ModelActionByte(1044)) = 0x3E4CCCCD;  // 0.20f
            *(DWORD*)(A + ModelActionByte(1252)) = 0x3E75C28F;  // 0.24f  PLAYER_STOP_TWO_HAND_SWORD_TWO
            *(DWORD*)(A + ModelActionByte(1268)) = 0x3E99999A;  // 0.30f  PLAYER_WALK_TWO_HAND_SWORD_TWO
            *(DWORD*)(A + ModelActionByte(1284)) = 0x3E99999A;  // 0.30f  PLAYER_RUN_TWO_HAND_SWORD_TWO
            *(DWORD*)(A + ModelActionByte(1300)) = 0x3E75C28F;  // 0.24f  PLAYER_ATTACK_TWO_HAND_SWORD_TWO
            *(DWORD*)(A + ModelActionByte(1076)) = 0x3EE66666;  // 0.45f

            // Flag de no-loop (action+0): 1 = se detiene en el último frame.
            *(BYTE*)(A + ModelActionByte(2096)) = 1;
            *(BYTE*)(A + ModelActionByte(2112)) = 1;
            *(BYTE*)(A + ModelActionByte(2064)) = 1;
        }
    }

    // Acciones propias (Data/Player/CustomAnim.bmd), anexadas al final del jugador.
    CustomAnim_AppendToPlayer();

    // Lock animation state machine
    SetAttackSpeed();
}
