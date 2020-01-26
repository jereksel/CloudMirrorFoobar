// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

class FileCopierWithProgress
{
public:
    static void CopyFromServerToClient(
        _In_ CONST CF_CALLBACK_INFO* callbackInfo,
        _In_ CONST CF_CALLBACK_PARAMETERS* callbackParameters);

    static void CancelCopyFromServerToClient(
        _In_ CONST CF_CALLBACK_INFO* callbackInfo,
        _In_ CONST CF_CALLBACK_PARAMETERS* callbackParameters);

private:

    static void CancelCopyFromServerToClientWorker(
        _In_ CONST CF_CALLBACK_INFO* callbackInfo,
        _In_ LARGE_INTEGER cancelFileOffset,
        _In_ LARGE_INTEGER cancelLength,
        _In_ CF_CALLBACK_CANCEL_FLAGS cancelFlags);

    static void TransferData(
        _In_ CF_CONNECTION_KEY connectionKey,
        _In_ LARGE_INTEGER transferKey,
        _In_reads_bytes_opt_(length.QuadPart) LPCVOID transferData,
        _In_ LARGE_INTEGER startingOffset,
        _In_ LARGE_INTEGER length,
        _In_ NTSTATUS completionStatus);
};

