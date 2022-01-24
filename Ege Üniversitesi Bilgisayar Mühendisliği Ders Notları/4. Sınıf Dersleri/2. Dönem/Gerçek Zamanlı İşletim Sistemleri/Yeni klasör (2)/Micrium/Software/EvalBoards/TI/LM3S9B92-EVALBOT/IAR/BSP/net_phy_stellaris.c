
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
*                                   NETWORK PHYSICAL LAYER DRIVER
*
*                                        GENERIC ETHERNET PHY
*
* Filename      : net_phy.c
* Version       : V2.10.02
* Programmer(s) : EHS
*                 FGK
*                 HMS
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/TCP-IP V2.02 (or more recent version) is included in the project build.
*
*                 (2) The (R)MII interface port is assumed to be part of the host EMAC.  Therefore, (R)MII
*                     reads/writes MUST be performed through the network device API interface via calls to
*                     function pointers 'Phy_RegRd()' & 'Phy_RegWr()'.
*
*                 (3) Interrupt support is Phy specific, therefore the generic Phy driver does NOT support
*                     interrupts.  However, interrupt support is easily added to the generic Phy driver &
*                     thus the ISR handler has been prototyped and & populated within the function pointer
*                     structure for example purposes.
*
*                 (4) Does NOT support 1000Mbps Phy.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_PHY_GENERIC_MODULE
#include  <net.h>
#include  <net_phy_stellaris.h>
#include  <inc\hw_types.h>
#include  <inc\hw_ethernet.h>
#include  <driverlib\ethernet.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  NET_PHY_ADDR_MAX                     31u               /* 5 bit Phy address max value.                         */

#define  NET_PHY_INIT_AUTO_NEG_RETRIES         3u               /* Attempt Auto-Negotiation x times                     */
#define  NET_PHY_INIT_RESET_RETRIES            3u               /* Check for successful reset x times                   */



/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*
* Note(s) : (1) Physical layer driver functions may be arbitrarily named.  However, it is recommended that
*               physical layer driver functions be named using the names provided below.  All driver function
*               prototypes should be located within the driver C source file ('net_phy_&&&.c') & be declared
*               as static functions to prevent name clashes with other network protocol suite physical layer
*               drivers.
*********************************************************************************************************
*/
static 	void  NetPhy_Init_Stellaris_Eth0 		(NET_IF 			*pif,
												 NET_ERR 			*perr);

static 	void  NetPhy_EnDis_Stellaris_Eth0 		(NET_IF 			*pif,
												 CPU_BOOLEAN 		en,
												 NET_ERR 			*perr);

static 	void  NetPhy_LinkStateGet_Stellaris_Eth0(NET_IF 			*pif,
												 NET_DEV_LINK_ETHER *plink_state,	
												 NET_ERR 			*perr);

#if (NET_VERSION >= 205u)												
static 	void  NetPhy_LinkStateSet_Stellaris_Eth0(NET_IF 			*pif,
												 NET_DEV_LINK_ETHER *plink_state,
												 NET_ERR 			*perr);
#endif

static  void  NetPhy_AutoNegStart_Stellaris_Eth0(NET_IF             *pif,
												 NET_ERR            *perr);

static  void  NetPhy_AddrProbe_Stellaris_Eth0	(NET_IF             *pif,
												 NET_ERR            *perr);




/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  NETWORK PHYSICAL LAYER DRIVER API
*
* Note(s) : (1) Physical layer driver API structures are used by applications during calls to NetIF_Add().
*               This API structure allows higher layers to call specific physical layer driver functions
*               via function pointer instead of by name.  This enables the network protocol suite to
*               compile & operate with multiple physical layer drivers.
*
*           (2) In most cases, the API structure provided below SHOULD suffice for most physical layer
*               drivers exactly as is with the exception that the API structure's name which MUST be
*               unique & SHOULD clearly identify the physical layer being implemented.  For example,
*               the AMD 79C874's API structure should be named NetPhy_API_AM79C874[].
*
*               The API structure MUST also be externally declared in the physical layer driver header
*               file ('net_phy_&&&.h') with the exact same name & type.
*********************************************************************************************************
*/
                                                                                    /* Generic phy API fnct ptrs :      */
