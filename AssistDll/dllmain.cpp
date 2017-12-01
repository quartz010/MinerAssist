// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "mineSweeper.h"
#include <string>
HMODULE g_hDll;
HANDLE g_hPipe;

DWORD writeNum = 128;

DWORD WINAPI UnloadProc(PVOID param)
{
	std::string Message = "[测试数据,由客户机发出]"; // 要发送的数据
	CString info(L"aaffvvf");
	WCHAR info2[128] = L"sdfsdfsdf";

	Sleep(100);
	 g_hPipe = CreateFile(L"\\\\.\\Pipe\\NamedPipe", GENERIC_READ | GENERIC_WRITE, \
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hPipe == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Open Pipe Failed!", L"title", NULL);
		goto exit;
	}

	//ScanFiled();
	for (size_t i = 0; i < 4; i++)
	{

		//WriteFile(g_hPipe, info2, wcslen(info2), &writeNum, NULL);
		//WriteFile(g_hPipe, Message.c_str(), Message.length(), &writeNum, NULL);
		//Unicode 字节数问题,烦死了!!!

		WriteFile(g_hPipe, info.GetString(), info.GetLength()*2, &writeNum, NULL);
		Sleep(1000);
	}
	//这里清空缓冲区,上面发送有点问题
	
	FlushFileBuffers(g_hPipe); //然而好像清不掉

	//这里16 设置投机取巧 够大 ,抹掉缓冲区

	//todo 这里hook 扫雷的api 
exit:



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
	
		WriteFile(g_hPipe, L"exit", 16, &writeNum, NULL);
		CloseHandle(g_hPipe); // 关闭管道句柄
	
		break;
    case DLL_PROCESS_DETACH:
	
		WriteFile(g_hPipe, L"exit", 16, &writeNum, NULL);
		CloseHandle(g_hPipe); // 关闭管道句柄
        break;
    }
    return TRUE;
}

