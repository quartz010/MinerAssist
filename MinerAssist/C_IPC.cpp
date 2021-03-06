#include "C_IPC.h"
#include "stdafx.h"
#include "CMineDect.h"
#include "MinerAssistDlg.h"
#include "IPC_msgno_list.h"

C_IPC::C_IPC(CString pipeName) :
	m_hPipe(NULL),
	IsPipeConnected(FALSE),
	IsPipeExist(FALSE)
{
	
}

C_IPC::C_IPC():
	m_hPipe(NULL),
	IsPipeConnected(FALSE),
	IsPipeExist(FALSE)
{



}
C_IPC::~C_IPC()
{
	//CloseHandle(m_hPipe);
}

HANDLE C_IPC::CrearePipe()
{

	LPWSTR lpPipeName = L"\\\\.\\Pipe\\NamedPipe";

	// 创建管道实例 实现与注入进程的IPC
	m_hPipe = CreateNamedPipe(lpPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, \
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 1000, NULL);
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		DWORD dwErrorCode = GetLastError();
		wsprintf(m_dbgInfo, L"[-] Create Pipe Failed : %d", dwErrorCode);
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		return INVALID_HANDLE_VALUE;
	}
	else
	{
		wsprintf(m_dbgInfo, L"[*] Create Pipe Succeed! waiting for IPC : %x", m_hPipe);
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		IsPipeExist = TRUE;

		return m_hPipe;
	}

	//if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) // 等待客户机的连接
	//{
	//	wsprintf(m_dbgInfo, L"[!] Pipe Connect Failed!");
	//	CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

	//}
	//else
	//{	
	//	//连接成功
	//	wsprintf(m_dbgInfo, L"[+] Pipe Connect succeed!");
	//	CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
	//	IsPipeConnected = TRUE;
	//}	
}

WORD C_IPC::ReadPipe()
{
	//CheckPipe();
	WCHAR buffer[1024]; // 数据缓存
	DWORD ReadNum;

	if (ReadFile(m_hPipe, buffer, sizeof(buffer), &ReadNum, NULL) == FALSE)
	{
		wsprintf(m_dbgInfo, L"[!] Read Pipe Exception!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		CloseHandle(m_hPipe); // 关闭管道句柄
	}
	else
	{
		buffer[ReadNum] = _T('\0');
		wsprintf(m_dbgInfo, L"[*] IPC get : %s", buffer);
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

	}

	return WORD();
}

WORD C_IPC::WritePipe(LPWSTR cmd)
{
	DWORD writeNum;
	m_cmd = cmd;
	//CheckPipe();
	
	wsprintf(m_dbgInfo, L"[*] Local send : %s", m_cmd.GetString());
	CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

	WriteFile(m_hPipe, m_cmd.GetString(), m_cmd.GetAllocLength()*2, &writeNum, NULL);

	return WORD();
}

BOOL C_IPC::CheckPipe()
{
	if (IsPipeExist == FALSE)
	{
		wsprintf(m_dbgInfo, L"[!] Pipe Not Exist yet!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		return FALSE;
	}
	if (IsPipeConnected == FALSE)
	{
		wsprintf(m_dbgInfo, L"[!] Pipe Not Connected yet!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		return FALSE;
	}
	return TRUE;
}

BOOL C_IPC::StartReadThrd()
{
 	CrearePipe();

	ThrdPara thrdPara;
	thrdPara.hPipe = m_hPipe;
	thrdPara.hWnd = ::AfxGetMainWnd()->m_hWnd;
	thrdPara.hWnd2 = ::AfxGetMainWnd()->m_hWnd;

	IsPipeExist = TRUE;
	
	Sleep(500); //等会再read
	if (NULL == (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadThrd, &thrdPara, NULL, 0)))
	{
		wsprintf(m_dbgInfo, L"[-] Create Thread Failed!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		return FALSE;
	}
	else
	{
		//这里传入 mainwnd 的句柄 , 这样才能发消息给 dbg
		wsprintf(m_dbgInfo, L"[+] Start Listening Thread");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		Sleep(100);
		return TRUE;
	}
}

DWORD  C_IPC::ReadThrd(ThrdPara* lpThrdPara)
{
	WCHAR dbgInfo[128] = {0};

	WCHAR szBuffer[128] = {0}; // 数据缓存
	WCHAR exitStr[16] = { L"exit" };
	DWORD ReadNum;

	HANDLE hPipe = lpThrdPara->hPipe;
	HWND hWnd = lpThrdPara->hWnd;
	
	//wsprintf(dbgInfo, L"[+]0x%x 0x%x", hWnd, hPipe);
	//::SendMessage(hWnd, WM_DBGMSG, (WPARAM)dbgInfo, 0);



	while (true)
	{
		// 从管道读取数据
		if (ReadFile(hPipe, szBuffer, sizeof(szBuffer), &ReadNum, NULL) == FALSE)
		{
			wsprintf(dbgInfo, L"[!] Read Pipe Exception! : %d, EXIT" , GetLastError());
			::SendMessage(hWnd, WM_DBGMSG, (WPARAM)dbgInfo, 0);

			goto err_exit;
		}
		else if(ReadNum != 0)
		{
			
			wsprintf(dbgInfo, L"[*] IPC gets : %s , %d", szBuffer, ReadNum);
			//给dbg窗口更新
			::SendMessage(hWnd, WM_DBGMSG, (WPARAM)dbgInfo, 0);
			
			
			//这里传入的字符比"exit"长, 就espxx了
			if( 0 ==  (wcscmp(exitStr, szBuffer)))
			{
				//收到exit  (远程退出) 退出
				wsprintf(dbgInfo, L"[!] Remote Exit");
				::SendMessage(hWnd, WM_DBGMSG, (WPARAM)dbgInfo, 0);

				goto err_exit;
			}
			memset(szBuffer, 0, 128);
		}
	}
	return 1;

err_exit:
	
	CloseHandle(hPipe); // 关闭管道句柄
	::SendMessage(hWnd, WM_ERRMSG, (WPARAM)EM_REXIT, 0);
	return -1;

}