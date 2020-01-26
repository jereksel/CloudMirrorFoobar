// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"

//===============================================================
// CloudProviderSyncRootWatcher
//
//   This class watches for any changes that happen to files
//   and folders in the Sync Root on the client machine. This
//   allows for hydration to be signalled or other actions.
//
// Fakery Factor:
//
//   This class is pretty usable as-is. You will probably want to
//   get rid of that whole Ctrl-C shenanigans thing to stop the
//   watcher and replace it with code that's called by some UI
//   you do to uninstall.
//
//===============================================================

bool CloudProviderSyncRootWatcher::s_shutdownWatcher;

void CloudProviderSyncRootWatcher::WatchAndWait()
{
    //  Main loop - wait for Ctrl+C or our named event to be signaled
    SetConsoleCtrlHandler(Stop, TRUE);

    while (!s_shutdownWatcher) {
        Sleep(1000);
    }

}

BOOL WINAPI
CloudProviderSyncRootWatcher::Stop(DWORD /*dwReason*/)
{
    s_shutdownWatcher = TRUE;
    return TRUE;
}

