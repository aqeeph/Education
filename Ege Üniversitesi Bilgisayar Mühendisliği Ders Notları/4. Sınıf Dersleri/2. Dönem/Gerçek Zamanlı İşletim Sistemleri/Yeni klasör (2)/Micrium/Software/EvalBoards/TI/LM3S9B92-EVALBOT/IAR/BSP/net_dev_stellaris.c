
/*
*********************************************************************************************************
*                                              uC/IPerf
*                                        The Embedded Product
*
*                         (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*              All rights reserved.  Protected by international copyright laws.
*
*              uC/TCP-IP is provided in source form to registered licensees ONLY.  It is
*              illegal to distribute this source code to any third party unless you receive
*              written permission by an authorized Micrium representative.  Knowledge of
*              the source code may NOT be used to develop a similar product.
*
*              Please help us continue to provide the Embedded community with the finest
*              software available.  Your honesty is greatly appreciated.
*
*              You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        NETWORK DEVICE DRIVER
*
*                                Texas Instruments Stellaris MAC 10/100
*
* Filename      : net_dev_stm32f107.c
* Version       : V2.10.02
* Programmer(s) : AF
*                 EHS
*                 AA
*				  DGT
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/TCP-IP V2.02 (or more recent version) is included in the project build.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_DEV_STELLARIS_MODULE
#include  <net.h>
#include  <net_dev_stellaris.h>
#include  <includes.h>



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define FLAG_RXPKT              1
#define FLAG_TXPKT              2
#define FLAG_RXPKTPEND          3

#define  RX_BUF_SIZE_MULT      4          /* Rx bufs must be a mul   of 4 bytes.           */
#define  RX_DESC_BUF_ALIGN     4          /* Rx bufs must be aligned to 4 byte boundary.   */
#define  TX_DESC_BUF_ALIGN     4          /* Tx bufs must be aligned to 4 byte boundary.   */


/*
*********************************************************************************************************
*                                                 GLOBAL VARIABLES
*********************************************************************************************************
*/

#pragma data_alignment=1024
tDMAControlTable g_sDMAControlTable[8];

static volatile unsigned long g_ulFlags;		/* used for Stellaris DMA state tracking 				*/


/*
*********************************************************************************************************
*                                                 MACROS
*********************************************************************************************************
*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*
* Note(s) : (1) Instance specific data area structures should be defined below.  The data area
*               structure typically includes error counters and variables used to track the
*               state of the device.  Variables required for correct operation of the device
*               MUST NOT be defined globally and should instead be included within the instance
*               specific data area structure and referenced as pif->Dev_Data structure members.
*
*           (2) DMA based devices may require more than one type of descriptor.  Each descriptor
*               type should be defined below.  An example descriptor has been provided.
*
*           (3) All device drivers MUST track the addresses of ALL buffers that have been
*               transmitted and not yet acknowledged through transmit complete interrupts.
*********************************************************************************************************
*/

typedef  struct  dev_desc  DEV_DESC;

struct dev_desc {
    CPU_REG32    Status;                                        /* DMA status register.                                 */
    CPU_REG32    Len;                                           /* DMA buffer size.                                     */
    CPU_INT08U  *Addr;                                          /* DMA buffer pointer                                   */
    DEV_DESC    *Next;                                          /* DMA next descriptor pointer.                         */
};

                                                                /* --------------- DEVICE INSTANCE DATA --------------- */
typedef  struct  net_dev_data {
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    NET_CTR      StatRxCtrRx;
    NET_CTR      StatRxCtrRxISR;
    NET_CTR      StatTxCtrTxISR;
    NET_CTR      StatRxCtrTaskSignalISR;
    NET_CTR      StatRxCtrTaskSignalSelf;
    NET_CTR      StatRxCtrTaskSignalNone;
#endif

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    NET_CTR      ErrRxCtrDataAreaAlloc;
    NET_CTR      ErrRxCtr;
    NET_CTR      ErrRxCtrOF;
    NET_CTR		 ErrTxCtr;
    NET_CTR      ErrRxCtrDescES;
    NET_CTR      ErrRxCtrDescOWN;
    NET_CTR      ErrRxCtrTaskSignalFail;
#endif

    CPU_REG32    IntStatusLast;

    DEV_DESC    *RxBufDescPtrStart;
    DEV_DESC    *RxBufDescPtrCur;
    DEV_DESC    *RxBufDescPtrEnd;

    DEV_DESC    *TxBufDescPtrStart;
    DEV_DESC    *TxBufDescPtrCur;
    DEV_DESC    *TxBufDescPtrComp;                              /* See Note #3.                                         */

#ifdef NET_MULTICAST_PRESENT
    CPU_INT08U   MulticastAddrHashBitCtr[64];
#endif
} NET_DEV_DATA;


/*
*********************************************************************************************************
*									LOCAL FUNCTION PROTOTYPES
*
* Note(s) : (1) Device driver functions may be arbitrarily named.  However, it is recommended that device
*               driver functions be named using the names provided below.  All driver function prototypes
*               should be located within the driver C source file ('net_dev_&&&.c') & be declared as
*               static functions to prevent name clashes with other network protocol suite device drivers.
*********************************************************************************************************
*/
static void NetDev_Init_Stellaris_Eth0 					(NET_IF *pif,
														 NET_ERR *perr);
														
static void NetDev_Start_Stellaris_Eth0 				(NET_IF *pif,
														 NET_ERR *perr);

static void NetDev_Stop_Stellaris_Eth0 					(NET_IF *pif,
														 NET_ERR *perr);

static void NetDev_Rx_Stellaris_Eth0 					(NET_IF *pif,
														 CPU_INT08U **p_data,
														 CPU_INT16U *size,
														 NET_ERR *perr);

static void NetDev_Tx_Stellaris_Eth0 					(NET_IF *pif,
														 CPU_INT08U *p_data,
														 CPU_INT16U size,
														 NET_ERR *perr);

#if (NET_VERSION >= 205u)
static void NetDev_AddrMulticastAdd_Stellaris_Eth0 		(NET_IF *pif,
														 CPU_INT08U *paddr_hw,
														 CPU_INT08U addr_hw_len,
														 NET_ERR *perr);

static void NetDev_AddrMulticastRemove_Stellaris_Eth0 	(NET_IF *pif,
														 CPU_INT08U *paddr_hw,
														 CPU_INT08U addr_hw_len,
														 NET_ERR *perr);
#endif

static void NetDev_ISR_Handler_Stellaris_Eth0 			(NET_IF *pif,
														 NET_DEV_ISR_TYPE type);

static void NetDev_IO_Ctrl_Stellaris_Eth0 				(NET_IF *pif,
														 CPU_INT08U opt,
														 void *p_data,
														 NET_ERR *perr);

static void NetDev_MII_Rd_Stellaris_Eth0 				(NET_IF *pif,
														 CPU_INT08U phy_addr,
														 CPU_INT08U reg_addr,
														 CPU_INT16U *p_data,
														 NET_ERR *perr);

static void NetDev_MII_Wr_Stellaris_Eth0 				(NET_IF *pif,
														 CPU_INT08U phy_addr,
														 CPU_INT08U reg_addr,
														 CPU_INT16U data,
														 NET_ERR *perr);
														
							/* FNCT'S COMMON TO DMA BASED DRV'S --------- */
static  void  NetDev_RxDescInit_Stellaris_Eth0         	(NET_IF             *pif,
														 NET_ERR            *perr);

static  void  NetDev_RxDescPtrCurInc_Stellaris_Eth0    	(NET_IF             *pif);

static  void  NetDev_TxDescInit_Stellaris_Eth0         	(NET_IF             *pif,
														 NET_ERR            *perr);

static  void  NetDev_RxDescFreeAll_Stellaris_Eth0      	(NET_IF             *pif,
														 NET_ERR            *perr);

/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      NETWORK DEVICE DRIVER API
*
* Note(s) : (1) Device driver API structures are used by applications during calls to NetIF_Add().  This
*               API structure allows higher layers to call specific device driver functions via function
*               pointer instead of by name.  This enables the network protocol suite to compile & operate
*               with multiple device drivers.
*
*           (2) In most cases, the API structure provided below SHOULD suffice for most device drivers
*               exactly as is with the exception that the API structure's name which MUST be unique &
*               SHOULD clearly identify the device being implemented.
*
*               The API structure MUST also be externally declared in the device driver header file
*               ('net_dev_&&&.h') with the exact same name & type.
*********************************************************************************************************
*/
																									/* Stellaris dev API fnct ptrs :    */
const NET_DEV_API_ETHER NetDev_API_Stellaris_Eth0={ NetDev_Init_Stellaris_Eth0,                     /*   Init/add                       */
													NetDev_Start_Stellaris_Eth0,                    /*   Start                          */
													NetDev_Stop_Stellaris_Eth0,                     /*   Stop                           */
													NetDev_Rx_Stellaris_Eth0,                       /*   Rx                             */
													NetDev_Tx_Stellaris_Eth0,                       /*   Tx                             */
#if  (NET_VERSION >= 20600u)
													NetDev_AddrMulticastAdd_Stellaris_Eth0,         /*   Multicast addr add             */
													NetDev_AddrMulticastRemove_Stellaris_Eth0,      /*   Multicast addr remove          */
#endif
													NetDev_ISR_Handler_Stellaris_Eth0,              /*   ISR handler                    */
													NetDev_IO_Ctrl_Stellaris_Eth0,                  /*   I/O ctrl                       */
#if ((NET_VERSION <  20600u) && \
     (NET_VERSION >= 205u))
													NetDev_AddrMulticastAdd_Stellaris_Eth0,         /*   Multicast addr add             */
													NetDev_AddrMulticastRemove_Stellaris_Eth0,      /*   Multicast addr remove          */
