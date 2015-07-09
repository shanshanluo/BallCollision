#include <windows.h>
#include <strsafe.h>
#include <initguid.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "lmusbdll.h"
#include "luminary_guids.h"

#include "resource.h"

#define USB_DATA

#define RECT_WIDTH	500
#define RECT_HEIGHT	500

#define REFRESH_INTERVAL	40	//40ms

#ifdef USB_DATA
////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
// Buffer size definitions.
//
//****************************************************************************
#define BALL_NUM	500
#define MAX_STRING_LEN 256
#define MAX_ENTRY_LEN 256
#define SZ_BUFFER_LEN BALL_NUM*4

#define IDT_1	1

int TxBuffer[SZ_BUFFER_LEN];
int RxBuffer[SZ_BUFFER_LEN];
UINT timer_id;

LMUSB_HANDLE hUSB;


typedef struct{
	unsigned int x_pos;
	unsigned int y_pos;
	int x_velocity;
	int y_velocity;
}
tBallData;

typedef struct { 
	unsigned int test_mode;
	unsigned int ball_num;
	unsigned int time_interval;
	tBallData DisplayBallInfo[BALL_NUM];
}
tBallDisplayInfo;

tBallDisplayInfo DisplayHandler;

FILE * fp_out = NULL;

TCHAR g_pcErrorString[MAX_STRING_LEN];

#endif
////////////////////////////////////////////////////////////////


const char g_szClassName[] = "myWindowClass";
const int ID_TIMER = 1;

const int BALL_MOVE_DELTA = 5;

typedef struct _BALLINFO 
{
	int width;
	int height;
	int x;
	int y;
}BALLINFO;

BALLINFO g_ballInfo[BALL_NUM];

HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;

// g_hbmBall2 = NULL;
//HBITMAP g_hbmMask2 = NULL;
/////////////////////////////////////////////////////////////////
#ifdef USB_DATA
LPTSTR GetSystemErrorString(DWORD dwError)
{
    DWORD dwRetcode;

    //
    // Ask Windows for the error message description.
    //
    dwRetcode = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, "%0", dwError, 0,
                              g_pcErrorString, MAX_STRING_LEN, NULL);

    if(dwRetcode == 0)
    {
        return((LPTSTR)L"Unknown");
    }
    else
    {
        //
        // Remove the trailing "\n\r" if present.
        //
        if(dwRetcode >= 2)
        {
            if(g_pcErrorString[dwRetcode - 2] == '\r')
            {
                g_pcErrorString[dwRetcode - 2] = '\0';
            }
        }

        return(g_pcErrorString);
    }
}

#define DREQ_NUM	16
#define FRM_SIZE	16
#define DISPLAY		1
#define STOP_LOOP	1
unsigned char IndexNum = 0;
int SendInitData2ARM(void)
{
		UINT BallInfoNum;
		UINT ulWritten, bResult;
		FILE * fp_in = NULL;
		unsigned int i, ball_id;

		fp_in = fopen("initial.txt", "rb");
		if(!fp_in)
		{
			printf("input file open error!\n");
			return -1;
		}

		i = 0;
		fscanf(fp_in, "%d\n", &TxBuffer[i++]);	//read mode
		DisplayHandler.test_mode = TxBuffer[0];
		fscanf(fp_in, "%d\n", &TxBuffer[i++]);	//ball_num
		DisplayHandler.ball_num = TxBuffer[1];

		if(DisplayHandler.test_mode == DISPLAY){
			DisplayHandler.time_interval = 0;
			IndexNum = 2;
		} else {
			fscanf(fp_in, "%d\n", &TxBuffer[i++]);
			DisplayHandler.time_interval = TxBuffer[2];
			IndexNum = 3;
		}

		BallInfoNum = DisplayHandler.ball_num * sizeof(tBallData);
		
		ball_id = 0;
		while(ball_id < DisplayHandler.ball_num) {
			i = 0;
			fscanf(fp_in, "%d\n", &bResult);
			while(i < (sizeof(tBallData)>>2)){
				fscanf(fp_in, "%d\n", &TxBuffer[IndexNum+(ball_id<<2)+i]);
				*(int *)(&(DisplayHandler.DisplayBallInfo[0])) = TxBuffer[IndexNum + (ball_id<<2)+i];
				i++;
			}
			ball_id++;
		}
			
		bResult = WriteUSBPacket(hUSB, (unsigned char *)TxBuffer, (BallInfoNum+(IndexNum<<2)), (unsigned long *)&ulWritten);
		if(!bResult)
		{
			// We failed to write the data for some reason.
			bResult = GetLastError();
			printf("Error %d (%S) writing to bulk OUT pipe.\n", bResult, GetSystemErrorString(bResult));
		}


		if(DisplayHandler.test_mode != DISPLAY) {
			fp_out = fopen("ball_info.txt", "w");
			if(!fp_out)
			{
				printf("output file open error!\n");
				return -1;
			}

		}

		if(fp_in)
			fclose(fp_in);
		return 0;
}

