#include "stdafx.h"
#include "IPC.h"

extern HANDLE g_hPipe;
extern HMODULE g_hDll;



DWORD WritePipe(LPCWSTR dbgInfo)
{
	WCHAR message[128];
	DWORD writeNum = 128;
	//CheckPipe();

	wsprintf(message, L"[*] Remote send : %s", dbgInfo);

	WriteFile(g_hPipe, message, wcslen(message)* 2, &writeNum, NULL);

	return writeNum;
}

DWORD DbgInfoPrint(LPWSTR dbgInfo)
{
	return WritePipe(dbgInfo);
}

HANDLE OpenPipe()
{
	g_hPipe = CreateFile(L"\\\\.\\Pipe\\NamedPipe", GENERIC_READ | GENERIC_WRITE, \
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hPipe == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Open Pipe Failed!", L"title", NULL);
		RemotExit();
	}
	WriteFile(g_hPipe, L"Connect Established!", 32, NULL, NULL);

	return g_hPipe;
}


DWORD WINAPI reciveCmd(PVOID param)
{
	CString info;
	DWORD writeNum = 128;

	g_hPipe = OpenPipe();
	//ScanFiled();

	for (size_t i = 0; i < 4; i++)
	{
		//Unicode 字节数问题,烦死了!!!
		info = L"HELLO";
		//WriteFile(g_hPipe, info.GetString(), 64, &writeNum, NULL);
		DbgInfoPrint(info.GetBuffer());
		Sleep(500);
	}

	//这里清空缓冲区,上面发送有点问题

	FlushFileBuffers(g_hPipe); //然而好像清不掉


	DWORD  nReadByte = 0, nWriteByte = 0, dwByte = 0;
	WCHAR  szBuf[64];
	DWORD ReadNum = 64;

	// 从管道读取数据
	if (ReadFile(g_hPipe, szBuf, sizeof(szBuf), &ReadNum, NULL) == FALSE)
	{
		errno = GetLastError();
		MessageBox(NULL, L"[!] Read Pipe Failed, exiting... ", L"Dll", NULL);
		RemotExit();
	}
	else
	{
		Sleep(1000);
		info = L"GOT!";
		WriteFile(g_hPipe, info.GetString(), info.GetLength() * 2, &writeNum, NULL);

	}
	return 0;

}


