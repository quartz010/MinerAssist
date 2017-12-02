#pragma once
#include "C_IPC.h"

class CMineDect
{
public:
	CMineDect();
	~CMineDect();

	
	
	
	
	HANDLE OpenProc(LPWSTR mineTitle);
	BOOL GetWndSize();
	//DWORD DectMine();
	//²éÀ×
	DWORD SetFlag();
	//ÅÅÀ×
	DWORD SweepMine();

	DWORD UnInject();


	
	C_IPC *m_ipc;
	//static DWORD UpdateDbgInfo(LPWSTR info);

private:

	static DWORD ThreadIPC(LPVOID lpParameter);
	//DWORD UpdateDbgInfo(const WCHAR * fmt, ...);


	HANDLE m_hProcMine;
	HWND m_hWndMine;

	DWORD m_sizey;
	DWORD m_sizex;

	DWORD m_offsetaddr = 0x1005361;
	DWORD m_offsety = 0x10056A8;
	DWORD m_offsetx = 0x10056AC;



};

