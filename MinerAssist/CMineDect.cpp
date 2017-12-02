#include "stdafx.h"
#include "CMineDect.h"
#include "MinerAssistDlg.h"
#include "Injection/InjectProc.h"
#include "C_IPC.h"

CMineDect::CMineDect()
{
	m_ipc = new C_IPC;
}

CMineDect::~CMineDect()
{
}

// reserve
HANDLE CMineDect::OpenProc(LPWSTR mineTitle)
{
	DWORD procId;
	WCHAR info[128];

	m_hProcMine = NULL;

	//��ɨ�״���
	if (NULL != (m_hWndMine = ::FindWindow(NULL, mineTitle)))
	{
		//���ݴ��ھ�������߳�(����)
		::GetWindowThreadProcessId(m_hWndMine, &procId);

		wsprintf(info, L"[*] Find mine Proc PID : %d", procId);
		CMinerAssistDlg::UpdateDbgInfo(info);

		//�򿪽��̾��
		HANDLE Process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
		m_hProcMine = Process;
	}
	else
	{
		wsprintf(info, L"[-] Find mine Proc Failed (IS THERE REALLY A WINMINE??)");
		CMinerAssistDlg::UpdateDbgInfo(info);
	}

	return m_hProcMine;
}

// reserve
BOOL CMineDect::GetWndSize()
{
	//���ڴ�ƫ��
	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsetx, &m_sizex, 1, NULL);    //��ȡ���򷽸񳤶�
	::ReadProcessMemory(m_hProcMine, (LPCVOID)m_offsety, &m_sizey, 1, NULL);    //��ȡ���򷽸񳤶�

	TRACE("[*] Got wnd size %d x %d\n", m_sizex, m_sizey);

	WCHAR info[128];
	//wsprintf(info, L"[*] Get field size %x x %x", m_offsetx, m_offsety);

	wsprintf(info, L"[*] Get field size %d x %d ", m_sizex, m_sizey);
	CMinerAssistDlg::UpdateDbgInfo(info);

	//���������С�Ƿ����(�޴��İ취)
	if (m_sizex < 0 || m_sizey < 0 || m_sizex > 100 || m_sizey >100)
	{
		wsprintf(info, L"[-] Get filed size Failed ");
		CMinerAssistDlg::UpdateDbgInfo(info);
		return FALSE;
	}

	return TRUE;
}


//����, ������ RESERVE
DWORD CMineDect::SetFlag()
{
	//һ�����ӵ�����
	BYTE mineNum = 0;
	BYTE currBlock;
	const BYTE flagBlock = 0x8E;

	//�������������С
	//��������������ó� һ��ѭ����32
	for (DWORD i = 0; i < m_sizey * 32; i += 32)
	{
		for (DWORD j = 0; j < m_sizex; j++)
		{
			::ReadProcessMemory(m_hProcMine, (LPCVOID)(m_offsetaddr + i + j), &currBlock, 1, NULL);

			if (currBlock == 0x8F)       //�ж��Ƿ�����
			{
				//������׾ͻ���С����
				mineNum++;

				::WriteProcessMemory(m_hProcMine, (LPVOID)(m_offsetaddr + i + j), &flagBlock, 1, NULL);
			}
		}
	}
	WCHAR info[128];
	if (mineNum > 0)
	{
		wsprintf(info, L"[*] Find mine number : %d ", mineNum);
	}
	else
	{
		wsprintf(info, L"[!] No mine Remained  ");
	}
	CMinerAssistDlg::UpdateDbgInfo(info);

	::InvalidateRect(m_hWndMine, NULL, TRUE);
	::CloseHandle(m_hProcMine);

	return mineNum;
}

//����,RESERVE
DWORD CMineDect::SweepMine()
{
	//for dbg
	LPWSTR szBuffer = _T("G:\\Documents\\Visual Studio 2017\\Projects\\MinerAssist\\Debug\\AssistDll.dll");

	DWORD procId;
	//�����ȡ ɨ�� ���̵�pid
	::GetWindowThreadProcessId(m_hWndMine, &procId);

	if (!InjectDll(szBuffer, procId))
	{
		WCHAR info[128];
		errno = GetLastError();
		wsprintf(info, L"[-] Inject mine Proc Failed : %d", errno);
		CMinerAssistDlg::UpdateDbgInfo(info);
	}
	else
	{
		WCHAR info[128];
		wsprintf(info, L"[+] Inject mine Proc Successfully ");
		CMinerAssistDlg::UpdateDbgInfo(info);
		
		////////////////////////TODO

		m_ipc->StartReadThrd();
	}
	return 0;
}


// reserve
DWORD CMineDect::ThreadIPC(LPVOID lpParameter)
{
	WCHAR info[128];
	DWORD  nReadByte = 0, nWriteByte = 0, dwByte = 0;
	WCHAR  szBuf[64];
	DWORD ReadNum = 64;

	HANDLE hPipe = (HANDLE)lpParameter;

	wsprintf(info, L"[+] Create Listening Thread Successfully");
	CMinerAssistDlg::UpdateDbgInfo(info);

	//if (ConnectNamedPipe(hPipe, NULL) == FALSE) // �ȴ��ͻ���������
	//{
	//	errno = GetLastError();
	//	wsprintf(info, L"[!] Connect Pipe Failed, exiting... : %d", errno);
	//	UpdateDbgInfo(info);
	//	goto exit;
	//}
	//else
	//{
	//	wsprintf(info, L"[+] Connected!");
	//	UpdateDbgInfo(info);
	//}
	
	// �ӹܵ���ȡ����
	if (ReadFile(hPipe, szBuf, sizeof(szBuf), &ReadNum, NULL) == FALSE)
	{
		errno = GetLastError();
		wsprintf(info, L"[!] Read Pipe Failed, exiting... : %d", errno);
		CMinerAssistDlg::UpdateDbgInfo(info);
		goto exit;
	}
	else
	{
		szBuf[ReadNum] = _T('\0'); // ��ʾ���յ�����Ϣ
		
		wsprintf(info, L"[*] From IPC : %s , %d", szBuf, ReadNum);
		CMinerAssistDlg::UpdateDbgInfo(info);
	}
	
exit:
	CloseHandle(hPipe); // �رչܵ����
	return 0;
}



// reserve
DWORD CMineDect::UnInject()
{
	DWORD procId;
	//�����ȡ ɨ�� ���̵�pid
	::GetWindowThreadProcessId(m_hWndMine, &procId);

	if (!UnloadDll(procId))
	{
		WCHAR info[128];
		errno = GetLastError();
		wsprintf(info, L"[-] Unload module Proc Failed : %d", errno);
		CMinerAssistDlg::UpdateDbgInfo(info);
	}
	else
	{
		WCHAR info[128];
		errno = GetLastError();
		wsprintf(info, L"[+] Unload module Proc Successfully : %d", errno);
		CMinerAssistDlg::UpdateDbgInfo(info);
	}
	return 0;
}

