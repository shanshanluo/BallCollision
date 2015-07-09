#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"
#include "utils/uartstdio.h"
#include "usb_device.h"


#define DEBUG_PRINT UARTprintf


#define USB_INBUF_SIZE	BALL_NUM*16*5


unsigned int guInitDataNum;		// initial data number received from PC
tUSBInBufObject USBInBufHandle;
unsigned char USBInbuf[USB_INBUF_SIZE];
extern volatile char InitBallInfoRdy;

//*****************************************************************************
//
// Variables tracking transmit and receive counts.

//*****************************************************************************
volatile unsigned long g_ulUSBTxCount = 0;
volatile unsigned long g_ulUSBRxCount = 0;

//*****************************************************************************
//
// Global flag indicating that a USB configuration has been set.
//
//*****************************************************************************
static volatile tBoolean g_bUSBConfigured = false;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************


//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const unsigned char g_pLangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const unsigned char g_pManufacturerString[] =
{
    (17 + 1) * 2,
    USB_DTYPE_STRING,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0,
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const unsigned char g_pProductString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'G', 0, 'e', 0, 'n', 0, 'e', 0, 'r', 0, 'i', 0, 'c', 0, ' ', 0, 'B', 0,
    'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0,
    'e', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const unsigned char g_pSerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The data interface description string.
//
//*****************************************************************************
const unsigned char g_pDataInterfaceString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0,
    'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const unsigned char g_pConfigString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const unsigned char * const g_pStringDescriptors[] =
{
    g_pLangDescriptor,
    g_pManufacturerString,
    g_pProductString,
    g_pSerialNumberString,
    g_pDataInterfaceString,
    g_pConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_pStringDescriptors) /                \
                                sizeof(unsigned char *))

//*****************************************************************************
//
// The bulk device initialization and customization structures. In this case,
// we are using USBBuffers between the bulk device class driver and the
// application code. The function pointers and callback data values are set
// to insert a buffer in each of the data channels, transmit and receive.
//
// With the buffer in place, the bulk channel callback is set to the relevant
// channel function and the callback data is set to point to the channel
// instance data. The buffer, in turn, has its callback set to the application
// function and the callback data set to our bulk instance structure.
//
//*****************************************************************************
tBulkInstance g_sBulkInstance;

extern const tUSBBuffer g_sTxBuffer;
extern const tUSBBuffer g_sRxBuffer;

tUSBDBulkDevice g_sBulkDevice =
{
    USB_VID_STELLARIS,
    USB_PID_BULK,
    500,
    USB_CONF_ATTR_SELF_PWR,
    USBBufferEventCallback,
    (void *)&g_sRxBuffer,
    USBBufferEventCallback,
    (void *)&g_sTxBuffer,
    g_pStringDescriptors,
    NUM_STRING_DESCRIPTORS,
    &g_sBulkInstance
};

//*****************************************************************************
//
// Receive buffer (from the USB perspective).
//
//*****************************************************************************
#define BULK_RX_BUFFER_SIZE BALL_NUM*16+12

unsigned char g_pucUSBRxBuffer[BULK_RX_BUFFER_SIZE];
unsigned char g_pucRxBufferWorkspace[USB_BUFFER_WORKSPACE_SIZE];
const tUSBBuffer g_sRxBuffer =
{
    false,                           // This is a receive buffer.
    RxHandler,                       // pfnCallback
    (void *)&g_sBulkDevice,          // Callback data is our device pointer.
    USBDBulkPacketRead,              // pfnTransfer
    USBDBulkRxPacketAvailable,       // pfnAvailable
    (void *)&g_sBulkDevice,          // pvHandle
    g_pucUSBRxBuffer,                // pcBuffer
    BULK_RX_BUFFER_SIZE,                // ulBufferSize
    g_pucRxBufferWorkspace           // pvWorkspace
};

//*****************************************************************************
//
// Transmit buffer (from the USB perspective).
//
//*****************************************************************************
#define BULK_TX_BUFFER_SIZE BALL_NUM*16+1

unsigned char g_pucUSBTxBuffer[BULK_TX_BUFFER_SIZE];
unsigned char g_pucTxBufferWorkspace[USB_BUFFER_WORKSPACE_SIZE];
const tUSBBuffer g_sTxBuffer =
{
    true,                            // This is a transmit buffer.
    TxHandler,                       // pfnCallback
    (void *)&g_sBulkDevice,          // Callback data is our device pointer.
    USBDBulkPacketWrite,             // pfnTransfer
    USBDBulkTxPacketAvailable,       // pfnAvailable
    (void *)&g_sBulkDevice,          // pvHandle
    g_pucUSBTxBuffer,                // pcBuffer
    BULK_TX_BUFFER_SIZE,                // ulBufferSize
    g_pucTxBufferWorkspace           // pvWorkspace
};