//#define DEBUG_DS
void UsbTimerEventProc(void)
{
	unsigned int i = 0;
	int bResult, dwError;
	unsigned long ulWritten, ulRead;
	static unsigned int loop_cnt = 0;
#ifdef DEBUG_DS
	static int bak_x[20], bak_y[20];
	static unsigned int count=0;
#endif

	
	TxBuffer[0] = 'D';
	TxBuffer[1] = 'R';
	TxBuffer[2] = 'E';
	TxBuffer[3] = 'Q';

	
	bResult = WriteUSBPacket(hUSB, (unsigned char *)TxBuffer, DREQ_NUM, &ulWritten);
	if(!bResult)
	{
		//
		// We failed to write the data for some reason.
		//
		dwError = GetLastError();
		printf("Error %d (%S) writing to bulk OUT pipe.\n", dwError,
		GetSystemErrorString(dwError));
	}
	else
	{
		loop_cnt++;
		printf("Wrote %d bytes to the device: 0x%x, 0x%x, 0x%x, 0x%x.\n", ulWritten, TxBuffer[0], TxBuffer[1], TxBuffer[2], TxBuffer[3]);

		dwError = ReadUSBPacket(hUSB, (unsigned char *)RxBuffer, DisplayHandler.ball_num * sizeof(tBallData), 
									&ulRead, INFINITE, NULL);

		if(dwError != ERROR_SUCCESS)
		{
			// We failed to read from the device
            printf("Error %d (%S) reading from bulk IN pipe.\n", dwError, GetSystemErrorString(dwError));
        }
		else
		{
			for(i = 0; i < DisplayHandler.ball_num; i++) {
				DisplayHandler.DisplayBallInfo[i].x_pos = RxBuffer[i<<2];
				DisplayHandler.DisplayBallInfo[i].y_pos = RxBuffer[(i<<2)+1];
				DisplayHandler.DisplayBallInfo[i].x_velocity = RxBuffer[(i<<2)+2];
				DisplayHandler.DisplayBallInfo[i].y_velocity = RxBuffer[(i<<2)+3];
				
//				printf("loop_cnt=%d, x_pos=%d, y_pos=%d, x_velocity=%d, y_velocity=%d.\n", loop_cnt,
//						DisplayHandler.DisplayBallInfo[i].x_pos, DisplayHandler.DisplayBallInfo[i].y_pos, 
//						DisplayHandler.DisplayBallInfo[i].x_velocity, DisplayHandler.DisplayBallInfo[i].y_velocity);
			}
#ifdef DEBUG_DS			
			count++;
			if(count == 240)
				printf("bak_x\n");
#endif
			// visualization
			if(DisplayHandler.test_mode == DISPLAY) {
				for(i = 0; i < DisplayHandler.ball_num; i++) {
					//convert mm to pixels
					g_ballInfo[i].x = (int)(DisplayHandler.DisplayBallInfo[i].x_pos>>1) - (g_ballInfo[i].width >> 1);
					g_ballInfo[i].y =  RECT_HEIGHT - (int)(DisplayHandler.DisplayBallInfo[i].y_pos>>1) - (g_ballInfo[i].height >> 1);
#ifdef DEBUG_DS	
/*					if((g_ballInfo[i].x == bak_x[i])&&(g_ballInfo[i].y == bak_y[i]))
						printf("bak_x\n");
					bak_x[i] = g_ballInfo[i].x;
					bak_y[i] = g_ballInfo[i].y;
*/
					if(g_ballInfo[i].y >= 490)
						printf("bak_x\n");
//					bak_y[i] = g_ballInfo[i].y;
#endif
				}

			} else { // output to file
				
				if(fp_out) {
					fprintf(fp_out, "current time: %dms\n\r", (loop_cnt*DisplayHandler.time_interval*1000));
					for(i = 0; i < DisplayHandler.ball_num; i++) {
						fprintf(fp_out, "ball%d:  x_pos:%d  y_pos:%d\n\r", (i+1), DisplayHandler.DisplayBallInfo[i].x_pos, DisplayHandler.DisplayBallInfo[i].y_pos);
					}
					if(loop_cnt == STOP_LOOP)

						fclose(fp_out);
				}
			}
			
		}
	}
}
#endif
/////////////////////////////////////////////////////////////////

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	SetBkColor(hdcMem, crTransparent);

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

