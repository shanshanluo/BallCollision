//*****************************************************************************
//
// usb_dev_bulk.c - Main routines for the generic bulk device example.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "usb_device.h"



//#define PERFORMANCE_TEST

#define REC_WIDTH	1000
#define REC_HEIGHT	1000


extern tUSBInBufObject USBInBufHandle;


volatile char InitBallInfoRdy = 0;
tPosDetecthandler sPosDetector;

//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND     100
#define SYSTICK_PERIOD_MS       (1000 / SYSTICKS_PER_SECOND)

//*****************************************************************************
//
// The global system tick counter.
//
//*****************************************************************************
volatile unsigned long g_ulSysTickCount = 0;

//*****************************************************************************
//
// Variables tracking transmit and receive counts.
//
//*****************************************************************************
volatile unsigned long g_ulTxCount = 0;
volatile unsigned long g_ulRxCount = 0;
#ifdef DEBUG
unsigned long g_ulUARTRxErrors = 0;
#endif

//*****************************************************************************
//
// Debug-related definitions and declarations.
//
// Debug output is available via UART0 if DEBUG is defined during build.
//
//*****************************************************************************
#ifdef DEBUG
//*****************************************************************************
//
// Map all debug print calls to UARTprintf in debug builds.
//
//*****************************************************************************
#define DEBUG_PRINT UARTprintf

#else

//*****************************************************************************
//
// Compile out all debug print calls in release builds.
//
//*****************************************************************************
#define DEBUG_PRINT while(0) ((int (*)(char *, ...))0)
#endif

//*****************************************************************************
//
// Flags used to pass commands from interrupt context to the main loop.
//
//*****************************************************************************
#define COMMAND_PACKET_RECEIVED 0x00000001
#define COMMAND_STATUS_UPDATE   0x00000002

volatile unsigned long g_ulFlags = 0;
char *g_pcStatus;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
    UARTprintf("Error at line %d of %s\n", ulLine, pcFilename);
    while(1)
    {
    }
}
#endif

#ifdef PERFORMANCE_TEST
unsigned int GetInitCycles(void)
{
	unsigned int tick;

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 0xffffffff);
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);

	tick = TimerValueGet(TIMER0_BASE, TIMER_A);

	return tick;
}

unsigned int GetUpdatedCycles(void)
{
	unsigned int tick;

	tick = TimerValueGet(TIMER0_BASE, TIMER_A);
	TimerDisable(TIMER0_BASE, TIMER_A);

	return tick;
}
#endif

//*****************************************************************************
//
// Interrupt handler for the system tick counter.
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    // Update our system tick counter.
    g_ulSysTickCount++;
}


void SysClkSet(void)
{
	//
    // Set the clocking to run from the PLL at 50MHz
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
}

void UartSet(void)
{
    // Enable the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
    UARTprintf("\033[2JBulk device application\n");
}


void SysTickSet(void)
{

    // Enable the system tick.
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();
}



void GetInitBallInfo(tPosDetecthandler *pPosDetector)
{
	//wait for initial ball information received through USB
	while(!InitBallInfoRdy);

	CbReadInfo2Detector((unsigned int *)pPosDetector);
}


void PosDetectorInit(tPosDetecthandler *ps, tBallData *pb);
main(void)
{
    tPosDetecthandler * PtrPosDetector = &sPosDetector;
    tBallData *PtBall = (tBallData *)PtrPosDetector->pBallInfo;

    unsigned int count = 0;

#ifdef PERFORMANCE_TEST
    unsigned int tick0, tick1, cycle;
    float time;
#endif

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPULazyStackingEnable();

    SysClkSet();

	UartSet();

 	SysTickSet();

	//default scenerio is 1 ball case, this will be updated after getting the initial info from PC
 	USBDeviceInit(sizeof(tBallData));

	GetInitBallInfo(PtrPosDetector);
	USBInBufHandle.ulInbufGrSize = PtrPosDetector->ucBallNum * sizeof(tBallData);

	// To do: main calculation module init
	PosDetectorInit(PtrPosDetector, PtBall);

  	// main calculation loop
  	while(1) {

  		if(USBInBufWritable((tUSBInBufObject *)&USBInBufHandle, USBInBufHandle.ulInbufGrSize) == 1){

#ifdef PERFORMANCE_TEST
  			tick0 = GetInitCycles();
#endif
  			// To do: PosDetectorProc
  			PosDetectorProc(PtrPosDetector, PtBall);

#ifdef PERFORMANCE_TEST
  			tick1 = GetUpdatedCycles();
  			cycle = tick1 - tick0;
  			time = (float)cycle/50000000;
#endif

  			USBInBufWrite((tUSBInBufObject *)&USBInBufHandle,
  					(unsigned char *)&(PtrPosDetector->pBallInfo[0]), USBInBufHandle.ulInbufGrSize);

  			count++;

  		}
  	}  
}