/****************************************************************************
This is just a test code. for the real case, this API will check if tx_buffer has
enough data to be sent to host PC. If enough data is ready in tx_buffer, send it out
and return number of data sent; otherwise, return "0" to show that no data is sent successfully
*****************************************************************************/
#define DREQ_NUM	4
#define DISPLAY_MODE	1

#define DRES_NUM	16
// maximun package size of USB bulk device is 64-byte
int TmpBuf[16];

static long
USBDevRespondData(tUSBDBulkDevice *psDevice, tUSBInBufObject *psInBufHdl, unsigned char *pcData, unsigned long ulNumIn)
{
	unsigned long ulSpace, i;
	unsigned char avail;
    tUSBBuffer *dev_ptr = (tUSBBuffer *)(psDevice->pvTxCBData);
    unsigned char *buf_ptr = dev_ptr->pcBuffer;
    unsigned int ulNumOut = psInBufHdl->ulInbufGrSize;
    static int total_num;
    int num_packets, remaining;
   

    USBBufferRead((tUSBBuffer *)psDevice->pvRxCBData, (unsigned char *)TmpBuf, ulNumIn);
    if((TmpBuf[0]!='D')||(TmpBuf[1]!='R')||(TmpBuf[2]!='E')||(TmpBuf[3]!='Q')||(ulNumIn != DRES_NUM))
    {
			//TmpBuf[1]: number of balls.
    		if(guInitDataNum == 0) {
    			if(TmpBuf[0] == DISPLAY_MODE)
    				total_num = TmpBuf[1] * sizeof(tBallData) + 8;
    			else
    				total_num = TmpBuf[1] * sizeof(tBallData) + 12;
    		}
    		guInitDataNum += ulNumIn;

    		if(guInitDataNum >= total_num)
    			InitBallInfoRdy = 1;
    		else
    			InitBallInfoRdy = 0;

    }
    else {
 			// check if input buffer has enough data to copy to USB TX_Buffer
 			avail = USBInBufReadable(psInBufHdl, ulNumOut);
 			if(!avail)
 				return -1;
 
    	// How much space is there in the transmit buffer?
    	ulSpace = USBBufferSpaceAvailable((tUSBBuffer *)(psDevice->pvTxCBData));
			if(ulSpace < ulNumOut)
				return -2;
    
		USBBufferInit((tUSBBuffer *)(psDevice->pvTxCBData));
		USBInBufRead(psInBufHdl, buf_ptr, ulNumOut);
    	    	
    	// send data to host.
		if(ulNumOut <= 64)
			USBBufferDataWritten((tUSBBuffer *)(psDevice->pvTxCBData), ulNumOut);
		else {
			num_packets = ulNumOut >> 6;
			remaining = ulNumOut & 0x3f;
			for(i=0; i<num_packets; i++)
				USBBufferDataWritten((tUSBBuffer *)(psDevice->pvTxCBData), 64);
			USBBufferDataWritten((tUSBBuffer *)(psDevice->pvTxCBData), remaining);
		}
	}
	return 0;
}

static unsigned long
RxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData)
{
    //
    // Which event are we being sent?
    //
    switch(ulEvent)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
        {
            g_bUSBConfigured = true;
            UARTprintf("Host connected.\n");

            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            break;
        }

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
        {
            g_bUSBConfigured = false;
            UARTprintf("Host disconnected.\n");
            break;
        }

      // A new packet has been received.
      //
      case USB_EVENT_RX_AVAILABLE:
      {
          tUSBDBulkDevice *psDevice;

          psDevice = (tUSBDBulkDevice *)pvCBData;

          UARTprintf("rx_available.\n");
#ifdef V1
          return(USBDevRespondData(psDevice, pvMsgData, ulMsgValue));
#else
          return(USBDevRespondData(psDevice, &USBInBufHandle, pvMsgData, ulMsgValue));
#endif
      }

      //
             // Ignore SUSPEND and RESUME for now.
             //
       case USB_EVENT_SUSPEND:
       case USB_EVENT_RESUME:
       {
    	   UARTprintf("suspend.\n");
    	   break;
        }

             //
             // Ignore all other events and return 0.
             //
         default:
         {
        	 UARTprintf("default.\n");
        	 break;
         }
    }

	return 0;
}

static unsigned long
TxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData)
{

    //
    // We are not required to do anything in response to any transmit event
    // in this example. All we do is update our transmit counter.
    //
    if(ulEvent == USB_EVENT_TX_COMPLETE)
    {
    	UARTprintf("TX_COMPLETE_TISR.\n");
//    	g_ulTxCount += ulMsgValue;
    }

    //
    // Dump a debug message.
    //
    UARTprintf("TX complete %d\n", ulMsgValue);

    return(0);
}


