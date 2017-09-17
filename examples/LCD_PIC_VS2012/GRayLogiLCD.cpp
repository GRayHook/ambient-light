// GRayLogiLCD.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#define PICSIZEBYTES 307200
#define PICSIZEX 320
#define PICSIZEY 240

BYTE *GetImageDataFromHBITMAP(HDC hdc, HBITMAP bitmap)
{
	BYTE *pImageData = (BYTE *)malloc(PICSIZEBYTES);
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc, bitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(hdc, bitmap, 0, bmpInfo.bmiHeader.biHeight, pImageData, &bmpInfo, DIB_RGB_COLORS);
	BYTE tmp[PICSIZEX*4];
	int last_line = PICSIZEBYTES-PICSIZEX*4;
	for (int i = 0; i < PICSIZEY / 2; i++)
	{
		int cur_line = i*PICSIZEX*4;
		memcpy(tmp, &pImageData[cur_line], PICSIZEX*4);
		memcpy(&pImageData[cur_line], &pImageData[last_line-cur_line], PICSIZEX*4);
		memcpy(&pImageData[last_line-cur_line], tmp, PICSIZEX*4);
	}
	for(int i = 0; i < (307200 / 4); i++){
		pImageData[i*4+3] = 255;
	}
	return pImageData;
}


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Start\n");

	LogiLcdInit((wchar_t *)L"Drisnya", LOGI_LCD_TYPE_COLOR);
	
	HBITMAP hbmp = (HBITMAP)::LoadImage(NULL, L"backg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BYTE *buf;
	HDC vhdc = CreateDC( L"DISPLAY", NULL,NULL,NULL );  
	HDC hdc = CreateCompatibleDC(vhdc);  

	SelectObject( hdc, hbmp );
	SetBkColor(hdc, RGB(255,0,0));
	SetTextColor(hdc, RGB(0,255,0));
	HBRUSH brush = CreateSolidBrush(RGB(0,0,255));
	SelectObject( hdc, brush );

	TextOut( hdc, 0, 0, L"HELLO WORLD", 11 );
	Rectangle(hdc, 50,50,150,150);

	buf = GetImageDataFromHBITMAP(hdc,hbmp);
	LogiLcdColorSetBackground((BYTE *)buf);
	free(buf);

	LogiLcdUpdate();

	printf("Press any key...\n");
	_getch();

	LogiLcdShutdown();

	return 0;
}

