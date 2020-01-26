// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"

//===============================================================
// ProviderFolderLocations
//
//   Manages the locations of the folders where the syncroot
//   and "cloud" live.
//
// Fakery Factor:
//
//   You will likely rewrite all of this. But, look on the bright
//   side: This is a tiny class that does barely anything.
//
//===============================================================

std::wstring ProviderFolderLocations::s_clientFolder;

bool ProviderFolderLocations::Init(_In_opt_ LPCWSTR clientFolder)
{

    s_clientFolder = PromptForFolderPath(L"\"Syncroot (Client)\" Location");

    auto result{ false };
    if (!s_clientFolder.empty())
    {
        // In case they were passed in params we may need to create the folder.
        // If the folder is already there then these are benign calls.
        CreateDirectory(s_clientFolder.c_str(), NULL);
        result = true;
    }
    return result;
}

std::wstring ProviderFolderLocations::PromptForFolderPath(_In_ PCWSTR title)
{
    winrt::com_ptr<IFileOpenDialog> fileOpen;
    winrt::check_hresult(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, __uuidof(&fileOpen), fileOpen.put_void()));
    winrt::check_hresult(fileOpen->SetOptions(FOS_PICKFOLDERS));
    winrt::check_hresult(fileOpen->SetTitle(title));

    try
    {
        winrt::check_hresult(fileOpen->Show(nullptr));
    }
    catch (winrt::hresult_canceled)
    {
        return std::wstring();
    }

    winrt::com_ptr<IShellItem> item;
    winrt::check_hresult(fileOpen->GetResult(item.put()));

    winrt::com_array<wchar_t> path;
    winrt::check_hresult(item->GetDisplayName(SIGDN_FILESYSPATH, winrt::put_abi(path)));

    return std::wstring(path.data());
}
