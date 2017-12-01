#pragma once
#include "stdafx.h"
struct ThrdPara
{
	HWND hWnd;
	HWND hWnd2;
	HANDLE hPipe;
};

class C_IPC
{
public:
	C_IPC();
	C_IPC(CString pipeName);

	~C_IPC();


	WORD ReadPipe();
	WORD WritePipe();

	BOOL StartReadThrd();


private:

	HANDLE m_hPipe;
	WCHAR m_dbgInfo[128];

	BOOL IsPipeConnected = FALSE;
	BOOL IsPipeExist = FALSE;
	
	BOOL CheckPipe();
	
	static DWORD ReadThrd(ThrdPara* lpThrdPara);
	
	


};



