// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"

//===============================================================
// FileCopierWithProgress
//
// Fakery Factor:
//
//   It's a fabrication to fake a connection to the internet.
//
//   This entire class is completely designed to let the sample go.
//   You will want to replace this class with one that actually knows
//   how to download stuff from your datacenter and onto the client.
//   You can take a look at the code that shows transfer progress,
//   that's kinda interesting.
//
//   This code is using the overlapped functions for ReadFileEx,
//   please see this if you are unfamiliar:
//
//      https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-readfileex
//
//===============================================================

// Since this is a local disk to local-disk copy, it would happen really fast.
// This is the size of each chunk to be copied due to the overlapped approach.
// I pulled this number out of a hat.
#define CHUNKSIZE 4096
// Arbitrary delay per chunk, again, so you can actually see the progress bar
// move
#define CHUNKDELAYMS 250

#define FIELD_SIZE( type, field ) ( sizeof( ( (type*)0 )->field ) )
#define CF_SIZE_OF_OP_PARAM( field )                                           \
    ( FIELD_OFFSET( CF_OPERATION_PARAMETERS, field ) +                         \
      FIELD_SIZE( CF_OPERATION_PARAMETERS, field ) )

struct READ_COMPLETION_CONTEXT
{
    OVERLAPPED Overlapped;
    CF_CALLBACK_INFO CallbackInfo;
    TCHAR FullPath[MAX_PATH];
    HANDLE Handle;
    CHAR PriorityHint;
    LARGE_INTEGER StartOffset;
    LARGE_INTEGER RemainingLength;
    ULONG BufferSize;
    BYTE Buffer[1];
};

// This entire class is static

void FileCopierWithProgress::CopyFromServerToClient(
    _In_ CONST CF_CALLBACK_INFO* lpCallbackInfo,
    _In_ CONST CF_CALLBACK_PARAMETERS* lpCallbackParameters)
{
    
    auto file = L"C:\\bensound-ukulele.mp3";

    auto handle = CreateFile(
        file,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        throw new std::exception("Cannot open file");
    }
    
    OVERLAPPED ol = OVERLAPPED();
    ol.Offset = lpCallbackParameters->FetchData.RequiredFileOffset.QuadPart;
    
    DWORD read = DWORD();

    std::vector<char> vec;
    vec.resize(lpCallbackParameters->FetchData.RequiredLength.QuadPart);

    if (!ReadFile(handle, vec.data(), vec.size(), nullptr, &ol)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            winrt::throw_last_error();
        }
    }
    if (!GetOverlappedResult(handle, &ol, &read, TRUE)) {
        winrt::throw_last_error();
    }

    TransferData(
        lpCallbackInfo->ConnectionKey,
        lpCallbackInfo->TransferKey,
        vec.data(),
        lpCallbackParameters->FetchData.RequiredFileOffset,
        lpCallbackParameters->FetchData.RequiredLength,
        STATUS_SUCCESS
    );

}

void FileCopierWithProgress::CancelCopyFromServerToClient(
    _In_ CONST CF_CALLBACK_INFO* lpCallbackInfo,
    _In_ CONST CF_CALLBACK_PARAMETERS* lpCallbackParameters)
{
    CancelCopyFromServerToClientWorker(lpCallbackInfo,
        lpCallbackParameters->Cancel.FetchData.FileOffset,
        lpCallbackParameters->Cancel.FetchData.Length,
        lpCallbackParameters->Cancel.Flags);
}

void FileCopierWithProgress::TransferData(
    _In_ CF_CONNECTION_KEY connectionKey,
    _In_ LARGE_INTEGER transferKey,
    _In_reads_bytes_opt_(length.QuadPart) LPCVOID transferData,
    _In_ LARGE_INTEGER startingOffset,
    _In_ LARGE_INTEGER length,
    _In_ NTSTATUS completionStatus)
{
    CF_OPERATION_INFO opInfo = { 0 };
    CF_OPERATION_PARAMETERS opParams = { 0 };

    opInfo.StructSize = sizeof(opInfo);
    opInfo.Type = CF_OPERATION_TYPE_TRANSFER_DATA;
    opInfo.ConnectionKey = connectionKey;
    opInfo.TransferKey = transferKey;
    opParams.ParamSize = CF_SIZE_OF_OP_PARAM(TransferData);
    opParams.TransferData.CompletionStatus = completionStatus;
    opParams.TransferData.Buffer = transferData;
    opParams.TransferData.Offset = startingOffset;
    opParams.TransferData.Length = length;

    auto result = CfExecute(&opInfo, &opParams);

    //Canceled by user
    if (result == -2147024498) {
        return;
    }

    winrt::check_hresult(result);
}

void FileCopierWithProgress::CancelCopyFromServerToClientWorker(
    _In_ CONST CF_CALLBACK_INFO* lpCallbackInfo,
    _In_ LARGE_INTEGER liCancelFileOffset,
    _In_ LARGE_INTEGER liCancelLength,
    _In_ CF_CALLBACK_CANCEL_FLAGS /*dwCancelFlags*/)
{
    // Yeah, a whole lotta noting happens here, because sample.
    wprintf(L"[%04x:%04x] - Cancelling read for %s%s, offset %08x`%08x length %08x`%08x\n",
        GetCurrentProcessId(),
        GetCurrentThreadId(),
        lpCallbackInfo->VolumeDosName,
        lpCallbackInfo->NormalizedPath,
        liCancelFileOffset.HighPart,
        liCancelFileOffset.LowPart,
        liCancelLength.HighPart,
        liCancelLength.LowPart);
}