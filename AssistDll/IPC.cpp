#include "stdafx.h"
#include "IPC.h"
#include "AssistFunc.h"

extern HANDLE g_hPipe;
extern HMODULE g_hDll;



DWORD WritePipe(LPCWSTR dbgInfo)
{
	CString message = dbgInfo;
	DWORD writeNum = 0;
	//CheckPipe();

	//wsprintf(message, L"[*] Remote send : %s", dbgInfo);

	WriteFile(g_hPipe, message.GetString(), message.GetAllocLength() * 2, &writeNum, NULL);

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
		MessageBox(NULL, L"Open Pipe Failed! EXIT!", L"title", NULL);
		RemotExit();
	}
	
	DbgInfoPrint(L"Connect Established!");
	return g_hPipe;
}


DWORD WINAPI reciveCmd(PVOID param)
{
	DWORD writeNum = 128;
	WCHAR szBuf[128] = { 0 };

	g_hPipe = OpenPipe();

	//这里清空缓冲区,上面发送有点问题


	DWORD ReadNum = 0;
	while (true)
	{

		// 从管道读取数据
		if (ReadFile(g_hPipe, szBuf, sizeof(szBuf), &ReadNum, NULL) == FALSE)
		{
			errno = GetLastError();
			wsprintf(szBuf, L"[!] Read Pipe Failed, exiting...  : %d", errno);

			MessageBox(NULL, szBuf, L"Dll", NULL);
			RemotExit();
		}
		else if(ReadNum != 0)
		{			

			//wsprintf(info.GetBuffer(), L"GOT : %s", szBuf);
			ExeCmd(szBuf);

			CString info = L"[*] executed";
			DbgInfoPrint(info.GetBuffer());
			//清空缓冲区
			memset(szBuf, 0, 128);
		}
	}
	return 0;

}


