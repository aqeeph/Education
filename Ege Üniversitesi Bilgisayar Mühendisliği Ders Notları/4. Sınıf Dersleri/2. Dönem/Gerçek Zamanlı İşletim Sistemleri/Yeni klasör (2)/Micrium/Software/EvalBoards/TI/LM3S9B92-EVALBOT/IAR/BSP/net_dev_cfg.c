/*
*********************************************************************************************************
*                                            uC/TCP-IP V2
*                                      The Embedded TCP/IP Suite
*                                            EXAMPLE CODE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.  However,
*               please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
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
*                                  NETWORK DEVICE CONFIGURATION FILE
*
*                                 Texas Instruments Stellaris LM3S9B92
*                                              on the
*                                      Texas Instruments EVALBOT
*
* Filename      : net_dev_cfg.c
* Version       : V2.10
* Programmer(s) : EHS
*                 FGK
*                 ITJ
*                 AA
*				  DGT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_DEV_CFG_MODULE
#include  <net.h>
#include  <includes.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                EXAMPLE NETWORK DEVICE CONFIGURATION
*
* Note(s) : (1) (a) Buffer & memory sizes & alignments configured in number of octets.
*               (b) Data bus size                      configured in number of bits.
*
*           (2) (a) All   network  buffer data area sizes MUST be configured greater than or equal to
*                   NET_BUF_DATA_SIZE_MIN.
*               (b) Large transmit buffer data area sizes MUST be configured greater than or equal to
*                   small transmit buffer data area sizes.
*
*               See also 'net_buf.c  NetBuf_PoolCfgValidate()  Note #3'.
*
*           (3) (a) MUST configure at least one (1) large receive  buffer.
*               (b) MUST configure at least one (1)       transmit buffer, however, zero (0) large OR
*                       zero (0) small transmit buffers MAY be configured.
*
*               See also 'net_buf.c  NetBuf_PoolCfgValidate()  Note #2'.
*
*           (4) Some processors or devices may be more efficient & may even REQUIRE that buffer data areas
*               align to specific CPU-word/octet address boundaries in order to successfully read/write
*               data from/to devices.  Therefore, it is recommended to align devices' buffer data areas to
*               the processor's or device's data bus width.
*
*               See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b'.
*
*           (5) Positive offset from base receive/transmit index, if required by device (or driver) :
*
*               (a) Some device's may receive or buffer additional octets prior to the actual  received packet.
*                   Thus, an offset may be required to ignore these additional octets :
*
*                   (1) If a device does NOT receive or buffer any  additional octets prior to received packets,
*                       then the default offset of '0' SHOULD be configured.
*
*                   (2) However, if a device does receive or buffer additional octets prior to received packets,
*                       then configure the device's receive offset with the number of additional octets.
*
*                       (A) The receive  buffer size SHOULD also be increased by the number of additional octets.
*
*                   See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b1A'.
*
*               (b) Some device's/driver's may require additional octets prior to the actual transmit packet.
*                   Thus, an offset may be required to reserve additional octets :
*
*                   (1) If a device/driver does NOT require any  additional octets prior to  transmit packets,
*                       then the default offset of '0' SHOULD be configured.
*
*                   (2) However, if a device/driver does require additional octets prior to  transmit packets,
*                       then configure the device's transmit offset with the number of additional octets.
*
*                       (A) The transmit buffer size SHOULD also be increased by the number of additional octets.
*
*                   See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b1B'.
*
*           (6) Flags to configure (optional) device features; bit-field flags logically OR'd :
*
*               (a) NET_DEV_CFG_FLAG_NONE           No device configuration flags selected.
*
*               (b) NET_DEV_CFG_FLAG_SWAP_OCTETS    Swap data octets [i.e. swap data words' high-order
*                                                       octet(s) with data words' low-order octet(s),
*                                                       & vice-versa] if required by device-to-CPU data
*                                                       bus wiring &/or CPU endian word order.
*
*           (7) Network devices with receive descriptors MUST configure the number of receive buffers
*               greater than the number of receive descriptors.
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                              EXAMPLE TI LM3S9B92 ETHERNET MAC/PHY CONFIGURATION
*********************************************************************************************************
*/

#ifdef  NET_IF_MODULE_ETHER_PRESENT

const  NET_DEV_CFG_ETHER  NetDev_Cfg_LM3S9Bxx = {

    NET_IF_MEM_TYPE_MAIN,           /* Desired receive  buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1536u,                          /* Desired size      of device's large receive  buffers (in octets) [see Note #2].  */
       10u,                          /* Desired number    of device's large receive  buffers             [see Note #3a]. */
      CPU_WORD_SIZE_32,             /* Desired alignment of device's       receive  buffers (in octets) [see Note #4].  */
       2u,                          /* Desired offset from base receive  index, if needed   (in octets) [see Note #5a]. */


    NET_IF_MEM_TYPE_MAIN,           /* Desired transmit buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1611,                           /* Desired size      of device's large transmit buffers (in octets) [see Note #2].  */
       8u,                          /* Desired number    of device's large transmit buffers             [see Note #3b]. */
     256u,                          /* Desired size      of device's small transmit buffers (in octets) [see Note #2].  */
       3u,                          /* Desired number    of device's small transmit buffers             [see Note #3b]. */
      CPU_WORD_SIZE_32,                          /* Desired alignment of device's       transmit buffers (in octets) [see Note #4].  */
       2u,                          /* Desired offset from base transmit index, if needed   (in octets) [see Note #5b]. */


    0x00000000u,                    /* Base address   of dedicated memory, if available.                                */
             0u,                    /* Size           of dedicated memory, if available (in octets).                    */


    NET_DEV_CFG_FLAG_NONE,          /* Desired option flags, if any (see Note #6).                                      */


             5u,                    /* Desired number of device's receive  descriptors (see Note #7).                   */
             8u,                    /* Desired number of device's transmit descriptors.                                 */


        ETH_BASE,                   /* Base address   of device's hardware/registers.                                   */

             0u,                    /* Size           of device's data bus (in bits), if available.                     */


            NULL,		    /* Use NULL to obtain MAC address from User Flash Registers at run time.            */
                                    /* Use string here for compile time assignemnt.  Example: "00:50:C2:25:61:00"       */
};


const  NET_PHY_CFG_ETHER  NetPhy_Cfg_LM3S9Bxx = {
    NET_PHY_ADDR_AUTO,              /* Phy bus address.                                                                 */
    NET_PHY_BUS_MODE_MII,           /* Phy bus mode.                                                                    */
    NET_PHY_TYPE_INT,               /* Phy type.                                                                        */
    NET_PHY_SPD_AUTO,               /* Auto-Negotiation determines link speed.                                          */
    NET_PHY_DUPLEX_AUTO,            /* Auto-Negotiation determines link duplex.                                         */
};
#endif                              /* End of LM3S9B92 ETH0 configuration.                                               */

