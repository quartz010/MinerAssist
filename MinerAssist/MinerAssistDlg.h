
// MinerAssistDlg.h: 头文件
//
#pragma once
#include "CMineDect.h"

//声明消息
#define WM_DBGMSG WM_USER+8
#define WM_ERRMSG WM_USER+9

// CMinerAssistDlg 对话框
class CMinerAssistDlg : public CDialog
{
// 构造
public:
	CMinerAssistDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINERASSIST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	CMineDect *	mineDect = new CMineDect(); ;


	//声明消息映射：
	DECLARE_MESSAGE_MAP()
public:
	
	//定义消息处理函数 用于文本框追加内容
	afx_msg LRESULT OnInfoPrint(WPARAM, LPARAM);
	afx_msg LRESULT OnErrorHandle(WPARAM, LPARAM);


	void AppendText(int controlId, CString strAdd);
	
	void ErrorHandle(DWORD errorId);

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedLaunch();
	afx_msg void OnBnClickedSweep();
	//这里是更新dbg的函数
	static DWORD UpdateDbgInfo(LPWSTR info);
};
