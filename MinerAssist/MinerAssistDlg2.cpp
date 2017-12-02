#include "stdafx.h"
#include "MinerAssist.h"
#include "MinerAssistDlg.h"
#include "afxdialogex.h"

#include "CMineDect.h"
#include "IPC/IPC_msgno_list.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//实现消息处理函数（当然可以不实现）
LRESULT CMinerAssistDlg::OnInfoPrint(WPARAM w, LPARAM l)
{

	AppendText(IDC_EDIT1, (LPWSTR)w);

	//AfxMessageBox(_T("Hello,World!"));
	return 0;
}

// 追加文本到EditControl
void CMinerAssistDlg::AppendText(int controlId, CString strAdd)
{
	//先是调整当前光标到文本末端
	((CEdit*)GetDlgItem(controlId))->SetSel(GetDlgItem(controlId)->GetWindowTextLength(), GetDlgItem(controlId)->GetWindowTextLength());
	//再者替换文本
	((CEdit*)GetDlgItem(controlId))->ReplaceSel(strAdd + L"\r\n");
}


LRESULT CMinerAssistDlg::OnErrorHandle(WPARAM w, LPARAM l)
{
	ErrorHandle((DWORD)w);
	return LRESULT();
}

void CMinerAssistDlg::ErrorHandle(DWORD errorId)
{
	switch (errorId)
	{
	case EM_REXIT:
		(GetDlgItem(IDC_SCAN))->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}

DWORD CMinerAssistDlg::UpdateDbgInfo(LPWSTR info)
{
	//MessageBoxA(NULL, info, "none", NULL);
	return(::SendMessage(::AfxGetMainWnd()->m_hWnd, WM_DBGMSG, (WPARAM)info, 0));
}
