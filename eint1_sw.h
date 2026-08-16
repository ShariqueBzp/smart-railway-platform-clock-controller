/*****************************************************************************
 * @file      eint1_sw.h
 * @brief     External Interrupt 1 Driver Header File for Admin Switch (Railway Project)
 * @details   Prototypes for EINT1 initialization, ISR, and global volatile flag.
 *****************************************************************************/

#ifndef EINT1_SW_H
#define EINT1_SW_H

#include "types.h"

/* Global volatile flag to handle admin switch press state */
extern volatile u8 admin_flag;

/* Function Prototypes */
void Init_EINT1(void);
void eint1_isr(void) __irq;

#endif /* EINT1_SW_H */
