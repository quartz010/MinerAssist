#include "stdafx.h"
#include "CMineDect.h"


CMineDect::CMineDect()
{
}


CMineDect::~CMineDect()
{
}

HANDLE CMineDect::OpenProc()
{
	m_hWndMine = ::FindWindow(NULL, _T("扫雷"));

	DWORD procId;
	::GetWindowThreadProcessId(m_hWndMine, &procId);
	
	HANDLE Process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
	
	m_hProcMine = Process;
	return m_hProcMine;

}

BOOL CMineDect::GetWndSize()
{

	int b = 0, s = 0, nx = 0, ny = 0;

	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsetx, &m_sizex, 1, NULL);    //获取横向方格长度
	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsety, &m_sizey, 1, NULL);    //获取纵向方格长度

	TRACE("[*] Got wnd size %d x %d\n", m_sizex, m_sizey);

	return 0;
}

DWORD CMineDect::DectMine()
{
	//这里由逆向分析得出 一个循环是32
	
	WORD currBlock;
	const WORD flagBlock = 0x8E;

	for (DWORD i = 0; i < m_sizex * 32; i += 32)
	{
		for (DWORD j = 0; j < m_sizey; j++)
		{
			::ReadProcessMemory(m_hProcMine, (LPCVOID)(m_offsetaddr + i + j), &currBlock, 1, NULL);
			
			if (currBlock == 0x8F)       //判断是否有雷
			{
				//如果有雷就画上小红旗
				::WriteProcessMemory(m_hProcMine, (LPVOID)(m_offsetaddr + i + j), &flagBlock, 1, NULL);
			}
		}
	}

	::InvalidateRect(m_hWndMine, NULL, TRUE);
	::CloseHandle(m_hProcMine);

	return 0;
}

