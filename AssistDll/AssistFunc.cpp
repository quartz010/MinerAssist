#include "stdafx.h"
#include "AssistFunc.h"
#include "IPC.h"
#include <map>

CONST LPWORD offsetFnClick = (LPWORD)/*0x10037E1*/0x01003512;        //ѡ����ĺ�����ַ
CONST LPBYTE offsetFiledaddr = (LPBYTE)0x1005361;
CONST LPWORD offsetSizeY = (LPWORD)0x10056A8;
CONST LPWORD offsetSizeX = (LPWORD)0x10056AC;
//  ����ɨ����Ϣ��
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

//ʵ����
CmineInfo minfo;

BOOL Click(DWORD x, DWORD y)
{
	_asm {
		//����ѹ�����
		push x       
		push y       
		call offsetFnClick  //����ѡ������
	}
	return 0;
}

DWORD ScanFiled(INT a, INT b)
{
	BYTE currBlock;
	WCHAR info[64];
	wsprintf(info, L"%d x %d", *offsetSizeX, *offsetSizeY);
//	MessageBox(NULL, info, L"Title", NULL);
//	for (DWORD i = 0; i < (*offsetSizeY); i ++) Ϊʲô������Ч����һ��???д����

	for (WORD i = 0; i < (*offsetSizeY) * 32; i += 32)
	{
		for (WORD j = 0; j < (*offsetSizeX); j++)
		{
			currBlock = *(offsetFiledaddr + i + j);

			if (currBlock != 0x8E && currBlock != 0x8F)       //�ж��Ƿ������� 
			{												//���������� = 8E
				//��Ҫ̫��԰� ���������Ż��ռ�
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

	//���������С�Ƿ����(�޴��İ취)
	if (size_x < 0 || size_y < 0 || size_x > 100 || size_y >100)
	{
		wsprintf(info, L"[-] illegal size ");
		
		return FALSE;
	}

	return TRUE;
}


//����, ������ 
DWORD SetFlag(INT a, INT b)
{
	//һ�����ӵ�����
	BYTE mineNum = 0;
	BYTE currBlock;
	const BYTE flagBlock = 0x8E;

	GetWndSize();

	//�������������С
	//��������������ó� һ��ѭ����32
	for (WORD i = 0; i < minfo.m_size_y * 32; i += 32)
	{
		for (WORD j = 0; j < minfo.m_size_x; j++)
		{
			currBlock = *(offsetFiledaddr + i + j);

			if (currBlock == 0x8F)       //�ж��Ƿ�����
			{
				//������׾ͻ���С����
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


//��map��

typedef DWORD(*LPMYFUNC)(INT wparam, INT lparam);

//���ｨ�� ���� �����ֵ�!!!
//c++ 11 ���е�

std::map <LPWSTR, LPMYFUNC> funcMap = {
	{ L"AutoTAP", NULL },
	{ L"StopTime", NULL },
	{ L"Kill", NULL },
	{ L"HI", test},
	{ L"ScanFiled", ScanFiled },
	{ L"SetFlag", SetFlag }
};
//���ﶨ�� �ֵ�ĵ�����

std::map<LPWSTR, LPMYFUNC>::iterator it;

BOOL ExeCmd(LPWSTR fnName)
{
	//�����ǹ��ڵ�������ʹ�� //cpp ��tm ���� ������ʱ��������ຯ��????
	for (it = funcMap.begin(); it != funcMap.end(); it++)
	{
		LPWSTR tmp = it->first;

		if (!wcscmp(tmp, fnName))		//��������ж�ָ��
		{
			return(it->second)(0, 0);		//�Ե���ָ��ľ� ���ú���
		}
	}
	return FALSE;
}

