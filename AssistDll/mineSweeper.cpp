#include "stdafx.h"
#include "mineSweeper.h"

CONST DWORD offsetFnClick = /*0x10037E1*/0x01003512;        //选方格的函数地址
CONST DWORD offsetFiledaddr = 0x1005361;
CONST DWORD offsetSizeX = 0x10056A8;
CONST DWORD offsetSizey = 0x10056AC;

BOOL Click(DWORD x, DWORD y)
{
	_asm {
		//这里压入参数
		push x       
		push y       
		call offsetFnClick  //调用选方格函数
	}

}

