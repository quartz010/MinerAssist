#include "stdafx.h"
#include "CMineDect.h"
#include "MinerAssistDlg.h"


CMineDect::CMineDect()
{
}

CMineDect::~CMineDect()
{
}

HANDLE CMineDect::OpenProc(LPWSTR mineTitle)
{
	m_hWndMine = ::FindWindow(NULL, mineTitle);

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
	
	WCHAR info[128];
	wsprintf(info, L"[*] Get field size %d x %d", m_sizex, m_sizey);
	UpdateDbgInfo(info);
	

	return 0;
}

DWORD CMineDect::UpdateDbgInfo(LPWSTR info)
{	
	//发送消息

	//MessageBoxA(NULL, info, "none", NULL);
	return(::SendMessage(::AfxGetMainWnd()->m_hWnd, WM_MYMSG, (WPARAM)info, 0));

}

DWORD CMineDect::DectMine()
{
	
	//一个格子的数据
	BYTE mineNum = 0;
	BYTE currBlock;
	const BYTE flagBlock = 0x8E;


	//这里遍历雷区大小
	//这里由逆向分析得出 一个循环是32
	for (DWORD i = 0; i < m_sizey * 32; i += 32)
	{
		for (DWORD j = 0; j < m_sizex; j++)
		{
			::ReadProcessMemory(m_hProcMine, (LPCVOID)(m_offsetaddr + i + j), &currBlock, 1, NULL);
			
			if (currBlock == 0x8F)       //判断是否有雷
			{
				//如果有雷就画上小红旗
				mineNum++;

				::WriteProcessMemory(m_hProcMine, (LPVOID)(m_offsetaddr + i + j), &flagBlock, 1, NULL);
			}
		}
	}
	WCHAR info[128];

	wsprintf(info, L"[*] Find mine number : %d ", mineNum);
	UpdateDbgInfo(info);

	::InvalidateRect(m_hWndMine, NULL, TRUE);
	::CloseHandle(m_hProcMine);

	return 0;
}

