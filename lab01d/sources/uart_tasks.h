// ENTIRE FILE COMPLETED BY STUDENT

#ifndef UART_TASKS_H
#define UART_TASKS_H

void ConfigureUART(void);
bool UART_CommandAvailable(void);
void UART_ReadCommand(char *dest);
void CMD_Run(char *cmd);

#endif