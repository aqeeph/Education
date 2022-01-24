/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                                      Texas Instruments TM4C1294
*                                                on the
*
*                                            EK-TM4C1294XL
*                                            Evaluation Kit
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FF
*                 SKB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/

#include  "..\bsp\bsp_sys.h"
#include  "..\bsp\bsp_led.h"
#include  "..\bsp\bsp_int.h"
#include  "..\bsp\bsp_misc.h"


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description: This function should be called by the application code before using any functions in this
*              module.
*
* Arguments  : none
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    BSP_IntInit();

    BSP_SysInit();                                              /* Initialize system services, clocks, etc.             */

    BSP_LED_Init();

    BSP_Misc_Init();
}

