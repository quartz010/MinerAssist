#include "stdafx.h"
#include "AssistFunc.h"
#include "IPC.h"
#include <map>

CONST LPWORD offsetFnClick = (LPWORD)/*0x10037E1*/0x01003512;        //选方格的函数地址
CONST LPBYTE offsetFiledaddr = (LPBYTE)0x1005361;
CONST LPWORD offsetSizeY = (LPWORD)0x10056A8;
CONST LPWORD offsetSizeX = (LPWORD)0x10056AC;
//  定义扫雷信息类
class CmineInfo
{
public:
	CmineInfo();
	~CmineInfo();


	WORD m_size_x;
	WORD m_size_y;
	WORD m_mineNum;

private:

};

CmineInfo::CmineInfo()
{
}

CmineInfo::~CmineInfo()
{
}

//实例化
CmineInfo minfo;

BOOL Click(DWORD x, DWORD y)
{
	_asm {
		//这里压入参数
		push x       
		push y       
		call offsetFnClick  //调用选方格函数
	}
	return 0;
}

DWORD ScanFiled(INT a, INT b)
{
	BYTE currBlock;
	WCHAR info[64];
	wsprintf(info, L"%d x %d", *offsetSizeX, *offsetSizeY);
//	MessageBox(NULL, info, L"Title", NULL);
//	for (DWORD i = 0; i < (*offsetSizeY); i ++) 为什么这两句效果不一样???写晕了

	for (WORD i = 0; i < (*offsetSizeY) * 32; i += 32)
	{
		for (WORD j = 0; j < (*offsetSizeX); j++)
		{
			currBlock = *(offsetFiledaddr + i + j);

			if (currBlock != 0x8E && currBlock != 0x8F)       //判断是否有旗子 
			{												//有雷有旗子 = 8E
				//不要太快对吧 这样才有优化空间
				Sleep(10);
				Click(i/32 + 1 , j+1);
			}
		}
	}
	return 0;
}

// reserve
BOOL GetWndSize()
{
	
	WORD size_y = (*offsetSizeY);
	WORD size_x = (*offsetSizeX);
	
	WCHAR info[128];
	//wsprintf(info, L"[*] Get field size %x x %x", m_offsetx, m_offsety);

	minfo.m_size_x = size_x;
	minfo.m_size_y = size_y;

	wsprintf(info, L"[*] Get field size %d x %d ", size_x, size_y);
	
	DbgInfoPrint(info);

	//检测雷区大小是否合理(愚蠢的办法)
	if (size_x < 0 || size_y < 0 || size_x > 100 || size_y >100)
	{
		wsprintf(info, L"[-] illegal size ");
		
		return FALSE;
	}

	return TRUE;
}


//查雷, 立旗子 
DWORD SetFlag(INT a, INT b)
{
	//一个格子的数据
	BYTE mineNum = 0;
	BYTE currBlock;
	const BYTE flagBlock = 0x8E;

	GetWndSize();

	//这里遍历雷区大小
	//这里由逆向分析得出 一个循环是32
	for (WORD i = 0; i < minfo.m_size_y * 32; i += 32)
	{
		for (WORD j = 0; j < minfo.m_size_x; j++)
		{
			currBlock = *(offsetFiledaddr + i + j);

			if (currBlock == 0x8F)       //判断是否有雷
			{
				//如果有雷就画上小红旗
				mineNum++;

				*(offsetFiledaddr + i + j) = flagBlock;
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

	DbgInfoPrint(info);

	return mineNum;
}

DWORD test(INT a, INT b)
{
	MessageBox(NULL, L"OKOKOKOKO", NULL, MB_OK);
	return 0;
}


//用map吧

typedef DWORD(*LPMYFUNC)(INT wparam, INT lparam);

//这里建立 函数 命令字典!!!
//c++ 11 独有的

std::map <LPWSTR, LPMYFUNC> funcMap = {
	{ L"AutoTAP", NULL },
	{ L"StopTime", NULL },
	{ L"Kill", NULL },
	{ L"HI", test},
	{ L"ScanFiled", ScanFiled },
	{ L"SetFlag", SetFlag }
};
//这里定义 字典的迭代器

std::map<LPWSTR, LPMYFUNC>::iterator it;

BOOL ExeCmd(LPWSTR fnName)
{
	//这里是关于迭代器的使用 //cpp 真tm 难用 都不及时补充基本类函数????
	for (it = funcMap.begin(); it != funcMap.end(); it++)
	{
		LPWSTR tmp = it->first;

		if (!wcscmp(tmp, fnName))		//这里迭代判断指令
		{
			return(it->second)(0, 0);		//对的上指令的就 调用函数
		}
	}
	return FALSE;
}

