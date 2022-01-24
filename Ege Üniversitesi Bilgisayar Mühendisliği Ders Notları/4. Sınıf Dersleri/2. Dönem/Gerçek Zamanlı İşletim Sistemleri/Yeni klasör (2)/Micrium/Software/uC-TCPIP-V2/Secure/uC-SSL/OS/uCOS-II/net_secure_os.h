/*
*********************************************************************************************************
*                                             uC/TCP-IP V2
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2011; Micrium, Inc.; Weston, FL
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
*                                      NETWORK SECURITY OS LAYER
*
*                                          Micrium uC/OS-II
*
* Filename      : net_secure_os.h
* Version       : V2.11.00
* Programmer(s) : SL
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/OS-II V2.86 (or more recent version) is included in the project build.
*
*                 (2) REQUIREs the following uC/OS-II feature(s) to be ENABLED :
*
*                         --------- FEATURE ----------    ---- MINIMUM CONFIGURATION FOR OS PORT ----
*
*                     (a) Mutex
*                         (1) OS_MUTEX_EN                 Enabled
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) Ideally, this network security OS port layer header file should NOT need to be protected 
*               from multiple pre-processor inclusion.  However, since this header file MAY be included 
*               from third-party security module files, this header file is protected from multiple 
*               pre-processor inclusion through use of the network security OS port layer module present 
*               pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  NET_SECURE_OS_MODULE_PRESENT                           /* See Note #1.                                         */
#define  NET_SECURE_OS_MODULE_PRESENT


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*
* Note(s) : (1) Ideally, this network security OS port layer header file SHOULD include the main network 
*               protocol suite header file.
*
*               #### However, this network security port's third-party security module currently prevents 
*               the main network protocol suite header file from being successfully included in the build.  
*               Therefore, the main network protocol suite header file CANNOT currently be included in the 
*               build while the third-party security module precludes its successful inclusion.
*********************************************************************************************************
*/

#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#include  <ucos_ii.h>

#if 0                                                           /* See Note #1.                                         */
#include  <net.h>
#endif

#include  <ssl.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  OS_EVENT    NET_SECURE_OS_MUTEX;

typedef  CPU_INT32U  NET_SECURE_OS_TICK;


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_INT32S  NetSecure_OS_MutexCreate(OS_EVENT  *p_mutex);

CPU_INT32S  NetSecure_OS_FreeMutex  (OS_EVENT  *p_mutex);

CPU_INT32S  NetSecure_OS_LockMutex  (OS_EVENT  *p_mutex);

CPU_INT32S  NetSecure_OS_UnLockMutex(OS_EVENT  *p_mutex);


CPU_INT32U  NetSecure_OS_TimeGet    (void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                            /* See this 'net_secure_os.h  Note #1'.     */
#if     (OS_VERSION < 286u)
#error  "OS_VERSION   [SHOULD be >= V2.86]"
#endif


                                                                            /* See this 'net_secure_os.h  Note #2a1'.   */
#if     (OS_MUTEX_EN < 1u)
#error  "OS_MUTEX_EN  illegally #define'd in 'os_cfg.h'                 "
#error  "             [MUST be  > 0, (see 'net_secure_os.h  Note #2a1')]"
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_secure_os.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net secure OS module include (see Note #1).   */

