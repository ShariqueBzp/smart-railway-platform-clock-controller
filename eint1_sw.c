/*****************************************************************************
 * @file      eint1_sw.c
 * @brief     External Interrupt 1 Driver File for Admin Switch (Railway Project)
 * @details   Implements External Interrupt 1 (EINT1) on P0.3 using VIC channel 15,
 *            edge triggering, and volatile flag handling for the admin dashboard.
 *****************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "all_peripheral_defines.h"
#include "eint1_sw.h"

#define EINT1_VIC_CHNO       15
#define EINT1_PIN_FUNC       0x000000C0  /* Alternative function 2 for P0.3 (EINT1) */

/* Global volatile flag to handle admin switch press state for the railway project */
volatile u8 admin_flag = 0;

/* Initialize External Interrupt 1 for Admin Switch on P0.3 */
void Init_EINT1(void) {
    /* Configure P0.3 as EINT1 input pin via PINSEL0 bits [7:6] */
    PINSEL0 &= ~(3UL << 6);          /* Clear bits 7 and 6 for P0.3 */
    PINSEL0 |=  EINT1_PIN_FUNC;      /* Set function 2 (EINT1) for P0.3 */
    
    /* Enable EINT1 via VIC channel 15 */
    VICIntEnable |= (1 << EINT1_VIC_CHNO);
    
    /* Configure EINT1 as Vectored IRQ with priority slot 1 */
    VICVectCntl1 = (1 << 5) | EINT1_VIC_CHNO;
    
    /* Load ISR address into IRQ slot 1 vector address register */
    VICVectAddr1 = (u32)eint1_isr;
    
    /* Configure External Interrupt Peripheral (EIP) */
    /* Set EINT1 as edge-triggered mode */
    EXTMODE |= (1 << 1);
    
    /* Configure EINT1 for falling edge trigger (Admin Switch Press) */
    EXTPOLAR &= ~(1 << 1);
}

/* External Interrupt 1 Service Routine */
void eint1_isr(void) __irq {
		if(READBIT(IOPIN0,ADMIN_EINT_PIN)==0)
		{
    /* Set volatile admin flag on switch press for railway system control logic */
    admin_flag = 1;
		}
    
    /* Clear interrupt status in VIC */
    VICVectAddr = 0;
    
    /* Clear EINT1 status flag in External Interrupt Peripheral (EXTINT) */
    EXTINT = (1 << 1);
}
