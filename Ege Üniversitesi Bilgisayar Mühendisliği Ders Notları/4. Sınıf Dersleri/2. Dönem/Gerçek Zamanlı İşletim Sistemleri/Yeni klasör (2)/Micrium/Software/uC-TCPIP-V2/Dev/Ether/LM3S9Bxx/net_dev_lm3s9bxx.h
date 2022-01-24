/*
*********************************************************************************************************
*                                             uC/TCP-IP V2
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        NETWORK DEVICE DRIVER
*
*                                       LUMINARY MICRO LM3S9Bxx
*
* Filename      : net_dev_lm3s9bxx.h
* Version       : V2.10.02
* Programmer(s) : BAN
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/TCP-IP V2.02 (or more recent version) is included in the project build.
*
*                 (2) The driver support DMA using the Micro Drect Memory Access (uDMA) module.
*
*                 (3) Only the following devices are supported by this driver:
*
*                         LMS9B90.
*                         LMS9B92.
*                         LMS9B95.
*                         LMS9B96.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  NET_DEV_LM3S9Bxx_MODULE_PRESENT
#define  NET_DEV_LM3S9Bxx_MODULE_PRESENT


/*
*********************************************************************************************************
*                                 MICRO DIRECT MEMORY ACCESS (uDMA) DEFINES
*********************************************************************************************************
*/

#define  NET_DEV_LM3S9Bxx_DMA_BASE_ADDR          0x400FF000
#define  NET_DEV_LM3S9Bxx_DMA_RX_CH                       6u
#define  NET_DEV_LM3S9Bxx_DMA_TX_CH                       7u
#define  NET_DEV_LM3S9Bxx_DMA_NBR_CH                     64u    /* 32 Primary channels + 32 Alternative channels        */


/*
*********************************************************************************************************
*                                      DEVICE DRIVER ERROR CODES
*
* Note(s) : (1) ALL device-independent error codes #define'd in      'net_err.h';
*               ALL device-specific    error codes #define'd in this 'net_dev_&&&.h'.
*
*           (2) Network error code '11,000' series reserved for network device drivers.
*               See 'net_err.h  NETWORK DEVICE ERROR CODES' to ensure that device-specific
*               error codes do NOT conflict with device-independent error codes.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

extern  const  NET_DEV_API_ETHER  NetDev_API_LM3Sxxxx;


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*
* Note(s) : (1) The received packet is organized in the LM3S9Bxx ethernet controller's FIFO as:
*
*       +=================+======+==================+
*       | FIFO Word Rd/Wr | Byte |      RX FIFO     |
*       |=================|======+==================+                          +=========+          -----
*       |       1st       |  0   | Frame Length LSB |                          | xxxxxxx | + 0x00     ^
*       |                 |  1   | Frame Length MSB |                          | xxxxxxx | + 0x01     |
*       |                 |  2   |    DA Octet 1    |     -----                | Octet 1 | + 0x02     |
*       |                 |  3   |    DA Octet 2    |       ^                  | Octet 2 | + 0x03     |
*       +=================|======|==================+       |       -----      |=========|            |
*       |       2st       |  0   |    DA Octet 3    |       |         ^        | Octet 3 | + 0x04     |
*       |                 |  1   |    DA Octet 4    |       |         |        | Octet 4 | + 0x05     |
*       |                 |  2   |    DA Octet 5    |       |         |        | Octet 5 | + 0x06     |
*       |                 |  3   |    DA Octet 6    |       |         |        | Octet 6 | + 0x07     |
*       +=================|======|==================+       |         |        |=========|            |
*                            .                                                 |    .    |
*                            .                           Ethernet    DMA       |    .    |         Network
*                            .                            Frame    Transfer    |    .    |         Buffer
*                            .                                                 |    .    |
*       +=================|======|==================+       |         |        |    .    |            |
*       |       Last      |  0   |      FCS 1a      |       |         |        |    .    |            |
*       |                 |  1   |      FCS 2a      |       |         |        |    .    |            |
*       |                 |  2   |      FCS 3a      |       |         |        |    .    |            |
*       |                 |  3   |      FCS 4a      |       v         v        |    .    |            v
*       +=================|======|==================+     -----     -----      +=========+          -----
*
*               because the size of a received packet is not known until the header is examined,
*               the frame size must be determined first. Once the packet length is known, a
*               DMA transfer can be set up to transfer the remaining received packet payload from
*               the FIFO into a buffer.
*
*               Since DMA transfer MUST be 32-bit aligned then the network buffer must be 32-bit
*               aligned and the data section should start at index 2.
*
*               See also 'net_dev_lm3s9bxx.c  NetDev_Init()  Note #2'.
*********************************************************************************************************
*/

#if (NET_VERSION < 21000u)
                                                                /* ----------- BUF DATA SECTION (SEE NOTE #1) --------- */
#if     (NET_BUF_DATA_BASE != 2u)
#error  "NET_BUF_DATA_BASE   illegally #define'd in 'net_if.h'"
#error  "                    [MUST be = 2u]                   "
#endif

#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of LM3S9Bxx module include.                      */

