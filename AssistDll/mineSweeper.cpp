#include "stdafx.h"
#include "mineSweeper.h"

CONST LPWORD offsetFnClick = (LPWORD)/*0x10037E1*/0x01003512;        //ѡ����ĺ�����ַ
CONST LPBYTE offsetFiledaddr = (LPBYTE)0x1005361;
CONST LPWORD offsetSizeY = (LPWORD)0x10056A8;
CONST LPWORD offsetSizeX = (LPWORD)0x10056AC;

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
DWORD ScanFiled()
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