static void USBInBufInit(tUSBInBufObject *pInBufHandle, unsigned char *pucBuf, unsigned int ulBufSize, unsigned int ulGrSize)
{
	 //
    // Initialize the ring buffer object.
    //
    pInBufHandle->ulInbufGrSize = ulGrSize;
    USBRingBufInit((tUSBRingBufObject *) &(pInBufHandle->pRingBufHandle), pucBuf, ulBufSize);
}

// return 1: USB ring buffer has enough space to store new data; 
// "0": no enough space to store new data
int USBInBufWritable(tUSBInBufObject *pInBufHandle, unsigned int ulDataWrNum)
{
	unsigned int ulBufFreeSize;
	tUSBRingBufObject *pRingBufHdl = (tUSBRingBufObject *) &(pInBufHandle->pRingBufHandle);
	
		//
    // How many space is there in the transmit buffer?
    //
    ulBufFreeSize = USBRingBufFree(pRingBufHdl);
		if(ulBufFreeSize < ulDataWrNum)
			return 0;
		else
			return 1;
}

// return 1: USB ring buffer has enough data to tramsmit; 
// "0": no enough data to transmit
int USBInBufReadable(tUSBInBufObject *pInBufHandle, unsigned int ulDataRdNum)
{
	unsigned int ulBufDataSize; 
	tUSBRingBufObject *pRingBufHdl = (tUSBRingBufObject *) &(pInBufHandle->pRingBufHandle);

	ulBufDataSize = USBRingBufUsed(pRingBufHdl);
	if(ulBufDataSize <= ulDataRdNum)
		return 0;
	else 
		return 1;
}

void USBInBufWrite(tUSBInBufObject *pInBufHandle, unsigned char *pucData, unsigned int ulDataWrNum)
{
	tUSBRingBufObject *pRingBufHdl = (tUSBRingBufObject *) &(pInBufHandle->pRingBufHandle);
	
	USBRingBufWrite(pRingBufHdl, pucData, ulDataWrNum);
}

void USBInBufRead(tUSBInBufObject *pInBufHandle, unsigned char *pucData, unsigned int ulDataRdNum)
{
	tUSBRingBufObject *pRingBufHdl = (tUSBRingBufObject *) &(pInBufHandle->pRingBufHandle);
	
	USBRingBufRead(pRingBufHdl, pucData, ulDataRdNum);
}

void * USBDeviceInit(unsigned int size)
{
	void * pDevHandle;


	// Enable the GPIO peripheral used for USB, and configure the USB pins.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	// Not configured initially.
    g_bUSBConfigured = false;
    
  // initialize USB input buffer
  	USBInBufInit((tUSBInBufObject *)&USBInBufHandle, USBInbuf, USB_INBUF_SIZE, size);
  
    // Initialize the transmit and receive buffers.
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);


    // Set the USB stack mode to Device mode with VBUS monitoring.
    USBStackModeSet(0, USB_MODE_DEVICE, 0);


    // Pass our device information to the USB library and place the device
    // on the bus.
    pDevHandle = USBDBulkInit(0, &g_sBulkDevice);
    
    UARTprintf("usb device config completed!\n");
    
	return (void *)(pDevHandle);
}

// arguments: pcCtrl: testing_mode, ball_number, time_interval
//	pcBallInfo: start address of each ball information.
void CbReadInfo2Detector(unsigned int *pcData)
{
	tUSBBuffer *dev_ptr = (tUSBBuffer *)(g_sBulkDevice.pvRxCBData);
	unsigned char *buf_ptr = dev_ptr->pcBuffer;
	unsigned int Num = guInitDataNum;
	int i = 0;
	int mode;

	mode  = buf_ptr[0] | (buf_ptr[1]<<8) | (buf_ptr[2]<<16) | (buf_ptr[3]<<24);
	*pcData++ = mode;
	*pcData++ = buf_ptr[4] | (buf_ptr[5]<<8) | (buf_ptr[6]<<16) | (buf_ptr[7]<<24);

	if(mode == DISPLAY_MODE){
		*pcData++ = 0;
		i = 8;
	} else {
		*pcData++ = buf_ptr[8] | (buf_ptr[9]<<8) | (buf_ptr[10]<<16) | (buf_ptr[11]<<24);	//get time interval
		i = 12;
	}


	for( ;i < Num; i+=4)
	{
		*pcData++ = buf_ptr[i] | (buf_ptr[i+1]<<8) | (buf_ptr[i+2]<<16) | (buf_ptr[i+3]<<24);
	}
}

