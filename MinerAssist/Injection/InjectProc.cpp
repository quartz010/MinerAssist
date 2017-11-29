#include "stdafx.h"
#include "InjectProc.h"

// Զ�̲����߳�
// char szDllFullPath[] DLL�ļ�����·��.
// DWORD dwRemoteProcessID Ҫ����Ľ���ID��
// ����: TRUE ������̳ɹ�
// FALSE ʧ��
BOOL InjectDll(PCWSTR szDllFullPath, DWORD dwRemoteProcessID)
{
	HANDLE hRemoteProcess;

	if (EnableDebugPriv(SE_DEBUG_NAME) == 0)
	{
		return FALSE;
	}

	// ��Զ���߳�
	if ((hRemoteProcess = OpenProcess(PROCESS_CREATE_THREAD |
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwRemoteProcessID)) ==
		NULL)
	{
		return FALSE;
	}

	PWSTR pszLibFileRemote;	
	INT cch = 1 + lstrlenW(szDllFullPath);
	INT cb = cch * sizeof(wchar_t);


	//ʹ��VirtualAllocEx������Զ�̽����ڴ��ַ�ռ����DLL�ļ���������
	pszLibFileRemote = (PWSTR)VirtualAllocEx(
		hRemoteProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	if (pszLibFileRemote == NULL)
	{
		MessageBox(GetDesktopWindow(), L"Failed 1", L"info", MB_OK);
		return FALSE;
	}
	//��Զ�̽�����д������
	if (!WriteProcessMemory(hRemoteProcess, pszLibFileRemote,
			(PVOID)szDllFullPath, cb, NULL))
	{
		MessageBox(GetDesktopWindow(), L"Failed 2", L"info", MB_OK);
		return FALSE;
	}
	
	/*LPTHREAD_START_ROUTINE  pfnStartAddr = (LPTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("user32")), "MessageBoxW");*/
	PTHREAD_START_ROUTINE  pfnStartAddr = (PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "LoadLibraryW");
	if (pfnStartAddr == NULL)
	{
		MessageBox(GetDesktopWindow(),  L"Failed 3", L"info", MB_OK);
		return FALSE;
	}

	

	// ͨ������Զ�����ӵĵ�ַ:pfnStartAddr
	// ���ݲ��� pszLibFileRemote Զ������DLL
	// ����Զ���߳� LoadLibraryW ͨ��Զ���̵߳����û���DLL�ļ�

	HANDLE hRemoteThread;
	if ((hRemoteThread =
		CreateRemoteThread(hRemoteProcess, NULL, 0, pfnStartAddr, pszLibFileRemote, 0, NULL))
		== NULL)
	{
		TCHAR errNO[16];
	
		//CreateRemoteThread(hRemoteProcess, NULL, 0, pfnStartAddr, NULL, 0, NULL);
		
		wsprintf(errNO, L"%d", GetLastError());
		MessageBox(GetDesktopWindow(), L"Failed 4", L"info", MB_OK);
		return FALSE;
	}
	return TRUE;
}

//�������Ҳ���õ���:


////////////////////////////////////////////////////////////////////////

// ��ȡ����ID��
// ���޴˽����򷵻� 0;
// char szProcName[] ������: .exe�ļ�.
DWORD GetProcID(TCHAR szProcName[])
{
	HANDLE th = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { sizeof(pe) };
	DWORD dwProcID = 0;

	BOOL bOK = Process32First(th, &pe);
	while (bOK)
	{
		bOK = Process32Next(th, &pe);

		LPCTSTR lpszExeFile = wcschr(pe.szExeFile, '//');
			if (lpszExeFile == NULL)
				lpszExeFile = pe.szExeFile;
			else
				lpszExeFile++;

		if (wcscmp(szProcName, lpszExeFile) == 0)
		{
			dwProcID = pe.th32ProcessID;
			break;
		}
	}


	return dwProcID;
}


////////////////////////////////////////////////////////////////////////

// ����ϵͳȨ�޵�DEBUGģʽ
int EnableDebugPriv(TCHAR szName[])
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	// �򿪽��̻�����
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))

	{
		return 0;
	}
	if (!LookupPrivilegeValue(NULL, szName, &luid))
	{
		return 0;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	// ����Ȩ��
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))

	{
		return 0;
	}

	return 1;
}
