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
* Filename      : bsp_sys.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDES
*********************************************************************************************************
*/

#include  "bsp_cfg.h"
#include  "..\..\..\..\..\uc-lib\lib_def.h"
#include  "..\bsp\bsp_sys.h"


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP SYSTEM INITIALIZATION
*
* Description: This function should be called early in the BSP initialization process.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Caller(s)  : Application.
*
* Note(s)    : 1) Depending on the CPU frequency, the application must program the Main Flash and EEPROM
*                 memory timing paremeters according to the following table:
*
*               +-----------------------+--------------------------+-------------+-----------+---------+
*               | CPU Freq. Range(F) in | Time Period Range (t) in | FBCHT/EBCHT | FBCE/EBCE | FWS/EWS |
*               |         MHz           |          ns              |             |           |         |
*               +-----------------------+--------------------------+-------------+-----------+---------+
*               |         16            |          62.5            |      0      |     1     |    0    |
*               |    16 < f <=  30      |    62.5  > f >= 33.3     |      1      |     1     |    1    |
*               |    30 < f <=  50      |    33.3  > f >= 20       |      2      |     1     |    2    |
*               |    50 < f <=  60      |    20    > f >= 16.67    |      3      |     0     |    3    |
*               |    60 < f <=  80      |    16.67 > f >= 12.5     |      4      |     0     |    4    |
*               |    80 < f <= 100      |    12.5  > f >= 10       |      5      |     0     |    5    |
*               |   100 < f <= 120      |    10    > f >=  8.33    |      6      |     0     |    6    |
*               +-----------------------+--------------------------+-------------+-----------+---------+
*********************************************************************************************************
*/

void  BSP_SysInit (void)
{
                                                                /* Enable xtal crystal oscillator                       */
    DEF_BIT_CLR(BSP_SYS_REG_MOSCCTL, (BSP_MOSCCTL_OSCRNG  |
                                      BSP_MOSCCTL_NOXTAL  |
                                      BSP_MOSCCTL_PWRDN));

    DEF_BIT_SET(BSP_SYS_REG_MOSCCTL, BSP_MOSCCTL_OSCRNG);

                                                                /* Set Flash & EEPROM timing CPU freq = 16Mhz           */
                                                                /* See Note 1.                                          */
    DEF_BIT_CLR(BSP_SYS_REG_MEMTIM0, (BSP_MEMTIM0_EBCHT_MASK |
                                      BSP_MEMTIM0_FBCHT_MASK |
                                      BSP_MEMTIM0_EWS_MASK   |
                                      BSP_MEMTIM0_FWS_MASK));

    DEF_BIT_SET(BSP_SYS_REG_MEMTIM0, (BSP_MEMTIM0_EBCE |
                                      BSP_MEMTIM0_FBCE));

                                                                /* ---------------- PLL CONFIGURATION ----------------- */
    BSP_SYS_REG_RSCLKCFG = DEF_BIT_NONE;                        /* Switch to PIOSC = 16Mhz for PLL input clk & Osc src  */

    DEF_BIT_SET(BSP_SYS_REG_RSCLKCFG, BSP_RSCLKCFG_MEMTIMU);    /* Update memory timing to match running from PIOSC     */

    BSP_SYS_REG_RSCLKCFG = (BSP_RSCLKCFG_OSCSRC_MOSC  |         /* Select MOSC = 25Mhz for PLL input clk & Osc source   */
                            BSP_RSCLKCFG_PLLSRC_MOSC);

                                                                /* MDIV = MINT + (MFRAC / 1024)                         */
                                                                /* MDIV = 96   + (0 / 1024) = 96                        */
    BSP_SYS_REG_PLLFREQ0 = ((BSP_CFG_PLL_MFRAC << 10u)  |
                            (BSP_CFG_PLL_MINT  <<  0u));
                                                                /* Fvco freq = (fmosc * MDIV) / ((Q + 1) * (N + 1))     */
                                                                /*           = (25MHz * 96) / (0 + 1) * (4 + 1))        */
                                                                /*           = 480MHz                                   */
    BSP_SYS_REG_PLLFREQ1 = ((BSP_CFG_PLL_Q << 8u)  |
                            (BSP_CFG_PLL_N << 0u));

                                                                /* Prepare Flash & EEPROM timing for CPU freq = 120Mhz  */
                                                                /* See Note 1.                                          */
    DEF_BIT_CLR(BSP_SYS_REG_MEMTIM0, (BSP_MEMTIM0_EBCHT_MASK |
                                      BSP_MEMTIM0_FBCHT_MASK |
                                      BSP_MEMTIM0_EWS_MASK   |
                                      BSP_MEMTIM0_FWS_MASK));

    BSP_SYS_REG_MEMTIM0 |= ((BSP_MEMTIM0_xBCHT_3_5 << 22u)  |
                            (BSP_MEMTIM0_WS_6      << 16u)  |
                            (BSP_MEMTIM0_xBCHT_3_5 <<  6u)  |
                            (BSP_MEMTIM0_WS_6      <<  0u));

    DEF_BIT_SET(BSP_SYS_REG_PLLFREQ0, BSP_PLLFREQ0_PLLPWR);     /* PLL power is applied.                                */

                                                                /* Wait for PLL to power and lock.                      */
    while (DEF_BIT_IS_CLR(BSP_SYS_REG_PLLSTAT, BSP_PLLSTAT_LOCK) == DEF_YES) {
        ;
    }

                                                                /* SysClk = Fvco / (PSYSDIV + 1 )                       */
                                                                /*        = 480MHz / (3 + 1) = 120MHz                   */
    BSP_SYS_REG_RSCLKCFG |= ((BSP_CFG_PLL_SYS_PSYSDIV + 1u)  |
                              BSP_RSCLKCFG_USEPLL            |
                              BSP_RSCLKCFG_MEMTIMU);            /* Update memory timing to match running from Sysclk    */
}