#endif
													NetDev_MII_Rd_Stellaris_Eth0,                   /*   Phy reg rd                     */
													NetDev_MII_Wr_Stellaris_Eth0                    /*   Phy reg wr                     */
};



/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_Init()
*
* Description : (1) Initialize Network Driver Layer :
*
*                   (a) Initialize required clock sources
*                   (b) Initialize external interrupt controller
*                   (c) Initialize external GPIO controller
*                   (d) Initialize driver state variables
*                   (e) Allocate memory for device DMA descriptors
*                   (f) Initialize additional device registers
*                       (1) (R)MII mode / Phy bus type
*                       (2) Disable device interrupts
*                       (3) Disable device receiver and transmitter
*                       (4) Other necessary device initialization
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE            No Error.
*                           NET_DEV_ERR_INIT            General initialization error.
*                           NET_BUF_ERR_POOL_MEM_ALLOC  Memory allocation failed.
*                           NET_DEV_ERR_INVALID_CFG     Invalid device configuration.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_IF_Add() via 'pdev_api->Init()'.
*
* Note(s)     : (2) The application developer SHOULD define NetDev_CfgClk() within net_bsp.c
*                   in order to properly enable clocks for specified network interface.  In
*                   some cases, a device may require clocks to be enabled for BOTH the device
*                   and accessory peripheral modules such as GPIO.  A call to this function
*                   MAY need to occur BEFORE any device register accesses are made.  In the
*                   event that a device does NOT require any external clocks to be enabled,
*                   it is recommended that the device driver still call the NetBSP fuction
*                   which may in turn leave the section for the specific interface number
*                   empty.
*
*               (3) The application developer SHOULD define NetDev_CfgIntCtrl() within net_bsp.c
*                   in order to properly enable interrupts on an external or CPU integrated
*                   interrupt controller.  Interrupt sources that are specific to the DEVICE
*                   hardware MUST NOT be initialized from within NetDev_CfgIntCtrl() and
*                   SHOULD only be modified from within the device driver.
*
*                   (a) External interrupt sources are cleared within the NetBSP first level
*                       ISR handler either before or after the call to the device driver ISR
*                       handler function.  The device driver ISR handler function SHOULD only
*                       clear the device specific interrupts and NOT external or CPU interrupt
*                       controller interrupt sources.
*
*               (4) The application developer SHOULD define NetDev_CfgGPIO() within net_bsp.c
*                   in order to properly configure any necessary GPIO necessary for the device
*                   to operate properly.  Micrium recommends defining and calling this NetBSP
*                   function even if no additional GPIO initialization is required.
*
*               (5) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*
*               (6) All device drivers that store instance specific data MUST declare all
*                   instance specific variables within the device data area defined above.
*$PAGE*
*               (7) Drivers SHOULD validate device configuration values and set *perr to
*                   NET_DEV_ERR_INVALID_CFG if unacceptible values have been specified. Fields
*                   of interest generally include, but are not limited to :
*
*                   (a) pdev_cfg->RxBufAlignOctets
*                   (b) pdev_cfg->RxBufLargeSize
*
*               (8) Many DMA devices may generate only one interrupt for several ready receive
*                   descriptors.  In order to accommodate this, it is recommended that all DMA
*                   based drivers count the number of ready receive descriptors during the
*                   receive event and signal the receive task accordingly ONLY for those
*                   NEW descriptors which have not yet been accounted for.  Each time a
*                   descriptor is processed (or discarded) the count for acknowledged and
*                   unprocessed frames should be decremented by 1.  This function initializes the
*                   acknowledged receive descriptor count to 0.
*
*              (9) Descriptors are typically required to be contiguous in memory.  Allocation of
*                   descriptors MUST occur as a single contigous block of memory.  The driver may
*                   use pointer arithmetic to sub-divide and traverse the descriptor list.
*
*              (10) NetDev_Init() should exit with :
*
*                   (a) All device interrupt source disabled. External interrupt controllers
*                       should however be ready to accept interrupt requests.
*                   (b) All device interrupt sources cleared.
*                   (c) Both the receiver and transmitter disabled.
*
*              (11) Some drivers MAY require knowledge of the Phy configuration in order
*                   to properly configure the MAC with the correct Phy bus mode, speed and
*                   duplex settings.  If a driver requires access to the Phy configuration,
*                   then the driver MUST validate the pif->Phy_Cfg pointer by checking for
*                   a NULL pointer BEFORE attempting to access members of the Phy
*                   configuration structure.  Phy configuration fields of interest generally
*                   include, but are  not limited to :
*
*                   (a) pphy_cfg->Type :
*
*                       (1) NET_PHY_TYPE_INT            Phy integrated with MAC.
*                       (2) NET_PHY_TYPE_EXT            Phy externally attached to MAC.
*
*                   (b) pphy_cfg->BusMode :
*
*                       (1) NET_PHY_BUS_MODE_MII        Phy bus mode configured to MII.
*                       (2) NET_PHY_BUS_MODE_RMII       Phy bus mode configured to RMII.
*                       (3) NET_PHY_BUS_MODE_SMII       Phy bus mode configured to SMII.
*
*                   (c) pphy_cfg->Spd :
*
*                       (1) NET_PHY_SPD_0               Phy link speed unknown or NOT linked.
*                       (2) NET_PHY_SPD_10              Phy link speed configured to  10   mbit/s.
*                       (3) NET_PHY_SPD_100             Phy link speed configured to  100  mbit/s.
*                       (4) NET_PHY_SPD_1000            Phy link speed configured to  1000 mbit/s.
*                       (5) NET_PHY_SPD_AUTO            Phy link speed configured for auto-negotiation.
*
*                   (d) pphy_cfg->Duplex :
*
*                       (1) NET_PHY_DUPLEX_UNKNOWN      Phy link duplex unknown or link not established.
*                       (2) NET_PHY_DUPLEX_HALF         Phy link duplex configured to  half duplex.
*                       (3) NET_PHY_DUPLEX_FULL         Phy link duplex configured to  full duplex.
*                       (4) NET_PHY_DUPLEX_AUTO         Phy link duplex configured for auto-negotiation.
*********************************************************************************************************
*/
/*$PAGE*/
static void NetDev_Init_Stellaris_Eth0	(NET_IF *pif,
										 NET_ERR *perr)
{
#if (NET_VERSION >= 21000u)
    NET_DEV_BSP_ETHER  *pdev_bsp;
#endif
    NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    NET_BUF_SIZE        buf_size_max;
    NET_BUF_SIZE        buf_ix;
    CPU_SIZE_T          reqd_octets;
    CPU_SIZE_T          nbytes;
    LIB_ERR             lib_err;


                                                                /* -------- OBTAIN REFERENCE TO CFGs/REGs/BSP --------- */
    pdev_cfg = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;

#if (NET_VERSION >= 21000u)
    pdev_bsp = (NET_DEV_BSP_ETHER *)pif->Dev_BSP;
#endif

                                                                /* --------------- VALIDATE DEVICE CFG ---------------- */
                                                                /* Validate Rx buf size.                                */
    if ((pdev_cfg->RxBufLargeSize   % RX_BUF_SIZE_MULT   != 0u)) {
        *perr = NET_DEV_ERR_INVALID_CFG;                        /* Rx buf's MUST be a mult of 16 octets.                */
         return;
    }
                                                                /* Validate Rx buf alignment.                           */
    if ((pdev_cfg->RxBufAlignOctets % RX_DESC_BUF_ALIGN) != 0u) {
        *perr = NET_DEV_ERR_INVALID_CFG;
         return;
    }

#if (NET_VERSION >= 21000u)                                     /* Validate Rx buf ix offset.                           */
    if (pdev_cfg->RxBufIxOffset != 0u) {
       *perr = NET_DEV_ERR_INVALID_CFG;
        return;
    }
#endif
                                                                /* Validate Rx buf size.                                */
#if (NET_VERSION >= 21000u)
    buf_ix       = NET_IF_IX_RX;
#else
    buf_ix       = NET_IF_RX_IX;
#endif
    buf_size_max = NetBuf_GetMaxSize((NET_IF_NBR     )pif->Nbr,
                                     (NET_TRANSACTION)NET_TRANSACTION_RX,
                                     (NET_BUF       *)0,
                                     (NET_BUF_SIZE   )buf_ix);
#if (NET_VERSION >= 21000u)
    if (buf_size_max < NET_IF_ETHER_FRAME_MAX_SIZE) {
       *perr = NET_DEV_ERR_INVALID_CFG;
        return;
    }
#else
   (void)&buf_size_max;
#endif

                                                                /* Validate Tx buf alignment.                           */
    if ((pdev_cfg->TxBufAlignOctets % TX_DESC_BUF_ALIGN != 0)) {
        *perr = NET_DEV_ERR_INVALID_CFG;                        /* Tx buf's MUST be aligned to 1 16 BYTE boundary.      */
         return;
    }

#if (NET_VERSION >= 21000u)                                     /* Validate Tx buf ix offset.                           */
    if (pdev_cfg->TxBufIxOffset != 0u) {
       *perr = NET_DEV_ERR_INVALID_CFG;
        return;
    }
#endif


                                                                /* ------------- ALLOCATE DEV DATA AREA --------------- */
    pif->Dev_Data = Mem_HeapAlloc((CPU_SIZE_T  ) sizeof(NET_DEV_DATA),
                                  (CPU_SIZE_T  ) 4,
                                  (CPU_SIZE_T *)&reqd_octets,
                                  (LIB_ERR    *)&lib_err);
    if (pif->Dev_Data == (void *)0) {
       *perr = NET_DEV_ERR_MEM_ALLOC;
        return;
    }

    pdev_data = (NET_DEV_DATA *)pif->Dev_Data;


                                                                /* ------------ ALLOCATE DESCRIPTOR LISTS ------------- */

    nbytes = pdev_cfg->RxDescNbr * sizeof(DEV_DESC);            /* Allocate Rx desc buf's.                              */
    pdev_data->RxBufDescPtrStart = (DEV_DESC *)Mem_HeapAlloc((CPU_SIZE_T  ) nbytes,
                                                             (CPU_SIZE_T  ) pdev_cfg->RxBufAlignOctets,
                                                             (CPU_SIZE_T *)&reqd_octets,
                                                             (LIB_ERR    *)&lib_err);
    if (pdev_data->RxBufDescPtrStart == (void *)0) {
       *perr = NET_DEV_ERR_MEM_ALLOC;
        return;
    }

    nbytes = pdev_cfg->TxDescNbr * sizeof(DEV_DESC);            /* Allocate Tx desc's.                                  */
    pdev_data->TxBufDescPtrStart = (DEV_DESC *)Mem_HeapAlloc((CPU_SIZE_T  ) nbytes,
                                                             (CPU_SIZE_T  ) pdev_cfg->TxBufAlignOctets,
                                                             (CPU_SIZE_T *)&reqd_octets,
                                                             (LIB_ERR    *)&lib_err);
    if (pdev_data->TxBufDescPtrStart == (void *)0) {
       *perr = NET_DEV_ERR_MEM_ALLOC;
        return;
    }



#if (NET_VERSION >= 21000u)                                     /* Enable module clks (GPIO & EMAC) [see Note #2].      */
    pdev_bsp->CfgClk(pif, perr);
#else
    NetDev_CfgClk(pif->Nbr, perr);
#endif
    if (*perr != NET_DEV_ERR_NONE) {
        return;
    }


    SysCtlPeripheralReset(SYSCTL_PERIPH_UDMA);					/* Reset uDMA to known defaults							*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);					/* Enable clock gating to uDMA							*/
    uDMAEnable();												/* Enable the uDMA for operation						*/
    uDMAControlBaseSet(g_sDMAControlTable);						/* Set the 1024 byte aligned control table for uDMA 	*/
	
	EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | ETH_INT_MDIO |					/* Disable and clear ethernet interrupts*/
                                      ETH_INT_RXER | ETH_INT_RXOF |
                                      ETH_INT_TX | ETH_INT_TXER | ETH_INT_RX));

    EthernetIntClear(ETH_BASE, EthernetIntStatus(ETH_BASE, false));

	

