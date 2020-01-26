// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"

//===============================================================
// Placeholders
//
//    Generates placeholders on the client to match what is on
//    the server.
//
// Fakery Factor:
//
//    Lotsa lyin' going on here. Since there is no cloud for this
//    sample, the Create method just walks the local folder that's
//    identified as the "cloud" and generates the placeholders along
//    with some custom states just because.
//
//===============================================================

void Placeholders::Create(
	_In_ PCWSTR destPath)
{
	try {

		auto one = LARGE_INTEGER();
		one.QuadPart = 1;

		std::wstring fileName(destPath);
		fileName.append(L"\\test.mp3");

		CF_PLACEHOLDER_CREATE_INFO cloudEntry;

		cloudEntry.FileIdentity = "abc";
		cloudEntry.FileIdentityLength = 1;

		cloudEntry.RelativeFileName = L"test.mp3";
		cloudEntry.Flags = CF_PLACEHOLDER_CREATE_FLAG_MARK_IN_SYNC;
		cloudEntry.FsMetadata.FileSize.QuadPart = 2050193;
		cloudEntry.FsMetadata.BasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
		cloudEntry.FsMetadata.BasicInfo.CreationTime = one;
		cloudEntry.FsMetadata.BasicInfo.LastWriteTime = one;
		cloudEntry.FsMetadata.BasicInfo.LastAccessTime = one;
		cloudEntry.FsMetadata.BasicInfo.ChangeTime = one;

		try
		{
			wprintf(L"Creating placeholder for test.txt\n");
			winrt::check_hresult(CfCreatePlaceholders(destPath, &cloudEntry, 1, CF_CREATE_FLAG_NONE, NULL));
		}
		catch (...)
		{
			// winrt::to_hresult() will eat the exception if it is a result of winrt::check_hresult,
			// otherwise the exception will get rethrown and this method will crash out as it should
			wprintf(L"Failed to create placeholder for test.txt with %08x\n", static_cast<HRESULT>(winrt::to_hresult()));
			// Eating it here lets other files still get a chance. Not worth crashing the sample, but
			// certainly noteworthy for production code
		}
	}
	catch (...)
	{
		wprintf(L"Could not create cloud file placeholders in the sync root with %08x\n", static_cast<HRESULT>(winrt::to_hresult()));
		// Something weird enough happened that this is worth crashing out
		throw;
	}

}