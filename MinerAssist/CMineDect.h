#pragma once
class CMineDect
{
public:
	CMineDect();
	~CMineDect();

	
	
	
	
	HANDLE OpenProc(LPWSTR mineTitle);
	BOOL GetWndSize();
	DWORD DectMine();

private:
	
	DWORD UpdateDbgInfo(LPWSTR info);
	//DWORD UpdateDbgInfo(const WCHAR * fmt, ...);
	
	
	HANDLE m_hProcMine;
	HWND m_hWndMine;

	DWORD m_sizey;
	DWORD m_sizex;

	DWORD m_offsetaddr = 0x1005361;
	DWORD m_offsety = 0x10056A8;
	DWORD m_offsetx = 0x10056AC;



};