const  NET_PHY_API_ETHER  NetPhy_API_Generic = { NetPhy_Init_Stellaris_Eth0,        /*   Init                           */
#if (NET_VERSION >= 20600u)
                                                 NetPhy_EnDis_Stellaris_Eth0,       /*   En/dis                         */
#endif
#if (NET_VERSION <  205u)
                                                 NetPhy_AutoNegStart_Stellaris_Eth0,/*   Auto-neg                       */
#endif
                                                 NetPhy_LinkStateGet_Stellaris_Eth0,/*   Link get                       */
#if (NET_VERSION >= 205u)
                                                 NetPhy_LinkStateSet_Stellaris_Eth0,/*   Link set                       */
#endif
#if (NET_VERSION <  20600u)
                                                 NetPhy_EnDis_Stellaris_Eth0,       /*   En/dis                         */
#endif
                                                 0                                  /*   ISR handler                    */
                                               };

/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetPhy_Init()
*
* Description : Initialize Ethernet physical layer.
*
* Argument(s) : pif         Pointer to interface to initialize Phy.
*               ---         Argument checked in NetIF_Ether_IF_Start().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE                Ethernet physical layer successfully
*                                                                   initialized.
*                               NET_PHY_ERR_TIMEOUT_RESET       Phy reset          time-out.
*                               NET_PHY_ERR_TIMEOUT_AUTO_NEG    Auto-Negotiation   time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_RD      Phy register read  time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_WR      Phy register write time-out.
*
* Return(s)   : none.
*
* Caller(s)   : Interface &/or device start handler(s).
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by
*               application function(s) [see also Note #3].
*
* Note(s)     : (1) Assumes the MDI port as already been enabled for the Phy.
*
*               (2) Phy initialization occurs each time the interface is started.
*                   See 'net_if.c  NetIF_Start()'.
*********************************************************************************************************
*/
static void NetPhy_Init_Stellaris_Eth0 (NET_IF *pif,
										NET_ERR *perr)
{
	NET_DEV_API_ETHER  *pdev_api;
    NET_PHY_CFG_ETHER  *pphy_cfg;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_val;
    CPU_INT16U          retries;
    CPU_INT08U          phy_addr;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;
    pphy_cfg = (NET_PHY_CFG_ETHER *)pif->Phy_Cfg;
    pif_data = (NET_IF_DATA_ETHER *)pif->IF_Data;

    phy_addr =  pphy_cfg->BusAddr;

    if (phy_addr == NET_PHY_ADDR_AUTO) {                        /* Automatic detection of Phy address enabled.          */
        NetPhy_AddrProbe_Stellaris_Eth0(pif, perr);                            /* Attempt to automatically determine Phy addr.         */
        if (*perr != NET_PHY_ERR_NONE) {
             return;
        }

        phy_addr           = pif_data->Phy_Addr;
    } else {
        pif_data->Phy_Addr = phy_addr;                          /* Set Phy addr to cfg'd val.                           */
    }
                                                                /* -------------------- RESET PHY --------------------- */
                                                                /* Reset Phy.                                           */
    pdev_api->Phy_RegWr(pif, phy_addr, PHY_MR0, PHY_MR0_RESET, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }
                                                                /* Rd ctrl reg, get reset bit.                          */
    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    reg_val &= PHY_MR0_RESET;                                      /* Mask out reset status bit.                           */


    retries = NET_PHY_INIT_RESET_RETRIES;
    while ((reg_val == PHY_MR0_RESET) && (retries > 0u)) {         /* Wait for reset to complete.                          */
        NetOS_TimeDly_ms(2u, perr);

        pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
        if (*perr != NET_PHY_ERR_NONE) {
             return;
        }

        reg_val &= PHY_MR0_RESET;
        retries--;
    }

    if (retries == 0u) {
       *perr = NET_PHY_ERR_TIMEOUT_RESET;
        return;
    }


}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetPhy_EnDis()
*
* Description : Enable/disable the Phy.
*
* Argument(s) : pif         Pointer to interface to enable/disable Phy.
*               ---
*
*               en          Enable option :
*
*                               DEF_ENABLED                     Enable  Phy
*                               DEF_DISABLED                    Disable Phy
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE                Physical layer successfully enabled/disabled.
*                               NET_PHY_ERR_TIMEOUT_RESET       Phy reset          time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_RD      Phy register read  time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_WR      Phy register write time-out.
*
* Caller(s)   : NetIF_Ether_IF_Start() via 'pphy_api->EnDis()',
*               NetIF_Ether_IF_Stop()  via 'pphy_api->EnDis()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void NetPhy_EnDis_Stellaris_Eth0 (NET_IF *pif,
										 CPU_BOOLEAN en,
										 NET_ERR *perr)
{
	NET_DEV_API_ETHER  *pdev_api;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_val;
    CPU_INT08U          phy_addr;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;
    pif_data = (NET_IF_DATA_ETHER *)pif->IF_Data;
    phy_addr =  pif_data->Phy_Addr;
                                                                /* Get cur ctrl reg val.                                */
    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    switch (en) {
        case DEF_DISABLED:
             reg_val |=  PHY_MR0_PWRDN;                            /* Dis Phy.                                             */
             break;


        case DEF_ENABLED:
        default:
             reg_val &= ~PHY_MR0_PWRDN;                            /* En  Phy.                                             */
             break;
    }
                                                                /* Pwr up / down the Phy.                               */
    pdev_api->Phy_RegWr(pif, phy_addr, PHY_MR0, reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

   *perr = NET_PHY_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetPhy_LinkStateGet()
*
* Description : Get current Phy link state (speed & duplex).
*
* Argument(s) : pif             Pointer to interface to get link state.
*               ---             Argument validated in NetIF_IO_CtrlHandler().
*
*               plink_state     Pointer to structure that will receive the link state.
*
*               perr            Pointer to variable  that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE            No  error.
*                               NET_PHY_ERR_NULL_PTR        Pointer argument(s) passed NULL pointer(s).
*                               NET_PHY_ERR_TIMEOUT_RESET   Phy reset          time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_RD  Phy register read  time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_WR  Phy register write time-out.
*
* Caller(s)   : Device driver(s)' link state &/or I/O control handler(s).
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by
*               application function(s) [see also Note #2].
*
* Return(s)   : none.
*
* Note(s)     : (1) Some Phy's have the link status field latched in the BMSR register.  The link status
*                   remains low after a temporary link failure until it is read. To retrieve the current
*                   link status, BMSR must be read twice.
*
*               (2) Current link state should be obtained by calling this function through the NetIF layer.
*                   See 'net_if.c  NetIF_IO_Ctrl()'.
*********************************************************************************************************
*/
static void NetPhy_LinkStateGet_Stellaris_Eth0 (NET_IF *pif,
												NET_DEV_LINK_ETHER *plink_state,
												NET_ERR *perr)
{
  NET_DEV_API_ETHER  *pdev_api;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_val;
    CPU_INT16U          link_self;
    CPU_INT16U          link_partner;
    CPU_INT08U          phy_addr;
    NET_ERR             err;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;
    pif_data = (NET_IF_DATA_ETHER *)pif->IF_Data;
    phy_addr =  pif_data->Phy_Addr;

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (plink_state == (NET_DEV_LINK_ETHER *)0) {
       *perr = NET_PHY_ERR_NULL_PTR;
        return;
    }
#endif

    plink_state->Spd    = NET_PHY_SPD_0;
    plink_state->Duplex = NET_PHY_DUPLEX_UNKNOWN;


                                                                /* ------------- OBTAIN CUR LINK STATUS  -------------- */
    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR1, &link_self, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }
                                                                /* Rd BMSR twice (see Note #1).                         */
    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR1, &link_self, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    if ((link_self & PHY_MR1_LINK) == 0u) {                     /* Chk if link down.                                    */
        *perr = NET_PHY_ERR_NONE;
         return;
    }
                                                                /* ------------- DETERMINE SPD AND DUPLEX ------------- */
                                                                /* Obtain AN settings.                                  */
    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    if ((reg_val & PHY_MR0_ANEGEN) == 0u) {                      /* IF AN disabled.                                      */
        if ((reg_val & PHY_MR0_SPEEDSL) == 0u) {                  /* Determine spd.                                       */
            plink_state->Spd = NET_PHY_SPD_10;
        } else {
            plink_state->Spd = NET_PHY_SPD_100;
        }

        if ((reg_val & PHY_MR0_DUPLEX) == 0u) {                  /* Determine duplex.                                    */
            plink_state->Duplex = NET_PHY_DUPLEX_HALF;
        } else {
            plink_state->Duplex = NET_PHY_DUPLEX_FULL;
        }
    } else {
                                                                /* AN enabled. Get self link capabilities.              */
        pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR4, &link_self, perr);
        if (*perr != NET_PHY_ERR_NONE) {
             return;
        }
                                                                /* Get link partner link capabilities.                  */
        pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR5, &link_partner, perr);
        if (*perr != NET_PHY_ERR_NONE) {
             return;
        }

        link_partner &= (PHY_MR4_A3  |						/* Preserve link status bits.                           */
                         PHY_MR4_A2  |
                         PHY_MR4_A1  |
                         PHY_MR4_A0);

        link_self    &= link_partner;                           /* Match self capabilties to partner capabilities.      */

        if (link_self           >= PHY_MR4_A3) {            /* Determine most likely link state.                    */
            plink_state->Spd     = NET_PHY_SPD_100;
            plink_state->Duplex  = NET_PHY_DUPLEX_FULL;
        } else if (link_self    >= PHY_MR4_A2) {
            plink_state->Spd     = NET_PHY_SPD_100;
            plink_state->Duplex  = NET_PHY_DUPLEX_HALF;
        } else if (link_self    >= PHY_MR4_A1) {
            plink_state->Spd     = NET_PHY_SPD_10;
            plink_state->Duplex  = NET_PHY_DUPLEX_FULL;
        } else {
            plink_state->Spd     = NET_PHY_SPD_10;
            plink_state->Duplex  = NET_PHY_DUPLEX_HALF;
        }
    }
                                                                /* Link established, update MAC settings.               */
    pdev_api->IO_Ctrl((NET_IF   *) pif,
                      (CPU_INT08U) NET_IF_IO_CTRL_LINK_STATE_UPDATE,
                      (void     *) plink_state,
                      (NET_ERR  *)&err);                        /* !!!! Ignore dev update err.                          */

   *perr = NET_PHY_ERR_NONE;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetPhy_LinkStateSet()
*
* Description : Set current Phy link state (speed & duplex).
*
* Argument(s) : pif             Pointer to interface to get link state.
*               ---             Argument validated in NetIF_Start().
*
*               plink_state     Pointer to structure that will contain the desired link state.
*
*               perr            Pointer to variable  that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE            No  error.
*                               NET_PHY_ERR_NULL_PTR        Pointer argument(s) passed NULL pointer(s).
*                               NET_PHY_ERR_TIMEOUT_RESET   Phy reset          time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_RD  Phy register read  time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_WR  Phy register write time-out.
*
* Caller(s)   : NetIF_Ether_IF_Start() via 'pphy_api->LinkStateSet()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void NetPhy_LinkStateSet_Stellaris_Eth0 (NET_IF *pif,
												NET_DEV_LINK_ETHER *plink_state,
												NET_ERR *perr)
{
//$$$$ TODO: DGT TO REVIEW THIS CODE
  NET_DEV_API_ETHER  *pdev_api;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_val;
    CPU_INT16U          spd;
    CPU_INT08U          duplex;
    CPU_INT08U          phy_addr;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;
    pif_data = (NET_IF_DATA_ETHER *)pif->IF_Data;
    phy_addr =  pif_data->Phy_Addr;

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (plink_state == (NET_DEV_LINK_ETHER *)0) {
       *perr = NET_PHY_ERR_NULL_PTR;
        return;
    }
	
#endif

    spd    = plink_state->Spd;
    duplex = plink_state->Duplex;

    if (((spd    != NET_PHY_SPD_10 )     &&                     /* Enable AN if cfg invalid or any member set to AUTO.  */
         (spd    != NET_PHY_SPD_100))    ||
        ((duplex != NET_PHY_DUPLEX_HALF) &&
         (duplex != NET_PHY_DUPLEX_FULL))) {
        NetPhy_AutoNegStart_Stellaris_Eth0(pif, perr);
        return;
    }

    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    reg_val &= ~PHY_MR0_ANEGEN;                                  /* Clr AN enable bit.                                   */

    switch (spd) {                                              /* Set spd.                                             */
        case NET_PHY_SPD_10:
             reg_val &= ~PHY_MR0_SPEEDSL;
             break;

        case NET_PHY_SPD_100:
             reg_val |=  PHY_MR0_SPEEDSL;
             break;

        default:
             break;
    }

    switch (duplex) {                                           /* Set duplex.                                          */
        case NET_PHY_DUPLEX_HALF:
             reg_val &= ~PHY_MR0_DUPLEX;
             break;

        case NET_PHY_DUPLEX_FULL:
             reg_val |=  PHY_MR0_DUPLEX;
             break;

        default:
             break;
    }
                                                                /* Cfg Phy.                                             */
    pdev_api->Phy_RegWr(pif, phy_addr, PHY_MR0, reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

   *perr = NET_PHY_ERR_NONE;

}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetPhy_AutoNegStart()
*
* Description : Start the Auto-Negotiation processs.
*
* Argument(s) : pif         Pointer to interface to start auto-negotiation.
*               ---         Argument validated in NetPhy_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE                Physical layer successfully started.
*                               NET_PHY_ERR_TIMEOUT_REG_RD      Phy register read  time-out.
*                               NET_PHY_ERR_TIMEOUT_REG_WR      Phy register write time-out.
*
* Return(s)   : none.
*
* Caller(s)   : NetPhy_LinkStateSet().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by
*               application function(s) [see also Note #2].
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetPhy_AutoNegStart_Stellaris_Eth0 (NET_IF   *pif,
												  NET_ERR  *perr)
{
//$$$$ TODO: DGT TO REVIEW THIS CODE
   NET_DEV_API_ETHER  *pdev_api;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_val;
    CPU_INT08U          phy_addr;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;
    pif_data = (NET_IF_DATA_ETHER *)pif->IF_Data;
    phy_addr =  pif_data->Phy_Addr;

    pdev_api->Phy_RegRd(pif, phy_addr, PHY_MR0, &reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

    reg_val |= PHY_MR0_ANEGEN |
               PHY_MR0_RANEG;
                                                                /* Restart Auto-Negotiation.                            */
    pdev_api->Phy_RegWr(pif, phy_addr, PHY_MR0, reg_val, perr);
    if (*perr != NET_PHY_ERR_NONE) {
         return;
    }

   *perr = NET_PHY_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetPhy_AddrProbe()
*
* Description : Automatically detect Phy bus address.
*
* Argument(s) : pif         Pointer to interface to probe.
*               ---         Argument validated in NetPhy_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_PHY_ERR_NONE                Physical layer's address successfully
*                                                                   detected.
*                               NET_PHY_ERR_ADDR_PROBE          Unable to determine Phy address.
*
* Return(s)   : none.
*
* Caller(s)   : NetPhy_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by
*               application function(s).
*
* Note(s)     : (1) Assumes the MDI port has already been initialized for the Phy.
*********************************************************************************************************
*/
static  void  NetPhy_AddrProbe_Stellaris_Eth0 (NET_IF   *pif,
											   NET_ERR  *perr)
{
    NET_DEV_API_ETHER  *pdev_api;
    NET_IF_DATA_ETHER  *pif_data;
    CPU_INT16U          reg_id1;
    CPU_INT16U          reg_id2;
    CPU_INT08U          i;


    pdev_api = (NET_DEV_API_ETHER *)pif->Dev_API;

    for (i = 0u; i <= NET_PHY_ADDR_MAX; i++) {
                                                                /* Get Phy ID #1 reg val.                               */
        pdev_api->Phy_RegRd(pif, i, PHY_MR2, &reg_id1, perr);
        if (*perr != NET_PHY_ERR_NONE) {
             continue;
        }
                                                                /* Get Phy ID #2 reg val.                               */
        pdev_api->Phy_RegRd(pif, i, PHY_MR3, &reg_id2, perr);
        if (*perr != NET_PHY_ERR_NONE) {
             continue;
        }

        if (((reg_id1 == 0x0000u) && (reg_id2 == 0x0000u)) ||
            ((reg_id1 == 0x3FFFu) && (reg_id2 == 0x0000u)) ||
            ((reg_id1 == 0x0000u) && (reg_id2 == 0x3FFFu)) ||
            ((reg_id1 == 0x3FFFu) && (reg_id2 == 0x3FFFu)) ||
            ((reg_id1 == 0xFFFFu) && (reg_id2 == 0x0000u)) ||
            ((reg_id1 == 0x0000u) && (reg_id2 == 0xFFFFu)) ||
            ((reg_id1 == 0x3FFFu) && (reg_id2 == 0xFFFFu)) ||
            ((reg_id1 == 0xFFFFu) && (reg_id2 == 0xFFFFu))) {
            continue;
        } else {
            break;
        }
    }

    if (i > NET_PHY_ADDR_MAX) {
       *perr = NET_PHY_ERR_ADDR_PROBE;
        return;
    }

    pif_data           = (NET_IF_DATA_ETHER *)pif->IF_Data;
    pif_data->Phy_Addr =  i;

   *perr = NET_PHY_ERR_NONE;

}