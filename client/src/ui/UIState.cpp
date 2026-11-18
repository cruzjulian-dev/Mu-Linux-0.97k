#include "stdafx.h"
#include "ui/UIState.h"

namespace UIState {

bool HasTextInput()
{
    // DLL Defines.h: CheckInputInterfaces; TabInputEnable es TabInputEnable.
    return InputEnable || GuildInputEnable || GoldInputEnable || TabInputEnable;
}

bool HasModalDialog() { return ErrorMessage != MESSAGE_NONE; } // ErrorMessage
bool IsItemMovePending() { return EquipmentItem != 0; } // EquipmentItem
bool IsMixBusy() { return MixState != 0; }

// IDA: GetScreenWidth (0x004CB520), flag de apertura de quest.
bool IsQuestPanelOpen()
{
    constexpr size_t OpenFlag = 0x1C87F;
    return g_csQuest && *((const BYTE*)(uintptr_t)g_csQuest + OpenFlag) != 0;
}

bool HasRightPanel()
{
    return InventoryOpened || CharacterOpened || GuildOpened || PartyOpened ||
        ShopOpened || WarehouseOpened || ChaosMixOpened || TradeOpened ||
        EventWindowOpened || GuildCreatorOpened || _g_bEventChipDialogEnable ||
        ServerDivisionOpened || IsQuestPanelOpen();
}

bool CanOpenInformationalPanel()
{
    // Conserva los bloqueos de M/H; la alternancia lateral se integra con su UI.
    return SceneFlag == 5 && !HasTextInput() && !HasModalDialog() &&
        !IsItemMovePending() && !HasRightPanel();
}

// Panel lateral (H): se alterna con inventario/personaje/guild/party, pero no se
// abre sobre una ventana de NPC, trade, creador de guild u otra con su propio cierre.
bool CanOpenSidePanel()
{
    return SceneFlag == 5 && !HasTextInput() && !HasModalDialog() && !IsItemMovePending() &&
        !(ShopOpened || WarehouseOpened || ChaosMixOpened || TradeOpened ||
          EventWindowOpened || GuildCreatorOpened || _g_bEventChipDialogEnable ||
          ServerDivisionOpened || IsQuestPanelOpen());
}

bool CanQueryChaosRate()
{
    // Chaos debe estar abierto; no comparte el bloqueo de paneles informativos.
    return ChaosMixOpened && !IsItemMovePending() && !IsMixBusy();
}

void CaptureMouseForUI() { MouseOnWindow = 1; } // MouseOnWindow

} // namespace UIState