/*
*********************************************************************************************************
*                                         SYSTEM CLOCK FREQUENCY
*
* Description: This function is used to retrieve system or CPU clock frequency.
*
* Arguments  : None
*
* Return     : System clock frequency in cycles.
*
* Caller(s)  : Application.
*
* Note(s)    : None
*********************************************************************************************************
*/

CPU_INT32U  BSP_SysClkFreqGet (void)
{
    CPU_INT32U  pll_mdiv;
    CPU_INT16U  pll_mint;
    CPU_INT16U  pll_mfrac;
    CPU_INT16U  pll_q;
    CPU_INT16U  pll_n;
    CPU_INT32U  sys_div;
    CPU_INT32U  sys_freq;
    CPU_INT32U  pll_src;

    if ((DEF_BIT_IS_SET(BSP_SYS_REG_RSCLKCFG, BSP_RSCLKCFG_USEPLL)) == DEF_YES) {
        pll_mfrac = (BSP_SYS_REG_PLLFREQ0 >> 10u) & 0x07FFu;
        pll_mint  = (BSP_SYS_REG_PLLFREQ0       ) & 0x07FFu;
        pll_mdiv  = (pll_mint + (pll_mfrac / 1024u));

        pll_q = (BSP_SYS_REG_PLLFREQ1 >> 8) & 0x1Fu;
        pll_n = (BSP_SYS_REG_PLLFREQ1     ) & 0x1Fu;

        pll_src = (BSP_SYS_REG_RSCLKCFG >> 24u) & 0x0Fu;

        switch (pll_src) {
            case 0:
                 pll_src = BSP_CFG_SYS_INT_CLK_FREQ;
                 break;


            case 3:
                 pll_src = BSP_CFG_SYS_EXT_CLK_FREQ;
                 break;


            default:
                 break;
        }

        sys_freq = ((pll_src * pll_mdiv) / ((pll_q + 1u) * (pll_n + 1u)));

        sys_div = (BSP_SYS_REG_RSCLKCFG & BSP_RSCLKCFG_PSYSDIV_MASK);

        sys_freq = sys_freq / sys_div;

    } else {
        sys_freq = 0;
    }

    return (sys_freq);
}
