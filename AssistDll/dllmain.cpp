// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "AssistFunc.h"
#include "IPC.h"

#include <string>
#include <map>
///////////////////////////////////
HANDLE OpenPipe();
BOOL ExeCmd(LPWSTR fnName);
VOID RemotExit();



///////////////////////////////////

HMODULE g_hDll;
HANDLE g_hPipe;


///////////////////////////////////
VOID test()
{
	CString info;

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

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	g_hDll = hModule;
	HANDLE hThread = NULL;
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		hThread = CreateThread(NULL, 0, reciveCmd, NULL, 0, NULL);

		break;
	case DLL_THREAD_ATTACH:
		break;
	
		//下面这里很关键 , 针对dll 卸载, 或者软件关闭时候通知 服务端
	case DLL_THREAD_DETACH:
		//CloseHandle(hThread);
		break;

	case DLL_PROCESS_DETACH:
		//CloseHandle(hThread);
		//MessageBox(NULL, L"A", NULL, NULL);
		if (g_hPipe != INVALID_HANDLE_VALUE)
		{
			//这里16 设置投机取巧 够大 ,抹掉缓冲区

//			DbgInfoPrint(L"exit");
			CloseHandle(g_hPipe); // 关闭管道句柄
		}
		break;
	}
	return TRUE;
}


VOID RemotExit()
{
	FreeLibraryAndExitThread(g_hDll, 0);
	return;
}



