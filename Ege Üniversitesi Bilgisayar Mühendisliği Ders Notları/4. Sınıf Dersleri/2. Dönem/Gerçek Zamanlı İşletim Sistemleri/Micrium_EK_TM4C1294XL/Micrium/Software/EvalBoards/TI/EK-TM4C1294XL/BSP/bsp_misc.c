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
* Filename      : bsp_misc.c
* Version       : V1.00
* Programmer(s) : SKB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  "..\bsp\bsp.h"
#include  "..\bsp\bsp_misc.h"
#include  "..\bsp\bsp_sys.h"
#include  "..\..\..\..\..\uc-lib\lib_def.h"


/*
*********************************************************************************************************
*                                       MISC INITIALIZATION
*
* Description: This function is used to initialize miscellaneous peripherals on the board.
*
* Argument(s): None.
*
* Return(s)  : None.
*
* Caller(s)  : Application.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  BSP_Misc_Init (void)
{
                                                                /* Enable GPIO clock                                    */
    DEF_BIT_SET(BSP_SYS_REG_RCGCGPIO,
                BSP_RCGCGPIO_PORT_J);

                                                                /* Set PN0(SWS1) and PN1(SWS2) as inputs                */
    DEF_BIT_CLR(BSP_GPIO_REG_DIR(BSP_GPIO_PORTJ),
               (BSP_SWS_1 | BSP_SWS_2));

                                                                /* En PN0(SWS1) and PN1(SWS2) pins pull-up resistors    */
    DEF_BIT_SET(BSP_GPIO_REG_PUR(BSP_GPIO_PORTJ),
               (BSP_SWS_1 | BSP_SWS_2));

                                                                /* En PN0(SWS1) and PN1(SWS2) pins digital functions    */
    DEF_BIT_SET(BSP_GPIO_REG_DEN(BSP_GPIO_PORTJ),
               (BSP_SWS_1 | BSP_SWS_2));
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                               SWS READ
*
* Description: This function is used to read the values of user switches.
*
* Arguments  : led    is the color of the User LED to control
*                     1    Read from switch 1
*                     2    Read from switch 2
*
* Return(s)  : DEF_ON  if the switch is on
*              DEF_OFF if the switch is off
*
* Caller(s)  : Application.
*
* Note(s)    : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Sws_Read (CPU_INT08U  led)
{
                                                                /* When the switch is pressed, the pin is grounded.     */
    switch (led) {
        case 1:
             return (BSP_GPIO_REG_DATA(BSP_GPIO_PORTJ) & BSP_SWS_1) ? DEF_OFF : DEF_ON;

        case 2:
             return (BSP_GPIO_REG_DATA(BSP_GPIO_PORTJ) & BSP_SWS_2) ? DEF_OFF : DEF_ON;

        default:
             return  DEF_OFF;
    }
}