#if (NET_VERSION >= 21000u)                                     /* Configure ext int ctrl'r (see Note #3).              */
    pdev_bsp->CfgIntCtrl(pif, perr);
#else
    NetDev_CfgIntCtrl(pif->Nbr, perr);
#endif
    if (*perr != NET_DEV_ERR_NONE) {
        return;
    }

#if (NET_VERSION >= 21000u)                                     /* Configure MAC / Phy GPIO (see Note #4).              */
    pdev_bsp->CfgGPIO(pif, perr);
#else
    NetDev_CfgGPIO(pif->Nbr, perr);
#endif
    if (*perr != NET_DEV_ERR_NONE) {
        return;
    }
/*$PAGE*/
                                                                /* -------------------- CFG MAC ----------------------- */

	
	EthernetInitExpClk	(ETH_BASE, SysCtlClockGet());
	EthernetConfigSet	(ETH_BASE, ETH_CFG_RX_BADCRCDIS | 		/* Disable RX of packets with bad CRC 					*/
								   ETH_CFG_RX_AMULEN |				/* Enable RX of multicast address 						*/
								   ETH_CFG_TX_DPLXEN |			/* Enable TX Duplex 									*/
								   ETH_CFG_TX_CRCEN |			/* Enable CRC Auto generation on TX 					*/
								   ETH_CFG_TX_PADEN );			/* Enable auto padding of TX packets up to minimum size */
																

																		/* ----------------- CFG DMA ------------------ */
    uDMAChannelAttributeDisable(UDMA_CHANNEL_ETH0TX, UDMA_ATTR_ALL);	/* Disable all TX Channel attributes			*/
    uDMAChannelControlSet(UDMA_CHANNEL_ETH0TX,							/* Setup control for TX DMA Channel				*/
                              UDMA_SIZE_32 | UDMA_SRC_INC_32 |
                              UDMA_DST_INC_NONE | UDMA_ARB_8);

	uDMAChannelAttributeDisable(UDMA_CHANNEL_ETH0RX, UDMA_ATTR_ALL);	/* Disable all RX Channel attributes			*/
    uDMAChannelControlSet(UDMA_CHANNEL_ETH0RX,							/* Setup control for RX DMA Channel				*/
                          UDMA_SIZE_32 | UDMA_SRC_INC_NONE |			
                          UDMA_DST_INC_32 | UDMA_ARB_8);

   *perr = NET_DEV_ERR_NONE;

}



