// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
HMODULE g_hDll;

DWORD WINAPI UnloadProc(PVOID param)
{
	MessageBox(NULL, TEXT("Press ok to unload me."),
		TEXT("MsgBox in dll"), MB_OK);
	Sleep(1000);

	//todo 这里hook 扫雷的api 
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

