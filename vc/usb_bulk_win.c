#include <windows.h>
#include <strsafe.h>
#include <initguid.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "lmusbdll.h"
#include "luminary_guids.h"


//****************************************************************************
//
// Buffer size definitions.
//
//****************************************************************************
#define MAX_STRING_LEN 256
#define MAX_ENTRY_LEN 256
#define SZ_BUFFER_LEN 128

#define IDT_1	1

int TxBuffer[SZ_BUFFER_LEN];
int RxBuffer[SZ_BUFFER_LEN];
UINT timer_id;

LMUSB_HANDLE hUSB;

#define BALL_NUM	20
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

//****************************************************************************
//
// The build version number
//
//****************************************************************************
//#define BLDVER "9107"

//****************************************************************************
//
// The number of bytes we read and write per transaction if in echo mode.
//
//****************************************************************************
//#define DATA_PACKET_SIZE 64

//****************************************************************************
//
// Buffer into which error messages are written.
//
//****************************************************************************
TCHAR g_pcErrorString[MAX_STRING_LEN];

//****************************************************************************
//
// The number of bytes transfered in the last measurement interval.
//
//****************************************************************************
//ULONG g_ulByteCount = 0;

//****************************************************************************
//
// The total number of packets transfered.
//
//****************************************************************************
//ULONG g_ulPacketCount = 0;

//****************************************************************************
//
// Return a string describing the supplied system error code.
//
// \param dwError is a Windows system error code.
//
// This function returns a pointer to a string describing the error code
// passed in the dwError parameter. If no description string can be found
// the string "Unknown" is returned.
//
// \return Returns a pointer to a string describing the error.
//
//****************************************************************************
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
#define STOP_LOOP	50
VOID CALLBACK TimerCallBack( 
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT idTimer,     // timer identifier 
    DWORD dwTime)     // current system time 
{
	unsigned int i = 0;
	int bResult, dwError;
	unsigned long ulWritten, ulRead;
	static unsigned int loop_cnt = 0;

	
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
				
				printf("loop_cnt=%d, x_pos=%d, y_pos=%d, x_velocity=%d, y_velocity=%d.\n", loop_cnt,
						DisplayHandler.DisplayBallInfo[i].x_pos, DisplayHandler.DisplayBallInfo[i].y_pos, 
						DisplayHandler.DisplayBallInfo[i].x_velocity, DisplayHandler.DisplayBallInfo[i].y_velocity);
			}
						
			// visualization
			if(DisplayHandler.test_mode == DISPLAY) {

			} else { // output to file
				
				fprintf(fp_out, "current time: %d ms\n", (loop_cnt*DisplayHandler.time_interval));
				for(i = 0; i < DisplayHandler.ball_num; i++) {
					fprintf(fp_out, "ball%d:  x_pos:%d  y_pos:%d  Vx:%d  Vy:%d\n", (i+1), DisplayHandler.DisplayBallInfo[i].x_pos, DisplayHandler.DisplayBallInfo[i].y_pos,
						DisplayHandler.DisplayBallInfo[i].x_velocity, DisplayHandler.DisplayBallInfo[i].y_velocity);
				}
				fprintf(fp_out, "\n");
				if(loop_cnt == STOP_LOOP)
					fclose(fp_out);
			}
			
		}
	}
}

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
		while(i<3){
			fscanf(fp_in, "%d\n", &TxBuffer[i++]);
		}

		DisplayHandler.test_mode = TxBuffer[0];
		DisplayHandler.ball_num = TxBuffer[1];
		DisplayHandler.time_interval = TxBuffer[2];

		BallInfoNum = DisplayHandler.ball_num * sizeof(tBallData);
		
		ball_id = 0;
		while(ball_id < DisplayHandler.ball_num) {
			i = 0;
			fscanf(fp_in, "%d\n", &bResult);
			while(i < (sizeof(tBallData)>>2)){
				fscanf(fp_in, "%d\n", &TxBuffer[3+(ball_id<<2)+i]);
				i++;
			}
			ball_id++;
		}
			
		bResult = WriteUSBPacket(hUSB, (unsigned char *)TxBuffer, (BallInfoNum+12), (unsigned long *)&ulWritten);
		if(!bResult)
		{
			// We failed to write the data for some reason.
			bResult = GetLastError();
			printf("Error %d (%S) writing to bulk OUT pipe.\n", bResult, GetSystemErrorString(bResult));
		}


		if(DisplayHandler.test_mode != DISPLAY) {
			fp_out = fopen("ball_info.txt", "wb");
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

int main(int argc, char *argv[])
{
	BOOL bDriverInstalled;
	DWORD dwError;

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
			return -1;
		}

		timer_id = SetTimer((HWND)NULL, (UINT_PTR)NULL, DisplayHandler.time_interval, (TIMERPROC)TimerCallBack);
		if(timer_id == 0)
		{
			printf("error timer handle!\n");
		}
		printf("time handle ready %d!\n", timer_id);

			
		while (GetMessage(&msg, NULL, 0, 0) != -1) 
			DispatchMessage(&msg);

		while(1);

	}
	else
	{
        //
        // An error was reported while trying to connect to the device.
        //
        dwError = GetLastError();

        printf("\nUnable to initialize the Stellaris Bulk USB Device.\n");
        printf("Error code is %d (%S)\n\n", dwError, GetSystemErrorString(dwError));
        printf("Please make sure you have a Stellaris USB-enabled evaluation\n");
        printf("or development kit running the usb_dev_bulk example\n");
        printf("application connected to this system via the \"USB OTG\" or\n");
        printf("\"USB DEVICE\" connectors. Once the device is connected, run\n");
        printf("this application again.\n\n");

        printf("\nPress \"Enter\" to exit: ");
 //       fgets(rx_buf, MAX_STRING_LEN, stdin);
        printf("\n");
        return(2);
	}
    TerminateDevice(hUSB);

    return(0);
}