/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_Start()
*
* Description : (1) Start network interface hardware :
*
*                   (a) Initialize transmit semaphore count
*                   (b) Initialize hardware address registers
*                   (c) Initialize receive and transmit descriptors
*                   (d) Clear all pending interrupt sources
*                   (e) Enable supported interrupts
*                   (f) Enable the transmitter and receiver
*                   (g) Start / Enable DMA if required
*
*
* Argument(s) : pif         Pointer to a network interface.
*               ---         Argument validated in NetIF_Start().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_DEV_ERR_NONE                Ethernet device successfully started.
*
*                                                               - RETURNED BY NetIF_AddrHW_SetHandler() : -
*                               NET_IF_ERR_NULL_PTR             Argument(s) passed a NULL pointer.
*                               NET_IF_ERR_NULL_FNCT            Invalid NULL function pointer.
*                               NET_IF_ERR_INVALID_CFG          Invalid/NULL API configuration.
*                               NET_IF_ERR_INVALID_STATE        Invalid network interface state.
*                               NET_IF_ERR_INVALID_ADDR         Invalid hardware address.
*                               NET_IF_ERR_INVALID_ADDR_LEN     Invalid hardware address length.
*
*                                                               ------- RETURNED BY RxDescInit() : --------
*                          !!!!
*
*                                                               ------- RETURNED BY TxDescInit() : --------
*                          !!!!
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_IF_Start() via 'pdev_api->Start()'.
*
* Note(s)     : (2) Setting the maximum number of frames queued for transmission is optional.  By
*                   default, all network interfaces are configured to block until the previous frame
*                   has completed transmission.  However, DMA based interfaces may have several
*                   frames configured for transmission before blocking is required. The number
*                   of queued transmit frames depends on the number of configured transmit
*                   descriptors.
*
*               (3) The physical hardware address should not be configured from NetDev_Init(). Instead,
*                   it should be configured from within NetDev_Start() to allow for the proper use
*                   of NetIF_Ether_HW_AddrSet(), hard coded hardware addresses from the device
*                   configuration structure, or auto-loading EEPROM's. Changes to the physical address
*                   only take effect when the device transitions from the DOWN to UP state.
*
*               (4) The device hardware address is set from one of the data sources below. Each source
*                   is listed in the order of precedence.
*
*                   (a) Device Configuration Structure       Configure a valid HW address during
*                                                                compile time.
*
*                                                            Configure either "00:00:00:00:00:00" or
*                                                                an empty string, "", in order to
*                                                                configure the HW address using using
*                                                                method (b).
*
*                   (b) NetIF_Ether_HW_AddrSet()             Call NetIF_Ether_HW_AddrSet() if the HW
*                                                                address needs to be configured via
*                                                                run-time from a different data
*                                                                source. E.g. Non auto-loading
*                                                                memory such as I2C or SPI EEPROM.
*                                                               (see Note #3).
*
*                   (c) Auto-Loading via EEPROM.             If neither options a) or b) are used,
*                                                                the IF layer will use the HW address
*                                                                obtained from the network hardware
*                                                                address registers.
*********************************************************************************************************
*/
/*$PAGE*/
static void NetDev_Start_Stellaris_Eth0 (NET_IF *pif,
										 NET_ERR *perr)
{
	NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    CPU_INT08U          hw_addr[NET_IF_ETHER_ADDR_SIZE];
    CPU_INT08U          hw_len;
    CPU_BOOLEAN         valid;
	CPU_INT32U			ulUser0, ulUser1;


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_cfg  = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;              /* Obtain ptr to the dev cfg struct.                    */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */


#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    pdev_data->StatRxCtrRx             = 0;
    pdev_data->StatRxCtrRxISR          = 0;
    pdev_data->StatTxCtrTxISR          = 0;
    pdev_data->StatRxCtrTaskSignalISR  = 0;
    pdev_data->StatRxCtrTaskSignalSelf = 0;
    pdev_data->StatRxCtrTaskSignalNone = 0;
#endif

#if (NET_CTR_CFG_ERR_EN  == DEF_ENABLED)
    pdev_data->ErrRxCtrDataAreaAlloc   = 0;
    pdev_data->ErrRxCtrROS             = 0;
    pdev_data->ErrRxCtrRBUS            = 0;
    pdev_data->ErrRxCtrRPSS            = 0;
    pdev_data->ErrRxCtrDescES          = 0;
    pdev_data->ErrRxCtrDescOWN         = 0;
    pdev_data->ErrRxCtrRunt            = 0;
    pdev_data->ErrRxTxCtrFatalBusErr   = 0;
    pdev_data->ErrRxCtrTaskSignalFail  = 0;
#endif

    NetOS_Dev_CfgTxRdySignal(pif->Nbr,                          /* See note #3.                                         */
                             pdev_cfg->TxDescNbr,
                             perr);
                                                                /* ------------------ CFG HW ADDR --------------------- */
                                                                /* See Note(s) #1 and #2.                               */
    NetASCII_Str_to_MAC( pdev_cfg->HW_AddrStr,
                        &hw_addr[0],
                         perr);

    NetIF_AddrHW_SetHandler((NET_IF_NBR  )pif->Nbr,             /* See Note #2a.                                        */
                            (CPU_INT08U *)hw_addr,
                            (CPU_INT08U  )sizeof(hw_addr),
                            (NET_ERR    *)perr);

    if (*perr == NET_IF_ERR_NONE) {
		EthernetMACAddrSet(ETH_BASE, hw_addr);					/* Set HW MAC addr. from the config data provided       */

    } else {                                                    /* See Note #2b.                                        */
                                                                /* Obtain a copy of the cur IF layer HW addr.           */
        hw_len = sizeof(hw_addr);
        NetIF_AddrHW_GetHandler(pif->Nbr, &hw_addr[0], &hw_len, perr);
        if (*perr == NET_IF_ERR_NONE) {                         /* Check if the IF HW addr has been user cfg'd.         */
             valid = NetIF_AddrHW_IsValidHandler(pif->Nbr, &hw_addr[0], perr);
        } else {
             valid = DEF_NO;
        }

/*$PAGE*/
        if (valid == DEF_YES) {
            EthernetMACAddrSet(ETH_BASE, hw_addr);	
			
        } else {                                                /* See Note #2c.                                        */
            FlashUserGet(&ulUser0, &ulUser1);					/* Get Stellaris dev style kit MAC from user flash		*/
			
			//
			// Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
			// address needed to program the hardware registers, then program the MAC
			// address into the Ethernet Controller registers.
			//
			hw_addr[0] = ((ulUser0 >>  0) & 0xff);
			hw_addr[1] = ((ulUser0 >>  8) & 0xff);
			hw_addr[2] = ((ulUser0 >> 16) & 0xff);
			hw_addr[3] = ((ulUser1 >>  0) & 0xff);
			hw_addr[4] = ((ulUser1 >>  8) & 0xff);
			hw_addr[5] = ((ulUser1 >> 16) & 0xff);
			
																/* Cfg the IF to use the auto-loaded HW addr.           */
            NetIF_AddrHW_SetHandler((NET_IF_NBR  )pif->Nbr,     /* See Note #2a.                                        */
                                    (CPU_INT08U *)hw_addr,
                                    (CPU_INT08U  )sizeof(hw_addr),
                                    (NET_ERR    *)perr);
            if (*perr != NET_IF_ERR_NONE) {                     /* No valid HW addr specified, return with err.         */
                return;
            }
			else {												/* HW addr from user flash was valid so load it to MAC	*/
				EthernetMACAddrSet(ETH_BASE, hw_addr);
			}
        }
    }

    NetDev_RxDescInit_Stellaris_Eth0(pif, perr);                               /* Init Rx desc's.                                      */
    if (*perr != NET_DEV_ERR_NONE) {
         return;
    }

    NetDev_TxDescInit_Stellaris_Eth0(pif, perr);                               /* Init Tx desc'c.                                      */
    if (*perr != NET_DEV_ERR_NONE) {
         return;
    }

	
	EthernetEnable(ETH_BASE);

    uDMAChannelEnable(UDMA_CHANNEL_ETH0RX);
	uDMAChannelEnable(UDMA_CHANNEL_ETH0TX);

    BSP_IntVectSet(BSP_INT_ID_ETH, NetDev_ISR_Handler);
	IntEnable(INT_ETH);
	EthernetIntEnable(ETH_BASE, ETH_INT_RX | ETH_INT_TX);
	EthernetIntEnable(ETH_BASE, ETH_INT_RXOF | ETH_INT_RXER | ETH_INT_TXER);
	
    (void)&pdev_data;                                            /* Prevent compiler warning.                            */

   *perr = NET_DEV_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_Stop()
*
* Description : (1) Shutdown network interface hardware :
*
*                   (a) Disable the receiver and transmitter
*                   (b) Disable receive and transmit interrupts
*                   (c) Clear pending interrupt requests
*                   (d) Free ALL receive descriptors (Return ownership to hardware)
*                   (e) Deallocate ALL transmit buffers
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE    No Error
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_IF_Stop() via 'pdev_api->Stop()'.
*
* Note(s)     : (2) (a) (1) It is recommended that a device driver should only post all currently-used,
*                           i.e. not-fully-transmitted, transmit buffers to the network interface transmit
*                           deallocation queue.
*
*                       (2) However, a driver MAY attempt to post all queued &/or transmitted buffers.
*                           The network interface transmit deallocation task will silently ignore any
*                           unknown or duplicate transmit buffers.  This allows device drivers to
*                           indiscriminately & easily post all transmit buffers without determining
*                           which buffers have NOT yet been transmitted.
*
*                   (b) (1) Device drivers should assume that the network interface transmit deallocation
*                           queue is large enough to post all currently-used transmit buffers.
*
*                       (2) If the transmit deallocation queue is NOT large enough to post all transmit
*                           buffers, some transmit buffers may/will be leaked/lost.
*
*               (3) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*********************************************************************************************************
*/
static void NetDev_Stop_Stellaris_Eth0 	(NET_IF *pif,
										 NET_ERR *perr)
{
    NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    CPU_INT08U          i;
    NET_ERR             err;


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_cfg  = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;              /* Obtain ptr to the dev cfg struct.                    */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */

    EthernetDisable(ETH_BASE);
	uDMAChannelDisable(UDMA_CHANNEL_ETH0RX);
	uDMAChannelDisable(UDMA_CHANNEL_ETH0TX);
	
                                                                /* ------------- DISABLE & CLEAR INT'S ---------------- */
    EthernetIntDisable(ETH_BASE, (ETH_INT_PHY 		|			/* Disable and clear ethernet interrupts				*/
									ETH_INT_MDIO 	|	
                                    ETH_INT_RXER 	|
									ETH_INT_RXOF 	|
                                    ETH_INT_TX 		|
									ETH_INT_TXER 	|
									ETH_INT_RX));

    EthernetIntClear(ETH_BASE, EthernetIntStatus(ETH_BASE, false));
	
                                                                /* --------------- FREE RX DESCRIPTORS ---------------- */
    NetDev_RxDescFreeAll_Stellaris_Eth0(pif, perr);
    if (*perr != NET_DEV_ERR_NONE) {
         return;
    }

                                                                /* ------------ FREE USED TX DESCRIPTORS -------------- */
    pdesc = &pdev_data->TxBufDescPtrStart[0];
    for (i = 0; i < pdev_cfg->TxDescNbr; i++) {                 /* Dealloc ALL tx bufs (see Note #2a2).                 */
        NetOS_IF_TxDeallocTaskPost((CPU_INT08U *)pdesc->Addr, &err);
       (void)&err;                                              /* Ignore possible dealloc err (see Note #2b2).         */
        pdesc++;
    }


   *perr = NET_DEV_ERR_NONE;
}

																	

/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_Rx()
*
* Description : (1) This function returns a pointer to the received data to the caller :
*                   (a) Determine which receive descriptor caused the interrupt
*                   (b) Obtain pointer to data area to replace existing data area
*                   (c) Reconfigure descriptor with pointer to new data area
*                   (d) Set return values.  Pointer to received data area and size
*                   (e) Update current receive descriptor pointer
*                   (f) Increment counters.
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               p_data  Pointer to pointer to received DMA data area. The received data
*                       area address should be returned to the stack by dereferencing
*                       p_data as *p_data = (address of receive data area).
*
*               size    Pointer to size. The number of bytes received should be returned
*                       to the stack by dereferencing size as *size = (number of bytes).
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE            No Error
*                           NET_DEV_ERR_RX              Generic Rx error.
*                           NET_DEV_ERR_INVALID_SIZE    Invalid Rx frame size.
*                           NET_BUF error codes         Potential NET_BUF error codes
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxPkt() via 'pdev_api->Rx()'.
*
* Note(s)     : (1)	Stellaris uDMA on ethernet reciece does not have advance knowldege of the
*					packet size.  Therefore the RX ISR first alerts this function that a RX has
*					occurred.  Then the size of packet is read from the MAC.  Then a uDMA transfer
*					is initiated by software to transfer the rest of the packet to the NET stack.
*
*					Stellaris MAC puts the frame lenght in the first two bytes read from the FIFO.
*					To maintain 32 bit alignment for DMA we have an offset in net_cfg of 2 bytes
*					for this length data. We then manually store the next two bytes obtained via
*					word read of the FIFO into the buffer.  All subsequent reads from FIFO are done
*					32 bits at a time by the DMA.
*
*					EXAMPLE:
*						Step 1: Read first 32 bit word from FIFO
*						Step 2: lower 16 bits is frame length
*						Step 3: upper 16 bits is frame data that must be kept.
*						Step 4: starting at 2nd word of FIFO read all of FIFO via 32 bit aligned uDMA access

*					
*					(a) The maximum uDMA transfer size is the frame length - the two bytes already
*						read truncated to the nearest whole word length. lRemainder then becomes the
*						number of bytes to read one at a time after uDMA gets all the whole words
*						that it can.
*
*					(b) For optimum efficiency the alignment of the network buffers is set in
*						net_cfg.c to 32 bit alignment.  This allows best DMA performance.
*						
*
*
*				(2) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*
*               (3) If a receive error occurs and the descriptor is invalid then the function
*                   SHOULD return 0 for the size, a NULL pointer to the data area AND an
*                   error equal to NET_DEV_ERR_RX.
*
*                   (a) If the next expected ready / valid descriptor is NOT owned by
*                       software, then there is descriptor pointer corruption and the
*                       driver should NOT increment the current receive descriptor
*                       pointer.
*
*                   (b) If the descriptor IS valid, but an error is indicated within
*                       the descriptor status bits, or length field, then the driver
*                       MUST increment the current receive descriptor pointer and discard
*                       the received frame.
*
*                   (c) If a new data area is unavailable, the driver MUST increment
*                       the current receive descriptor pointer and discard the received
*                       frame.  This will invoke the DMA to re-use the existing configured
*                       data area.
*********************************************************************************************************
*/
/*$PAGE*/
static void NetDev_Rx_Stellaris_Eth0 	(NET_IF *pif,
										 CPU_INT08U **p_data,
										 CPU_INT16U *size,
										 NET_ERR *perr)
{
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    CPU_INT08U         *pbuf_new;
	
	CPU_INT32U			ulTemp;
	CPU_INT08U			*pucBuffer;
	CPU_INT32S			lTempLen, lFrameLen;

	                                                            /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */
    pdesc     = (DEV_DESC          *)pdev_data->RxBufDescPtrCur;/* Obtain ptr to next ready desc.                       */


	pucBuffer = (CPU_INT08U *)pdesc->Addr;						/* where to put the incoming data						*/

	
	
	ulTemp = HWREG(ETH_BASE + MAC_O_DATA);						/* Read Word 0 from FIFO  See Note 1					*/
	lFrameLen = (long)(ulTemp & 0xFFFF);						/* use Word 0 to set frame length						*/
																/* first two bytes of p_data reserved for length field	*/
	pucBuffer[2] = (unsigned char) ((ulTemp >> 16) & 0xFF);		/* use Word 0 to store first two bytes of dest addr		*/
	pucBuffer[3] = (unsigned char) ((ulTemp >> 24) & 0xFF);	
	
	lTempLen = (lFrameLen + 1) >> 2;							/* # of 32b words in FIFO, may copy a few extra bytes	*/
	
	HWREGBITW(&g_ulFlags, FLAG_RXPKTPEND) = 1;					/* Mark the receive as pending 							*/

	uDMAChannelTransferSet(UDMA_CHANNEL_ETH0RX, UDMA_MODE_AUTO,	/* Set up the uDMA Transfer of remaining bytes			*/
                           (void *)(ETH_BASE + MAC_O_DATA),
                           &pucBuffer[4], lTempLen);			/* start loading buffer at first unused words			*/
    uDMAChannelEnable(UDMA_CHANNEL_ETH0RX);	
	
	uDMAChannelRequest(UDMA_CHANNEL_ETH0RX);				/* Issue Software Request for uDMA Transfer					*/
	
	while(HWREGBITW(&g_ulFlags, FLAG_RXPKTPEND) == 1) {
															/* Do nothing waiting for recieve ISR to clear pending flag */
	}

                                                                /* --------- OBTAIN PTR TO NEW DMA DATA AREA ---------- */
    pbuf_new = NetBuf_GetDataPtr((NET_IF        *)pif,
                                 (NET_TRANSACTION)NET_TRANSACTION_RX,
#if (NET_VERSION >= 21000u)
                                 (NET_BUF_SIZE   )NET_IF_ETHER_FRAME_MAX_SIZE,
                                 (NET_BUF_SIZE   )NET_IF_IX_RX,
                                 (NET_BUF_SIZE  *)0,
#else
                                 (NET_BUF_SIZE   )pdev_cfg->RxBufLargeSize,
                                 (NET_BUF_SIZE   )0u,
#endif
                                 (NET_BUF_SIZE  *)0,
                                 (NET_TYPE      *)0,
                                 (NET_ERR       *)perr);


    if (*perr != NET_BUF_ERR_NONE) {                            /* If unable to get a buffer.                           */
        NetDev_RxDescPtrCurInc_Stellaris_Eth0(pif);                            /* Free cur desc.                                       */
        NET_CTR_ERR_INC(pdev_data->ErrRxCtrDataAreaAlloc);
       *size   = (CPU_INT16U  )0;
       *p_data = (CPU_INT08U *)0;
        return;
    }

   *size          =  lFrameLen;                                 /* Return the size of the recv'd frame.                 */
   *p_data        =  pucBuffer;                  				/* Return a ptr to the newly recv'd data.               */

    pdesc->Addr   =  pbuf_new;                                  /* Update the desc to point to a new data area          */
    NetDev_RxDescPtrCurInc_Stellaris_Eth0(pif);                                /* Free the current desc.                               */

    NET_CTR_STAT_INC(pdev_data->StatRxCtrRx);

    *perr = NET_DEV_ERR_NONE;

}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_Tx()
*
* Description : (1) This function transmits the specified data :
*
*                   (a) Check if the transmitter is ready.
*                   (b) Configure the next transmit descriptor for pointer to data and data size.
*                   (c) Issue the transmit command.
*                   (d) Increment pointer to next transmit descriptor
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE        No Error
*                           NET_DEV_ERR_TX_BUSY     No Tx descriptors available
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_TxPkt() via 'pdev_api->Tx()'.
*
* Note(s)     : (2) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*
*               (3) Care should be taken to avoid skipping transmit descriptors while selecting
*                   the next available descriptor.  Software MUST track the descriptor which
*                   is expected to generate the next transmit complete interrupt.  Skipping
*                   descriptors, unless carefully accounted for, may make it difficult to
*                   know which descriptor will complete transmission next.  Some device
*                   drivers may find it useful to adjust pdev_data->TxBufDescCompPtr after
*                   having selected the next available transmit descriptor.
*
*               (4)  The T01 descriptor bit is reserved for software use ONLY. This bit is set by software
*                    during transmission to indicate that the data ready bit for the descriptor is being set.
*                    When the DMA hardware completes transmission of the descriptor data, the ready bit is
*                    cleared and the T01 bit remains set. The transmit complete ISR handler then detects the
*                    cleared ready bit and set T01 bit and determines that the descriptor has completed
*                    transmission. The T01 bit is then cleared by the ISR handler.
*********************************************************************************************************
*/
static void NetDev_Tx_Stellaris_Eth0 	(NET_IF *pif,
										 CPU_INT08U *p_data,
										 CPU_INT16U size,
										 NET_ERR *perr)
{
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
	CPU_INT32U			ulTemp;
                                                                /* ------- OBTAIN REFERENCE TO DEVICE REGISTERS ------- */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */
    pdesc     = (DEV_DESC          *)pdev_data->TxBufDescPtrCur;


    pdesc->Addr                =  p_data;                       /* Cfg desc with Tx data area addr.                     */
    pdesc->Len                 =  size;

	ulTemp = size - 14;											/* MAC wants only payload size not total sized			*/
	p_data[0] = ulTemp & 0xFF;
	p_data[1] = (ulTemp & 0xFF00) >> 8;
	
	uDMAChannelTransferSet(UDMA_CHANNEL_ETH0TX, UDMA_MODE_AUTO,
                           p_data, (void *)(ETH_BASE + MAC_O_DATA),
                           (size + 5)>>2);
	
	uDMAChannelEnable(UDMA_CHANNEL_ETH0TX);
	
	uDMAChannelRequest(UDMA_CHANNEL_ETH0TX);
	
	while((HWREGBITW(&g_ulFlags, FLAG_TXPKT) == 1) &&			/* Do nothing waiting for the TX To complete.  			*/
          EthernetSpaceAvail(ETH_BASE)) {				
	}
	
	
	pdev_data->TxBufDescPtrCur =  pdesc->Next;                  /* Increment for the next transmit                      */

   *perr = NET_DEV_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetDev_RxDescFreeAll_Stellaris_Eth0()
*
* Description : (1) This function returns the descriptor memory block and descriptor data area
*                   memory blocks back to their respective memory pools :
*
*                   (a) Free Rx descriptor data areas
*                   (b) Free Rx descriptor memory block
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE        No error
*                           NET_IF_MGR_ERR_nnnn     Various Network Interface management error codes
*                           NET_BUF_ERR_nnn         Various Network buffer error codes
* Return(s)   : none.
*
* Caller(s)   : NetDev_Stop().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetDev_RxDescFreeAll_Stellaris_Eth0(NET_IF   *pif,
                                   NET_ERR  *perr)
{
    NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    CPU_INT16U          i;
    CPU_INT08U         *pdesc_data;


                                                                /* ------- OBTAIN REFERENCE TO DEVICE CFG/DATA -------- */
    pdev_cfg  = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;              /* Obtain ptr to the dev cfg struct.                    */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */

                                                                /* ------------- FREE RX DESC DATA AREAS -------------- */
    pdesc  = pdev_data->RxBufDescPtrStart;
    for (i = 0; i < pdev_cfg->RxDescNbr; i++) {                 /* Free Rx desc ring.                                   */
        pdesc_data = (CPU_INT08U *)(pdesc->Addr);
        NetBuf_FreeBufDataAreaRx(pif->Nbr, pdesc_data);         /* Return data area to Rx data area pool.               */
        pdesc++;
    }

   *perr = NET_DEV_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetDev_RxDescInit_Stellaris_Eth0()
*
* Description : This function initializes the Rx descriptor list for the specified interface.
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE        No error
*                           NET_IF_MGR_ERR_nnnn     Various Network Interface management error codes
*                           NET_BUF_ERR_nnn         Various Network buffer error codes
* Return(s)   : none.
*
* Caller(s)   : NetDev_Init().
*
* Note(s)     : (1) Memory allocation for the descriptors and receive buffers MUST be performed BEFORE
*                   calling this function. This ensures that multiple calls to this function do NOT
*                   allocate additional memory to the interface and that the Rx descriptors may be
*                   safely re-initialized by calling this function.
*********************************************************************************************************
*/

static  void  NetDev_RxDescInit_Stellaris_Eth0 (NET_IF   *pif,
                                 NET_ERR  *perr)
{
//$$$$ DGT TODO REVIEW THIS CODE.
    NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    CPU_INT16U          i;


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_cfg  = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;              /* Obtain ptr to the dev cfg struct.                    */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */


                                                                /* ------------- INIT DESCRIPTOR PTRS  ---------------- */
    pdesc                        = (DEV_DESC *)pdev_data->RxBufDescPtrStart;
    pdev_data->RxBufDescPtrStart = (DEV_DESC *)pdesc;
    pdev_data->RxBufDescPtrCur   = (DEV_DESC *)pdesc;
    pdev_data->RxBufDescPtrEnd   = (DEV_DESC *)pdesc + (pdev_cfg->RxDescNbr - 1);

    for (i = 0; i < pdev_cfg->RxDescNbr; i++) {                 /* Init Rx descriptor ring.                             */
        pdesc->Len    = pdev_cfg->RxBufLargeSize;

        pdesc->Addr   = NetBuf_GetDataPtr((NET_IF        *)pif,
                                          (NET_TRANSACTION)NET_TRANSACTION_RX,
#if (NET_VERSION >= 21000u)
                                          (NET_BUF_SIZE   )NET_IF_ETHER_FRAME_MAX_SIZE,
                                          (NET_BUF_SIZE   )NET_IF_IX_RX,
                                          (NET_BUF_SIZE  *)0,
#else
                                          (NET_BUF_SIZE   )pdev_cfg->RxBufLargeSize,
                                          (NET_BUF_SIZE   )0u,
#endif
                                          (NET_BUF_SIZE  *)0,
                                          (NET_TYPE      *)0,
                                          (NET_ERR       *)perr);
        if (*perr != NET_BUF_ERR_NONE) {
             return;
        }

        pdesc->Next = (DEV_DESC *)(pdesc + 1);                  /* Point to next desc in list.                          */
        pdesc++;
    }

    pdesc--;
    pdesc->Next    = (DEV_DESC *)pdev_data->RxBufDescPtrStart;  /* Last desc points to first desc.                      */

   *perr = NET_DEV_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetDev_RxDescPtrCurInc_Stellaris_Eth0()
*
* Description : (1) Increment current descriptor pointer to next receive descriptor :
*
*                   (a) Return ownership of current descriptor back to DMA.
*                   (b) Point to the next descriptor.
*
* Argument(s) : pif     Pointer to interface requiring service.
*               ---
*
* Return(s)   : none.
*
* Caller(s)   : NetDev_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetDev_RxDescPtrCurInc_Stellaris_Eth0 (NET_IF  *pif)
{
    //$$$$ DGT TODO REVIEW THIS CODE.
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    NET_ERR             err;


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */



    pdesc                      = pdev_data->RxBufDescPtrCur;
    pdev_data->RxBufDescPtrCur = pdesc->Next;

    pdesc = pdesc->Next;                                        /* Check if there are add'l frames to Rx.               */
    NetOS_IF_RxTaskSignal(pif->Nbr, &err);                  /* Signal Net IF RxQ Task for each new rdy Desc.        */
    switch (err) {
        case NET_IF_ERR_NONE:
             NET_CTR_STAT_INC(pdev_data->StatRxCtrTaskSignalSelf);
             break;


        case NET_IF_ERR_RX_Q_FULL:
        case NET_IF_ERR_RX_Q_SIGNAL_FAULT:
        default:
             NET_CTR_ERR_INC(pdev_data->ErrRxCtrTaskSignalFail);
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetDev_TxDescInit_Stellaris_Eth0()
*
* Description : (1) This function initializes the Tx descriptor list for the specified interface :
*
*                   (a) Obtain reference to the Tx descriptor(s) memory block
*                   (b) Initialize Tx descriptor pointers
*                   (c) Obtain Tx descriptor data areas
*                   (d) Initialize hardware registers
*
* Argument(s) : pif     Pointer to the interface requiring service.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NONE        No error
*                           NET_IF_MGR_ERR_nnnn     Various Network Interface management error codes
*                           NET_BUF_ERR_nnn         Various Network buffer error codes
* Return(s)   : none.
*
* Caller(s)   : NetDev_Start().
*
* Note(s)     : (2) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*
*               (3) All Tx descriptors are allocated as ONE memory block.  This removes the
*                   necessity to ensure that descriptor blocks are returned to the descriptor
*                   pool in the opposite order in which they were allocated; doing so would
*                   ensure that each memory block address was contiguous to the one before
*                   and after it.  If the descriptors are NOT contiguous, then software
*                   MUST NOT assign a pointer to the pool start address and use pointer
*                   arithmetic to navigate the descriptor list.  Since pointer arithmetic
*                   is a convenient way to navigate the descriptor list, ONE block is allocated
*                   and the driver uses pointer arithmetic to slice the block into descriptor
*                   sized units.
*********************************************************************************************************
*/

static  void  NetDev_TxDescInit_Stellaris_Eth0 (NET_IF   *pif,
                                 NET_ERR  *perr)
{
//$$$$ DGT TODO REVIEW THIS CODE.
    NET_DEV_CFG_ETHER  *pdev_cfg;
    NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    CPU_INT16U          i;


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_cfg  = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;              /* Obtain ptr to the dev cfg struct.                    */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */


                                                                /* -------------- INIT DESCRIPTOR PTRS  --------------- */
    pdesc                       = (DEV_DESC *)pdev_data->TxBufDescPtrStart;
    pdev_data->TxBufDescPtrComp = (DEV_DESC *)pdev_data->TxBufDescPtrStart;
    pdev_data->TxBufDescPtrCur  = (DEV_DESC *)pdev_data->TxBufDescPtrStart;

    for (i = 0; i < pdev_cfg->TxDescNbr; i++) {                 /* Init Tx desc ring.                                   */
        pdesc->Addr   =  0;
        pdesc->Len    =  0;

        pdesc->Next   = (DEV_DESC *)(pdesc + 1);                /* Point to next desc in list.                          */

        pdesc++;
    }

    pdesc--;
    pdesc->Next    = (DEV_DESC *)pdev_data->TxBufDescPtrStart;  /* Last desc wraps to first desc.                       */

    *perr = NET_DEV_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDev_AddrMulticastAdd()
*
* Description : Configure hardware address filtering to accept specified hardware address.
*
* Argument(s) : pif         Pointer to an Ethernet network interface.
*               ---         Argument validated in NetIF_AddrHW_SetHandler().
*
*               paddr_hw    Pointer to hardware address.
*               --------    Argument checked   in NetIF_AddrHW_SetHandler().
*
*               addr_len    Length  of hardware address.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_DEV_ERR_NONE                Hardware address successfully configured.
*
*                                                               - RETURNED BY NetUtil_32BitCRC_Calc() : -
*                               NET_UTIL_ERR_NULL_PTR           Argument 'pdata_buf' passed NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Argument 'len' passed equal to 0.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_AddrMulticastAdd() via 'pdev_api->AddrMulticastAdd()'.
*
* Note(s)     : (1) The device is capable of the following multicast address filtering techniques :
*                       (a) Perfect        filtering of ONE multicast address.
*                       (b) Imperfect hash filtering of 64  multicast addresses.
*                       (c) Promiscous non filtering.  Disable filtering of all received frames.
*
*               (2) This function implements the filtering mechanism described in 1b.
*********************************************************************************************************
*/

#if (NET_VERSION >= 205u)
static void NetDev_AddrMulticastAdd_Stellaris_Eth0 	(NET_IF *pif,
													 CPU_INT08U *paddr_hw,
													 CPU_INT08U addr_hw_len,
													 NET_ERR *perr)
{
#ifdef NET_MULTICAST_PRESENT
  /* Not Yet Implemented for Stellaris */
	#error "Multicast not yet implemented"
#else
   (void)&pif;                                                  /* Prevent 'variable unused' compiler warnings.         */
   (void)&paddr_hw;
   (void)&addr_hw_len;
#endif

   *perr = NET_DEV_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetDev_AddrMulticastRemove()
*
* Description : Configure hardware address filtering to reject specified hardware address.
*
* Argument(s) : pif         Pointer to an Ethernet network interface.
*               ---         Argument validated in NetIF_AddrHW_SetHandler().
*
*               paddr_hw    Pointer to hardware address.
*               --------    Argument checked   in NetIF_AddrHW_SetHandler().
*
*               addr_len    Length  of hardware address.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_DEV_ERR_NONE                Hardware address successfully removed.
*
*                                                               - RETURNED BY NetUtil_32BitCRC_Calc() : -
*                               NET_UTIL_ERR_NULL_PTR           Argument 'pdata_buf' passed NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Argument 'len' passed equal to 0.
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_AddrMulticastAdd() via 'pdev_api->AddrMulticastRemove()'.
*
* Note(s)     : (1) The device is capable of the following multicast address filtering techniques :
*                       (a) Perfect        filtering of ONE multicast address.
*                       (b) Imperfect hash filtering of 64  multicast addresses.
*                       (c) Promiscous non filtering.  Disable filtering of all received frames.
*
*               (2) This function implements the filtering mechanism described in 1b.
*********************************************************************************************************
*/
#if (NET_VERSION >= 205u)
static void NetDev_AddrMulticastRemove_Stellaris_Eth0 	(NET_IF *pif,
														 CPU_INT08U *paddr_hw,
														 CPU_INT08U addr_hw_len,
														 NET_ERR *perr)
{
#ifdef NET_MULTICAST_PRESENT
    /* Not Yet Implemented for Stellaris */
	#error "Multicast not yet implemented"
#endif

   *perr = NET_DEV_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_ISR_Handler()
*
* Description : This function serves as the device Interrupt Service Routine Handler. This ISR
*               handler MUST service and clear all necessary and enabled interrupt events for
*               the device.
*
* Argument(s) : pif     Pointer to interface requiring service.
*               ---
*
*               type    Network Interface defined argument representing the type of ISR in progress. Codes
*                       for Rx, Tx, Overrun, Jabber, etc... are defined within net_if.h and are passed
*                       into this function by the corresponding Net BSP ISR handler function. The Net
*                       BSP ISR handler function may be called by a specific ISR vector and therefore
*                       know which ISR type code to pass.  Otherwise, the Net BSP may pass
*                       NET_DEV_ISR_TYPE_UNKNOWN and the device driver MAY ignore the parameter when
*                       the ISR type can be deduced by reading an available interrupt status register.
*
*                       Type codes that are defined within net_if.c include but are not limited to :
*                           NET_DEV_ISR_TYPE_RX
*                           NET_DEV_ISR_TYPE_TX_COMPLETE
*                           NET_DEV_ISR_TYPE_UNKNOWN
*
* Return(s)   : none.
*
* Caller(s)   : Specific first- or second-level BSP ISR handler.
*
* Note(s)     : (1) This function is called by name from the context of an ISR.
*
*               (2) In the case of an interrupt occurring prior to Network Protocol Stack initialization,
*                   the device driver should ensure that the interrupt source is cleared in order
*                   to prevent the potential for an infinite interrupt loop during system initialization.
*
*               (3) Many DMA devices generate only one interrupt event for several ready receive
*                   descriptors.  In order to accommodate this, it is recommended that all DMA based
*                   drivers count the number of ready receive descriptors during the receive event
*                   and signal the receive task for ONLY newly received descriptors which have not
*                   yet been signaled for during the last receive interrupt event.
*
*               (4) Many DMA devices generate only one interrupt event for several transmit
*                   complete descriptors.  In this case, the driver MUST determine which descriptors
*                   have completed transmission and post each descriptor data area address to
*                   the transmit deallocation task.  The code provided below assumes one
*                   interrupt per transmit event which may not necessarily be the case for all
*                   devices.
*
*               (5) Receive complete interrupts are disabled until all received frames have been
*                   processed.  This action reduces unnecessary interrupt overhead.  Rx interrupts
*                   are restored from NetDev_RxDescPtrCurInc_Stellaris_Eth0() when the 'next' descriptor is
*                   found to be owned by the DMA.
*
*********************************************************************************************************
*/
/*$PAGE*/
static void NetDev_ISR_Handler_Stellaris_Eth0 	(NET_IF *pif,
												 NET_DEV_ISR_TYPE type)
{
	NET_DEV_DATA       *pdev_data;
    DEV_DESC           *pdesc;
    NET_ERR             err;
    CPU_INT32U          int_status;
    CPU_BOOLEAN         tx_dealloc_done;

   (void)&type;                                                 /* Prevent compiler warning for arg type unused.        */


                                                                /* -- OBTAIN REFERENCE TO DEVICE CFG/DATA/REGISTERS --- */
    pdev_data = (NET_DEV_DATA      *)pif->Dev_Data;             /* Obtain ptr to dev data area.                         */


	IntDisable(INT_ETH); 			                            /* Disable normal & abnormal int. summary src's to ...  */

	int_status = EthernetIntStatus(ETH_BASE, false);			/* Clr active int. src's.                               */
	EthernetIntClear(ETH_BASE, int_status);

																/* First handle any error int. requests					*/
	if ((int_status & (ETH_INT_TXER   |                         /* Tx int. Error                                        */
						ETH_INT_RXOF  |                         /* Rx (FIFO) overflow.                                  */
						ETH_INT_RXER)) > 0) {                  	/* Other Rx Error		                                */
	
		if((int_status & ETH_INT_RXER) > 0) {
			NET_CTR_ERR_INC(pdev_data->ErrRxCtr);
		}
	
		if((int_status & ETH_INT_RXOF) > 0) {
			NET_CTR_ERR_INC(pdev_data->ErrRxCtrOF);
		}
	
		if((int_status & ETH_INT_TXER) > 0) {
			NET_CTR_ERR_INC(pdev_data->ErrTxCtr);
		}
		
		pdev_data->IntStatusLast = int_status;                  /* Record last ISR status for debug.                    */
	}
	
	if((int_status & ETH_INT_RX) > 0) {							/* Handle RX Packets									*/
		
		HWREGBITW(&g_ulFlags, FLAG_RXPKT) = 1;
		EthernetIntDisable(ETH_BASE, ETH_INT_RX);
		
		NetOS_IF_RxTaskSignal(pif->Nbr, &err);                  /* Signal Net IF RxQ Task for each new rdy Desc.        */
		switch (err) {
			case NET_IF_ERR_NONE:
				 NET_CTR_STAT_INC(pdev_data->StatRxCtrTaskSignalISR);
				 break;

			case NET_IF_ERR_RX_Q_FULL:
			case NET_IF_ERR_RX_Q_SIGNAL_FAULT:
			default:
				 // $$$$ TODO DGT                   /* Re-enable int's and signal on next Rx.               */
				 NET_CTR_ERR_INC(pdev_data->ErrRxCtrTaskSignalFail);
				 break;
		}
	}
	
	if(HWREGBITW(&g_ulFlags, FLAG_RXPKTPEND) == 1) {			/* Check to see if waiting on RX DMA to complete		*/

		if(uDMAChannelModeGet(UDMA_CHANNEL_ETH0RX)
								== UDMA_MODE_STOP) {		 	/* Verify the channel transfer is done 					*/

																
            HWREGBITW(&g_ulFlags, FLAG_RXPKTPEND) = 0;			/* Indicate that a data has been read in.				*/
        }
    }
	
	if(HWREGBITW(&g_ulFlags, FLAG_TXPKT) == 1)					/* Handle TX uDMA transfer complete.					*/
	{
		if(uDMAChannelModeGet(UDMA_CHANNEL_ETH0TX) == UDMA_MODE_STOP)	/* Verify that channel transfer is done			*/
		{
			HWREG(ETH_BASE + MAC_O_TR) = MAC_TR_NEWTX;			/* Trigger Transmission of the data						*/
			
			HWREGBITW(&g_ulFlags, FLAG_TXPKT) = 0;				/* Indicate that the packet has been sent				*/
			
			tx_dealloc_done = DEF_NO;
			while (tx_dealloc_done == DEF_NO) {                     /* Dealloc all buf tx'd (1 int for 2 & more buf tx'd).  */
				NET_CTR_STAT_INC(pdev_data->StatTxCtrTxISR);
				pdesc = pdev_data->TxBufDescPtrComp;
				pdev_data->TxBufDescPtrComp = pdesc->Next;
				NetOS_IF_TxDeallocTaskPost(pdesc->Addr, &err);
				NetOS_Dev_TxRdySignal(pif->Nbr);                    /* Signal Net IF that Tx resources are available.       */

                                                                /* if all tx'd buff dealloc'd.                          */
				if (pdev_data->TxBufDescPtrComp == pdev_data->TxBufDescPtrCur) {
					tx_dealloc_done = DEF_YES;
				}
			}
		}
	}
	
	IntEnable(INT_ETH);											/* Re-Enable normal & abnormal int. src's.              */

}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_IO_Ctrl()
*
* Description : This function provides a mechanism for the Phy driver to update the MAC link
*               and duplex settings, as well as a method for the application and link state
*               timer to obtain the current link status.  Additional user specified driver
*               functionality MAY be added if necessary.
*
* Argument(s) : pif     Pointer to interface requiring service.
*
*               opt     Option code representing desired function to perform. The Network Protocol Suite
*                       specifies the option codes below. Additional option codes may be defined by the
*                       driver developer in the driver's header file.
*                           NET_IF_IO_CTRL_LINK_STATE_GET
*                           NET_IF_IO_CTRL_LINK_STATE_UPDATE
*
*                       Driver defined operation codes MUST be defined starting from 20 or higher
*                       to prevent clashing with the pre-defined operation code types. See the
*                       device driver header file for more details.
*
*               data    Pointer to optional data for either sending or receiving additional function
*                       arguments or return data.
*
*               perr    Pointer to return error code.
*                           NET_IF_ERR_INVALID_IO_CTRL_OPT      Invalid option number specified.
*                           NET_IF_ERR_NULL_FNCT                Null interface function pointer encountered.
*
*                           NET_DEV_ERR_NONE                    IO Ctrl operation completed successfully.
*                           NET_DEV_ERR_NULL_PTR                Null argument pointer passed.
*
*                           NET_PHY_ERR_NULL_PTR                Pointer argument(s) passed NULL pointer(s).
*                           NET_PHY_ERR_TIMEOUT_RESET           Phy reset          time-out.
*                           NET_PHY_ERR_TIMEOUT_AUTO_NEG        Auto-Negotiation   time-out.
*                           NET_PHY_ERR_TIMEOUT_REG_RD          Phy register read  time-out.
*                           NET_PHY_ERR_TIMEOUT_REG_WR          Phy register write time-out.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Ether_IO_CtrlHandler() via 'pdev_api->IO_Ctrl()',
*               NetPhy_LinkStateGet()        via 'pdev_api->IO_Ctrl()'.
*
* Note(s)     : (1) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*********************************************************************************************************
*/
/*$PAGE*/
static void NetDev_IO_Ctrl_Stellaris_Eth0 	(NET_IF *pif,
											 CPU_INT08U opt,
											 void *p_data,
											 NET_ERR *perr)
{
    NET_DEV_LINK_ETHER  *plink_state;
    NET_PHY_API_ETHER   *pphy_api;

                                                                /* ------- OBTAIN REFERENCE TO DEVICE REGISTERS ------- */


    switch (opt) {
        case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:
             plink_state      = (NET_DEV_LINK_ETHER *)p_data;
             if (plink_state == (NET_DEV_LINK_ETHER *)0) {
                *perr = NET_DEV_ERR_NULL_PTR;
                 return;
             }
             pphy_api = (NET_PHY_API_ETHER *)pif->Phy_API;
             if (pphy_api == (void *)0) {
                *perr = NET_IF_ERR_NULL_FNCT;
                 return;
             }
             pphy_api->LinkStateGet(pif, plink_state, perr);
             if (*perr != NET_PHY_ERR_NONE) {
                return;
             }

            *perr = NET_DEV_ERR_NONE;
             break;


        case NET_IF_IO_CTRL_LINK_STATE_UPDATE:

             switch (plink_state->Duplex) {                     /* Update duplex setting on device.                     */
                case NET_PHY_DUPLEX_FULL:
                     EthernetConfigSet(ETH_BASE, EthernetConfigGet(ETH_BASE) | ETH_CFG_TX_DPLXEN);
                     break;

                case NET_PHY_DUPLEX_HALF:
                     EthernetConfigSet(ETH_BASE, EthernetConfigGet(ETH_BASE) & (~ETH_CFG_TX_DPLXEN));
                     break;

                default:
                     break;
             }

             switch (plink_state->Spd) {                        /* Update speed setting on device.                */
                case NET_PHY_SPD_10:
                     EthernetPHYWrite(ETH_BASE, PHY_MR0, EthernetPHYRead(ETH_BASE, PHY_MR0) & (~PHY_MR0_SPEEDSL));
                     break;

                case NET_PHY_SPD_100:
                     EthernetPHYWrite(ETH_BASE, PHY_MR0, EthernetPHYRead(ETH_BASE, PHY_MR0) | PHY_MR0_SPEEDSL);
                     break;

                default:
                     break;
             }

             *perr = NET_DEV_ERR_NONE;
             break;


        default:
            *perr = NET_IF_ERR_INVALID_IO_CTRL_OPT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_MII_Rd()
*
* Description : Read data over the (R)MII bus from the specified Phy register.
*
* Argument(s) : pif         Pointer to the interface requiring service.
*
*               phy_addr   (R)MII bus address of the Phy requiring service.
*
*               reg_addr    Phy register number to write to.
*
*               p_data      Pointer to variable to store returned register data.
*
*               perr    Pointer to return error code.
*                           NET_DEV_ERR_NULL_PTR            Pointer argument(s) passed NULL pointer(s).
*                           NET_PHY_ERR_NONE                MII write completed successfully.
*                           NET_PHY_ERR_TIMEOUT_REG_RD      Register read time-out.
*
* Return(s)   : none.
*
* Caller(s)   : Various Phy functions.
*
* Note(s)     : (1) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*********************************************************************************************************
*/

static void NetDev_MII_Rd_Stellaris_Eth0 	(NET_IF *pif,
											 CPU_INT08U phy_addr,
											 CPU_INT08U reg_addr,
											 CPU_INT16U *p_data,
											 NET_ERR *perr)
{
	NET_DEV_CFG_ETHER  *pdev_cfg;


                                                                 /* ------- OBTAIN REFERENCE TO DEVICE REGISTERS ------ */
    pdev_cfg = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;                /* Obtain ptr to the dev cfg struct.                   */

	

    *p_data = EthernetPHYRead(pdev_cfg->BaseAddr, reg_addr);
	
    *perr = NET_PHY_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDev_MII_Wr()
*
* Description : Write data over the (R)MII bus to the specified Phy register.
*
* Argument(s) : pif         Pointer to the interface requiring service.
*
*               phy_addr   (R)MII bus address of the Phy requiring service.
*
*               reg_addr    Phy register number to write to.
*
*               data        Data to write to the specified Phy register.
*
*               perr    Pointer to return error code.
*                           NET_PHY_ERR_NONE                MII write completed successfully.
*                           NET_PHY_ERR_TIMEOUT_REG_WR      Register write time-out.
*
* Return(s)   : none.
*
* Caller(s)   : Various Phy functions.
*
* Note(s)     : (1) All functions that require device register access MUST obtain reference
*                   to the device hardware register space PRIOR to attempting to access
*                   any registers.  Register definitions SHOULD NOT be absolute and SHOULD
*                   use the provided base address within the device configuration structure,
*                   as well as the device register definition structure in order to properly
*                   resolve register addresses during run-time.
*********************************************************************************************************
*/
static void NetDev_MII_Wr_Stellaris_Eth0 	(NET_IF *pif,
											 CPU_INT08U phy_addr,
											 CPU_INT08U reg_addr,
											 CPU_INT16U data,
											 NET_ERR *perr)
{
	NET_DEV_CFG_ETHER  *pdev_cfg;

                                                                  /* ------ OBTAIN REFERENCE TO DEVICE REGISTERS ------ */
    pdev_cfg = (NET_DEV_CFG_ETHER *)pif->Dev_Cfg;                 /* Obtain ptr to the dev cfg struct.                  */

	EthernetPHYWrite( pdev_cfg->BaseAddr, reg_addr, data);
	

    *perr = NET_PHY_ERR_NONE;
}


