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

		//下面这里很关键 , 针对dll 卸载, 或者软件关闭时候通知 服务端
	case DLL_THREAD_DETACH:
		CloseHandle(hThread);
		if (g_hPipe != INVALID_HANDLE_VALUE)
		{
			//这里16 设置投机取巧 够大 ,抹掉缓冲区
			WriteFile(g_hPipe, L"exit", 16, NULL, NULL);
			CloseHandle(g_hPipe); // 关闭管道句柄
		}
		break;

	case DLL_PROCESS_DETACH:
		CloseHandle(hThread);
		if (g_hPipe != INVALID_HANDLE_VALUE)
		{
			//这里16 设置投机取巧 够大 ,抹掉缓冲区
			WriteFile(g_hPipe, L"exit", 16, NULL, NULL);
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




//用map吧

typedef DWORD(*LPMYFUNC)(INT n, INT b, INT c);

//这里建立 函数 命令字典!!!
//c++ 11 独有的
std::map <LPWSTR, LPMYFUNC> funcMap = {
	{L"AutoTAP", NULL},
	{L"StopTime", NULL},
	{L"Kill", NULL}
};
//这里定义 字典的迭代器
std::map<LPWSTR, LPMYFUNC>::iterator it;

BOOL ExeCmd(LPWSTR fnName)
{
	//这里是关于迭代器的使用 //cpp 真tm 难用 都不及时补充基本类函数????
	for (it = funcMap.begin(); it != funcMap.end(); it++)
	{
		LPWSTR tmp = it->first;
		if (!wcscmp(tmp, fnName))		//这里迭代判断指令
		{
			return(it->second)(0, 0, 0);		//调用函数
		}
	}
	return FALSE;
}