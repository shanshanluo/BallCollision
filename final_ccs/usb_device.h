#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include "usblib/usblib.h"

#define BALL_NUM		200

typedef struct {
	unsigned int ulInbufGrSize;	//the size of each USB data transmission.
	tUSBRingBufObject pRingBufHandle;
}tUSBInBufObject;

typedef struct{
	unsigned int x_pos;
	unsigned int y_pos;
	int x_velocity;
	int y_velocity;
}
tBallData;

typedef struct{
	unsigned int mode;
	unsigned int ucBallNum;
	unsigned int time;
	tBallData pBallInfo[BALL_NUM];
}
tPosDetecthandler;



extern void * USBDeviceInit(unsigned int size);
extern int USBInBufWritable(tUSBInBufObject *pInBufHandle, unsigned int ulDataWrNum);
extern int USBInBufReadable(tUSBInBufObject *pInBufHandle, unsigned int ulDataRdNum);
extern void USBInBufWrite(tUSBInBufObject *pInBufHandle, unsigned char *pucData, unsigned int ulDataWrNum);
extern void USBInBufRead(tUSBInBufObject *pInBufHandle, unsigned char *pucData, unsigned int ulDataRdNum);
extern void CbReadInfo2Detector(unsigned int *pcData);

static unsigned long RxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData);
static unsigned long TxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData);


#endif