void DrawBall(HDC hdc, RECT* prc)
{
	UINT i;
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, prc->right, prc->bottom);
	HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);

	FillRect(hdcBuffer, prc, (HBRUSH)GetStockObject(BLACK_BRUSH));


	for(i = 0; i < DisplayHandler.ball_num; i++) {
		BitBlt(hdcBuffer, g_ballInfo[i].x, g_ballInfo[i].y, g_ballInfo[i].width, g_ballInfo[i].height, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, g_hbmBall);
		BitBlt(hdcBuffer, g_ballInfo[i].x, g_ballInfo[i].y, g_ballInfo[i].width, g_ballInfo[i].height, hdcMem, 0, 0, SRCPAINT);
	}

	BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	SelectObject(hdcBuffer, hbmOldBuffer);
	DeleteDC(hdcBuffer);
	DeleteObject(hbmBuffer);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			UINT ret;
			BITMAP bm;
			UINT i;

			g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BALL));
			if(g_hbmBall == NULL)
				MessageBox(hwnd, "Could not load IDB_BALL!", "Error", MB_OK | MB_ICONEXCLAMATION);

			g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
			if(g_hbmMask == NULL)
				MessageBox(hwnd, "Could not create mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

			GetObject(g_hbmBall, sizeof(bm), &bm);

			ZeroMemory(&g_ballInfo, sizeof(g_ballInfo));


			for(i = 0; i < DisplayHandler.ball_num; i++) {
				g_ballInfo[i].width = bm.bmWidth;
				g_ballInfo[i].height = bm.bmHeight;

				g_ballInfo[i].x = (DisplayHandler.DisplayBallInfo[i].x_pos>>1) - (g_ballInfo[i].width>>1);
				g_ballInfo[i].y = RECT_HEIGHT - (DisplayHandler.DisplayBallInfo[i].y_pos>>1) - (g_ballInfo[i].height>>1);

				printf("x_pos=%d, y_pos=%d\n", g_ballInfo[i].x, g_ballInfo[i].y);
			}

			if(DisplayHandler.test_mode == DISPLAY)
				ret = SetTimer(hwnd, ID_TIMER, REFRESH_INTERVAL, NULL);
			else 
				ret = SetTimer(hwnd, ID_TIMER, DisplayHandler.time_interval*1000, NULL);

			if(ret == 0)
				MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_PAINT:
		{
			RECT rcClient;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			GetWindowRect(hwnd, &rcClient);
			DrawBall(hdc, &rcClient);

			EndPaint(hwnd, &ps);
		}
		break;
		case WM_TIMER:
		{
			RECT rcClient;
			HDC hdc;

			UsbTimerEventProc();
			
			if(DisplayHandler.test_mode == DISPLAY) {
				hdc = GetDC(hwnd);
				GetWindowRect(hwnd, &rcClient);

				DrawBall(hdc, &rcClient);
				ReleaseDC(hwnd, hdc);
			}			
		}
		break;
		case WM_DESTROY:
			KillTimer(hwnd, ID_TIMER);

			DeleteObject(g_hbmBall);
			DeleteObject(g_hbmMask);

			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;
	BOOL bDriverInstalled;
//	DWORD dwError;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);
	

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

/////////////////////////////////////////////////////////////////
	 //
    // Find our USB device and prepare it for communication.
    //
    hUSB = InitializeDevice(BULK_VID, BULK_PID,	\
                            (LPGUID)&(GUID_DEVINTERFACE_STELLARIS_BULK),	\
                            &bDriverInstalled);
	if(hUSB)
    {
			//set timer
		UINT uResult;
		MSG msg;

		uResult = SendInitData2ARM();
		if(uResult != 0){
			printf("send initial data error!\n");
			return -2;
		}

/////////////////////////////////////////////////////////////////
//		if(DisplayHandler.test_mode == DISPLAY) {
			hwnd = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				g_szClassName,
				"Bouncing Ball Program",
				WS_OVERLAPPEDWINDOW,
				0, 0, 520, 540,
				NULL, NULL, hInstance, NULL);

			if(hwnd == NULL)
			{
				MessageBox(NULL, "Window Creation Failed!", "Error!",
						MB_ICONEXCLAMATION | MB_OK);
				return -3;
			}

			ShowWindow(hwnd, nCmdShow);
			UpdateWindow(hwnd);
//		}
		while(GetMessage(&Msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	TerminateDevice(hUSB);

    return(0);
}
