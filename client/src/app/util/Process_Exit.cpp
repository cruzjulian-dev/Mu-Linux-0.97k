// Process_Exit.cpp

#include "stdafx.h"
#include "platform/IWindow.h"
#include "globals.h"
#include "functions.h"

// ExitProgram @ 0x004F6CB0 — ExitProgram
// Shows the fatal error message at GlobalText[11] then destroys the main window.
void __cdecl ExitProgram(void)
{
    Platform::GetWindow().ShowError(GlobalText[11], NULL, true, false);
    gWindow.RequestCloseApplication();  // WM_DESTROY
}

