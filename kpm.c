/*****************************************************************************
 * @file     kpm.c
 * @brief    Keypad Matrix Driver Source File for LPC2148
 * @details  Implements keypad scanning and number parsing using legacy
 *           registers (IODIR1, IOPIN1, IOCLR1) and nibble-level macros,
 *           configured with an option/choice layout (A, B, C, D).
 *****************************************************************************/

#include <lpc21xx.h>
#include "types.h"
#include "all_peripheral_defines.h"
#include "kpm.h"

/* 4x4 Keypad Layout Lookup Table incorporating choice keys (A, B, C, D) */
u8 KPMLUT[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* Internal helper function to check if any key is pressed across the column nibble */
static u32 ColScan(void) {
    if (READNIBBLE(IOPIN1, COL0) < 15)
        return 0;
    else
        return 1;
}

/* Initialize Keypad Matrix Pins (Rows as outputs using legacy IODIR1) */
void Init_KPM(void) {
    /* Configure rows (P0.4-P0.11) as outputs using nibble write macro */
    WRITENIBBLE(IODIR1, ROW0, 15);
}

/* Identify which row contains the pressed key */
u32 RowCheck(void) {
    u32 rno;
    for (rno = 0; rno < 4; rno++) {
        WRITENIBBLE(IOPIN1, ROW0, (~(1 << rno)));
        if (ColScan() == 0)
            break;
    }
    /* Reset rows to default state using legacy IOCLR1 */
    IOCLR1 = 15 << ROW0;
    return rno;
}

/* Identify which column contains the pressed key */
u32 ColCheck(void) {
    u32 cno;
    for (cno = 0; cno < 4; cno++) {
        if (READBIT(IOPIN1, (cno + COL0)) == 0)
            break;
    }
    return cno;
}

/* Scan and return the character corresponding to the pressed key */
u8 KeyScan(void) {
    u32 rno, cno;
    u8 key;

    /* Wait for switch press */
    while (ColScan());

    /* Find row and column coordinates */
    rno = RowCheck();
    cno = ColCheck();

    /* Collect character from lookup table */
    key = KPMLUT[rno][cno];

    /* Wait for switch release */
    while (!ColScan());

    return key;
}

/* Read consecutive digit keypresses with 'C' as Backspace, 'D' as Delete/Clear, and validation against empty input on Enter */
u32 ReadNum(void) {
    u8 key;
    u32 num = 0;
    u8 cnt = 0;

input_restart:
    num = 0;
    cnt = 0;

    while (1) {
        key = KeyScan();

        if (key >= '0' && key <= '9') {
            if (cnt < 12) {
                num = (num * 10) + (key - '0');
                CharLCD(key);
                cnt++;
            }
        }
        else if (key == 'C') {
            if (cnt > 0) {
                num /= 10;
                cnt--;
                CmdLCD(0x10); CharLCD(' '); CmdLCD(0x10);
            }
            if (cnt == 0) goto input_restart;
        }
        else if (key == 'D') {
            goto clear_and_restart;
        }
        else {
            if (cnt == 0) goto input_restart; // Prevent returning 0 on empty input
            break;
        }
    }
    return num;

clear_and_restart:
    while (cnt > 0) {
        CmdLCD(0x10); CharLCD(' '); CmdLCD(0x10);
        cnt--;
    }
    goto input_restart;
}


//read password
u8 ReadPass(void) {
    u8 key;
    u32 num = 0;
    u8 cnt = 0;

input_restart:
    num = 0;
    cnt = 0;

    while (1) {
        key = KeyScan();

        if (key >= '0' && key <= '9') {
            if (cnt < 4) {
                num = (num * 10) + (key - '0');
                CharLCD('*');
                cnt++;
            }
        }
        else if (key == 'C') {
            if (cnt > 0) {
                num /= 10;
                cnt--;
                CmdLCD(0x10); CharLCD(' '); CmdLCD(0x10);
            }
            if (cnt == 0) goto input_restart;
        }
        else if (key == 'D') {
            goto clear_and_restart;
        }
        else {
            if (cnt == 0) goto input_restart; // Prevent returning 0 on empty password
            break;
        }
    }
    return num;

clear_and_restart:
    while (cnt > 0) {
        CmdLCD(0x10); CharLCD(' '); CmdLCD(0x10);
        cnt--;
    }
    goto input_restart;
}
