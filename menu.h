/*****************************************************************************
 * @file      menu.h
 * @brief     Admin Menu Header File for LPC2148 Smart Railway Platform
 *****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include "types.h"

u8 password(void);
void Time(void);
void DATE(void);
void DAY(void);
void edit_time(void);
void edit_train_details(void);
void edit_password(void);
void RunAdminDashboard(void);

#endif /* MENU_H */
