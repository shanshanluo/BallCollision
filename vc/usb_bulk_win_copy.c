#include <windows.h>
#include <strsafe.h>
#include <initguid.h>
#include <stdio.h>
//#include <stdlib.h>
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

unsigned char TxBuffer[SZ_BUFFER_LEN];
unsigned char RxBuffer[SZ_BUFFER_LEN];
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

//****************************************************************************
//
// The build version number
//
//****************************************************************************
#define BLDVER "9107"

//****************************************************************************
//
// The number of bytes we read and write per transaction if in echo mode.
//
//****************************************************************************
#define DATA_PACKET_SIZE 64

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
ULONG g_ulByteCount = 0;

//****************************************************************************
//
// The total number of packets transfered.
//
//****************************************************************************
ULONG g_ulPacketCount = 0;

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

#define DREQ_NUM	4
#define FRM_SIZE	16
VOID CALLBACK TimerCallBack( 
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT idTimer,     // timer identifier 
    DWORD dwTime)     // current system time 
{
	unsigned int i = 0;
	int bResult, dwError;
	unsigned int ulWritten, ulRead;
	unsigned int x_pos, y_pos;
	int x_velocity, y_velocity;

	
	TxBuffer[0] = 'D';
	TxBuffer[1] = 'R';
	TxBuffer[2] = 'E';
	TxBuffer[3] = 'Q';

	
	bResult = WriteUSBPacket(hUSB, (unsigned char *)TxBuffer, DREQ_NUM, (unsigned long *)&ulWritten);
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
                    //
                    // We wrote data successfully so now read it back.
                    //
		printf("Wrote %d bytes to the device: 0x%x, 0x%x, 0x%x, 0x%x.\n", ulWritten, TxBuffer[0], TxBuffer[1], TxBuffer[2], TxBuffer[3]);

                    //
                    // We expect the same number of bytes as we just sent.
                    //
			dwError = ReadUSBPacket(hUSB, (unsigned char *)RxBuffer, FRM_SIZE, (unsigned long *)&ulRead, INFINITE, NULL);

                    if(dwError != ERROR_SUCCESS)
                    {
                        //
                        // We failed to read from the device.
                        //
                        printf("Error %d (%S) reading from bulk IN pipe.\n", dwError,
                               GetSystemErrorString(dwError));
                    }
                    else
                    {
					
							x_pos = RxBuffer[0]|(RxBuffer[1]<<8)|(RxBuffer[2]<<16)|(RxBuffer[3]<<24);
							y_pos = RxBuffer[4]|(RxBuffer[5]<<8)|(RxBuffer[6]<<16)|(RxBuffer[7]<<24);
							x_velocity = RxBuffer[8]|(RxBuffer[9]<<8)|(RxBuffer[10]<<16)|(RxBuffer[11]<<24);
							y_velocity = RxBuffer[12]|(RxBuffer[13]<<8)|(RxBuffer[14]<<16)|(RxBuffer[15]<<24);
						
						printf("x_pos=%d, y_pos=%d, x_velocity=%d, y_velocity=%d.\n",x_pos, y_pos, x_velocity, y_velocity);
                    }
	}
}

//****************************************************************************
//
// The main application entry function.
//
// \param None.
//
// This function forms the main entry point of the example application. It
// initializes the USB bulk device, prompts the user for an ASCII string,
// sends this string to the device, reads the response from the device (which
// will be the same string with the character order reversed if using the
// sample device provided in StellarisWare) and displays the returned
// string.
//
// \return Set the exit code to 0 of no errors cause the application to end
// or a non-zero value otherwise.
//
//****************************************************************************

int main(int argc, char *argv[])
{
	BOOL bDriverInstalled;
	DWORD dwError;


    //
    // Find our USB device and prepare it for communication.
    //
   hUSB = InitializeDevice(BULK_VID, BULK_PID,
                            (LPGUID)&(GUID_DEVINTERFACE_STELLARIS_BULK),
                            &bDriverInstalled);
						
//							hUSB = 0xaadd;
	if(hUSB)
    {
				//set timer
//	UINT uResult;
	MSG msg;

	UINT BallInfoNum;
	UINT ulWritten, bResult;
	FILE * fp;
	char ch;


			bResult = fopen_s(&fp, "initial.txt", "rw");
			if(bResult){
				printf("file open error!\n");
				return -1;
			}

	//		ch = getchar();

			// read initial value and send to launchpad
//			fprintf(fp, "######!");
//			fclose(fp);
			bResult = fread(TxBuffer, 4, 3, fp);
			if(bResult != 3){
				printf("initial value read error!\n");
				return -2;
			}
			DisplayHandler.test_mode = TxBuffer[0];
			DisplayHandler.ball_num = TxBuffer[1];
			DisplayHandler.time_interval = TxBuffer[2];

			BallInfoNum = DisplayHandler.ball_num * sizeof(tBallData);

			bResult = fread(TxBuffer+3, 4, BallInfoNum, fp);
			if(bResult != BallInfoNum){
				printf("initial ball information read error!\n");
				return -3;
			}
			
			bResult = WriteUSBPacket(hUSB, (unsigned char *)TxBuffer, (BallInfoNum+3), (unsigned long *)&ulWritten);
//			if(!bResult)
			{

			}

			timer_id = SetTimer(NULL, NULL, 40, (TIMERPROC)TimerCallBack);
			if(timer_id == 0)
			{
				printf("error timer handle!\n");
			}
			printf("time handle ready %d!\n", timer_id);

			
			while (GetMessage(&msg, NULL, 0, 0) != -1) 
				DispatchMessage(&msg);

			while(1);

	}
	else{
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

