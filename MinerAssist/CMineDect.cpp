#include "stdafx.h"
#include "CMineDect.h"
#include "MinerAssistDlg.h"
#include "Injection/InjectProc.h"

CMineDect::CMineDect()
{
}

CMineDect::~CMineDect()
{
}

HANDLE CMineDect::OpenProc(LPWSTR mineTitle)

{
	DWORD procId;
	WCHAR info[128];
	
	m_hProcMine = NULL;

	if (NULL != (m_hWndMine = ::FindWindow(NULL, mineTitle)))
	{
		::GetWindowThreadProcessId(m_hWndMine, &procId);
		
		wsprintf(info, L"[*] Find mine Proc PID : %d", procId);
		UpdateDbgInfo(info);
		
		HANDLE Process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
		m_hProcMine = Process;
	}
	else
	{
		wsprintf(info, L"[-] Find mine Proc Failed (IS THERE A WINMINE??)");
		UpdateDbgInfo(info);
	}
	
	return m_hProcMine;

}

BOOL CMineDect::GetWndSize()
{

	int b = 0, s = 0, nx = 0, ny = 0;

	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsetx, &m_sizex, 1, NULL);    //获取横向方格长度
	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsety, &m_sizey, 1, NULL);    //获取纵向方格长度


	TRACE("[*] Got wnd size %d x %d\n", m_sizex, m_sizey);
	
	WCHAR info[128];
	//wsprintf(info, L"[*] Get field size %x x %x", m_offsetx, m_offsety);
	
	wsprintf(info, L"[*] Get field size %d x %d ", m_sizex, m_sizey);
	UpdateDbgInfo(info);

	//检测雷区大小是否合理(愚蠢的办法)
	if (m_sizex < 0 || m_sizey < 0 || m_sizex > 100 || m_sizey >100)
	{
		
		wsprintf(info, L"[-] Get filed size Failed ");
		UpdateDbgInfo(info);
		return FALSE;
	}

	return TRUE;
}

DWORD CMineDect::UpdateDbgInfo(LPWSTR info)
{	
	//发送消息

	//MessageBoxA(NULL, info, "none", NULL);
	return(::SendMessage(::AfxGetMainWnd()->m_hWnd, WM_MYMSG, (WPARAM)info, 0));

}
//查雷, 立旗子
DWORD CMineDect::SetFlag()
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

	return mineNum;
}
//排雷,
DWORD CMineDect::SweepMine()
{
	//for dbg
	LPWSTR szBuffer = _T("G:\\Documents\\Visual Studio 2017\\Projects\\MinerAssist\\Debug\\AssistDll.dll");

	DWORD procId;
	//这里获取 扫雷 进程的pid
	::GetWindowThreadProcessId(m_hWndMine, &procId);

	if (!InjectDll(szBuffer, procId))
	{
		WCHAR info[128];
		errno = GetLastError();
		wsprintf(info, L"[-] Inject mine Proc Failed : %d", errno);
		UpdateDbgInfo(info);
	}
	else
	{
		WCHAR info[128];
		errno = GetLastError();
		wsprintf(info, L"[+] Inject mine Proc Successfully : %d", errno);
		UpdateDbgInfo(info);
	}
	return 0;

}

