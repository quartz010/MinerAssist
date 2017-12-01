#include "stdafx.h"
#include "mineSweeper.h"

CONST LPWORD offsetFnClick = (LPWORD)/*0x10037E1*/0x01003512;        //选方格的函数地址
CONST LPBYTE offsetFiledaddr = (LPBYTE)0x1005361;
CONST LPWORD offsetSizeY = (LPWORD)0x10056A8;
CONST LPWORD offsetSizeX = (LPWORD)0x10056AC;

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
DWORD ScanFiled()
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