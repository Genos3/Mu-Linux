// Process_Exit.cpp

#include "stdafx.h"
#include "Platform/IWindow.h"
#include "globals.h"
#include "functions.h"

// ExitProgram @ 0x004F6CB0 — ExitProgram
// Shows the fatal error message at ExitErrorMsgBuf then destroys the main window.
void __cdecl ExitProgram(void)
{
    Platform::GetWindow().ShowError(ExitErrorMsgBuf, NULL, true, false);
    gWindow.RequestCloseApplication();  // WM_DESTROY
}

