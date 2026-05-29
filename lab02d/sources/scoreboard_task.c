// ENTIRE FILE COMPLETED BY STUDENT

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <globalvars.h>
#include <priorities.h>

#include <ST7735.h>

#define SCOREBOARDTASKSTACKSIZE 512

void vScoreboardTask(void *pvParameters) {
    const UBaseType_t arraySize = uxTaskGetNumberOfTasks();
    TaskStatus_t *taskArray = pvPortMalloc(arraySize * sizeof(TaskStatus_t));
    uint32_t totalRunTime;
    size_t len;
    char *title = "Task| State | Prio | ExecTime | StackLeft";
    ST7735_Message(0, 0, title, strlen(title));

    if (taskArray == NULL) {
        while(1); // Cannot proceed if allocation fails
    }

    for (;;) {
        UBaseType_t taskCount = uxTaskGetSystemState(taskArray, arraySize, &totalRunTime);

        xQueueSend(xUARTQueue, title, 0);
        for (UBaseType_t i = 0; i < taskCount; i++) {
            /* allocate message buffer - receiver must vPortFree() after using it */
            char *msg = pvPortMalloc(MAX_STRING_LEN);
            if (msg != NULL) {
                snprintf(msg, MAX_STRING_LEN,
                         "%s | %d | %lu | %lu | %lu\n",
                         taskArray[i].pcTaskName,
                         taskArray[i].eCurrentState,
                         (unsigned long)taskArray[i].uxCurrentPriority,
                         (unsigned long)taskArray[i].ulRunTimeCounter,
                         (unsigned long)taskArray[i].usStackHighWaterMark);
                len = strlen(msg);
                ST7735_Message(0, i+1, msg, len);
                if (xQueueSend(xUARTQueue, msg, 0) != pdPASS) { // send to UART for debugging
                    /* if queue full/fail, free buffer to avoid leak */
                    vPortFree(msg);
                }
                /* receiver must vPortFree() after using it */
            }
            
        }
        
        /* allocate newline - receiver must vPortFree() after using it */
        char *newLinePtr = pvPortMalloc(MAX_STRING_LEN);
        if (newLinePtr != NULL) {
            snprintf(newLinePtr, MAX_STRING_LEN, "\n");
            xQueueSend(xUARTQueue, &newLinePtr, 0);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // update every second
    }
}

uint32_t
ScoreboardTaskInit(void) {
    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    //
    // Create the LCD task.
    //
    if(xTaskCreate(vScoreboardTask, (const portCHAR *)"LCD   ", SCOREBOARDTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_SCOREBOARD_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
