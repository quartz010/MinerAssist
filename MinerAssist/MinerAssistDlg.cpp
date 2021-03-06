
// MinerAssistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MinerAssist.h"
#include "MinerAssistDlg.h"
#include "afxdialogex.h"

#include "CMineDect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CMinerAssistDlg 对话框

CMinerAssistDlg::CMinerAssistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MINERASSIST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMinerAssistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
//自定义消息


//这里 建立消息 和其服务函数的映射关系
BEGIN_MESSAGE_MAP(CMinerAssistDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMinerAssistDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMinerAssistDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SCAN, &CMinerAssistDlg::OnBnClickedScan)

//这里自定义消息
	ON_MESSAGE(WM_ERRMSG, &CMinerAssistDlg::OnErrorHandle)
	ON_MESSAGE(WM_DBGMSG, &CMinerAssistDlg::OnInfoPrint)
	ON_BN_CLICKED(IDC_LAUNCH, &CMinerAssistDlg::OnBnClickedLaunch)
	ON_BN_CLICKED(IDC_Sweep, &CMinerAssistDlg::OnBnClickedSweep)
END_MESSAGE_MAP()


// CMinerAssistDlg 消息处理程序

BOOL CMinerAssistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//这里禁止Sweep按钮
	(GetDlgItem(IDC_Sweep))->EnableWindow(FALSE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMinerAssistDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//这里判断 syscmd 的来源, 给about ,还是给 main 
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

//  如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMinerAssistDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMinerAssistDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMinerAssistDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void CMinerAssistDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}

/////////////////////////////////////////////
void CMinerAssistDlg::OnBnClickedScan()
{
	
	
	if (NULL != mineDect->OpenProc(_T("扫雷")))
	{
		AppendText(IDC_EDIT1, _T("[+] Get proc Handle Succeed "));
	
		if (mineDect->InjectModule())
		{
			//成功注入进程 ,避免再次操作
			(GetDlgItem(IDC_SCAN))->EnableWindow(FALSE);
		}



		//使能注入按钮

	}
	else
	{
		//失能注入按钮
//		(GetDlgItem(IDC_Sweep))->EnableWindow(FALSE);

		//AppendText(IDC_EDIT1, _T("[-] Get proc Handle Failed "));
		//AfxMessageBox(_T("not found winmine!"));
	}

	// TODO: 在此添加控件通知处理程序代码
}





void CMinerAssistDlg::OnBnClickedLaunch()
{
	//AppendText(IDC_EDIT1, _T("[-] Not Finished !"));
	
	mineDect->m_ipc->WritePipe(L"ScanFiled");

	// TODO: 在此添加控件通知处理程序代码
}


void CMinerAssistDlg::OnBnClickedSweep()
{
	//static BOOL bInjected = FALSE;
	//if (!bInjected)
	//{
	//	mineDect->SweepMine();
	//	((CButton*)GetDlgItem(IDC_Sweep))->SetWindowTextW(L"UnInject");
	//}
	//bInjected = !bInjected;
	//点击之后失能 , 只能扫雷一次
	(GetDlgItem(IDC_Sweep))->EnableWindow(FALSE);

	// TODO: 在此添加控件通知处理程序代码
}

