#include "C_IPC.h"
#include "stdafx.h"
#include "CMineDect.h"
#include "MinerAssistDlg.h"


C_IPC::C_IPC(CString pipeName) :
	m_hPipe(NULL),
	IsPipeConnected(FALSE),
	IsPipeExist(FALSE)
{
	//����ȴ� IPC ����Ϣ

	//CString lpPipeName = L"\\\\.\\Pipe\\NamedPipe";
	CString lpPipeName = pipeName;

	// �����ܵ�ʵ�� ʵ����ע����̵�IPC
	m_hPipe = CreateNamedPipe(lpPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, \
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 1024, 1024, 1000, NULL);
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		DWORD dwErrorCode = GetLastError();
		wsprintf(m_dbgInfo, L"[-] Create Pipe Failed : %d", dwErrorCode);
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		return;
	}
	else
	{
		wsprintf(m_dbgInfo, L"[*] Create Pipe Successfully! waiting for IPC ");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		IsPipeExist = TRUE;
		//�������ܵ��߳�
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadIPC, hPipe, 0, NULL);
		return;
	}
}

C_IPC::C_IPC():
	m_hPipe(NULL),
	IsPipeConnected(FALSE),
	IsPipeExist(FALSE)
{
	//����ȴ� IPC ����Ϣ

	LPWSTR lpPipeName = L"\\\\.\\Pipe\\NamedPipe";

	// �����ܵ�ʵ�� ʵ����ע����̵�IPC
	m_hPipe = CreateNamedPipe(lpPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, \
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 1000, NULL);
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		DWORD dwErrorCode = GetLastError();
		wsprintf(m_dbgInfo, L"[-] Create Pipe Failed : %d", dwErrorCode);
		//CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		return;
	}
	else
	{
		wsprintf(m_dbgInfo, L"[*] Create Pipe Succeed! waiting for IPC : %x" , m_hPipe);
		//CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
		IsPipeExist = TRUE;
		
		return;
	}

	//if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) // �ȴ��ͻ���������
	//{
	//	wsprintf(m_dbgInfo, L"[!] Pipe Connect Failed!");
	//	CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

	//}
	//else
	//{	
	//	//���ӳɹ�
	//	wsprintf(m_dbgInfo, L"[+] Pipe Connect succeed!");
	//	CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);
	//	IsPipeConnected = TRUE;
	//}	

}
C_IPC::~C_IPC()
{
	//CloseHandle(m_hPipe);
}

WORD C_IPC::ReadPipe()
{
	//CheckPipe();
	WCHAR buffer[1024]; // ���ݻ���
	DWORD ReadNum;

	if (ReadFile(m_hPipe, buffer, sizeof(buffer), &ReadNum, NULL) == FALSE)
	{
		wsprintf(m_dbgInfo, L"[!] Read Pipe Exception!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		CloseHandle(m_hPipe); // �رչܵ����
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

	WriteFile(m_hPipe, m_cmd.GetString(), m_cmd.GetLength() * 2, &writeNum, NULL);

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
	WCHAR dbgInfo[1024];

	ThrdPara thrdPara;
	thrdPara.hPipe = m_hPipe;
	thrdPara.hWnd = ::AfxGetMainWnd()->m_hWnd;
	thrdPara.hWnd2 = ::AfxGetMainWnd()->m_hWnd;

	
	if (NULL == (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadThrd, &thrdPara, NULL, 0)))
	{
		wsprintf(m_dbgInfo, L"[-] Create Thread Failed!");
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		return FALSE;
	}
	else
	{
		//���ﴫ�� mainwnd �ľ�� , �������ܷ���Ϣ�� dbg
		wsprintf(m_dbgInfo, L"[+] Create Thread Succeed! 0x%x 0x%x", thrdPara.hWnd, thrdPara.hPipe);
		CMinerAssistDlg::UpdateDbgInfo(m_dbgInfo);

		Sleep(1000);
		return TRUE;
	}
}

DWORD  C_IPC::ReadThrd(ThrdPara* lpThrdPara)
{
	WCHAR dbgInfo[1024] = {0};
	WCHAR buffer[1024] = {0}; // ���ݻ���
	DWORD ReadNum;

	HANDLE hPipe = lpThrdPara->hPipe;
	HWND hWnd = lpThrdPara->hWnd;
	
	wsprintf(dbgInfo, L"[+]0x%x 0x%x", hWnd, hPipe);
	::SendMessage(hWnd, WM_MYMSG, (WPARAM)dbgInfo, 0);



	while (true)
	{
		// �ӹܵ���ȡ����
		if (ReadFile(hPipe, buffer, sizeof(buffer), &ReadNum, NULL) == FALSE)
		{
			wsprintf(dbgInfo, L"[!] Read Pipe Exception!");
			::SendMessage(hWnd, WM_MYMSG, (WPARAM)dbgInfo, 0);

			goto err_exit;
		}
		else
		{
			//buffer[ReadNum] = _T('\0');
			
			wsprintf(dbgInfo, L"[*] IPC gets : %s", buffer);
			::SendMessage(hWnd, WM_MYMSG, (WPARAM)dbgInfo, 0);

			if(0 ==  (wcscmp(buffer, L"exit")))
			{
				//�յ�exit  (Զ���˳�) �˳�

				wsprintf(dbgInfo, L"[!] Remote Exit");
				::SendMessage(hWnd, WM_MYMSG, (WPARAM)dbgInfo, 0);
				
				goto err_exit;
			}
		}
	}
	return 1;

err_exit:

	CloseHandle(hPipe); // �رչܵ����
	return -1;

}