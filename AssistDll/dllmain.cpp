// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "mineSweeper.h"

HMODULE g_hDll;
DWORD writeNum = 128;
DWORD WINAPI UnloadProc(PVOID param)
{
	
	CString info(L"我是aaaaaaaaa");
	LPWSTR info2 = L"sdfsdfsdf";

	Sleep(100);
	HANDLE hPipe = CreateFile(L"\\\\.\\Pipe\\NamedPipe", GENERIC_READ | GENERIC_WRITE, \
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Open Pipe Failed!", L"title", NULL);
		goto exit;
	}

	//ScanFiled();
	for (size_t i = 0; i < 4; i++)
	{

		WriteFile(hPipe, info.GetBuffer(16), info.GetLength(), &writeNum, NULL);

		//WriteFile(hPipe, (LPCVOID)info2, 11, &writeNum, NULL);
		Sleep(1000);
	}
	//这里清空缓冲区,上面发送有点问题
	FlushFileBuffers(hPipe);
		WriteFile(hPipe, L"exit", 4, &writeNum, NULL);

	//todo 这里hook 扫雷的api 
exit:
	//CloseHandle(hPipe); // 关闭管道句柄
	FreeLibraryAndExitThread(g_hDll, 0);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	g_hDll = hModule;
	HANDLE hThread = NULL;
	switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

		hThread = CreateThread(NULL, 0, UnloadProc, NULL, 0, NULL);
		CloseHandle(hThread);

		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

