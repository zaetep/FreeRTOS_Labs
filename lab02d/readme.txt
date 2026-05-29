FreeRTOS Example

This application demonstrates the use of FreeRTOS on Launchpad.

The application blinks the user-selected LED at a user-selected frequency.

The UART outputs the application status at 115,200 baud,
8-n-1 mode.

This application utilizes FreeRTOS to perform the tasks in a concurrent
fashion.  The following tasks are created:

- An LED task, which blinks the user-selected on-board LED at 500 ms interval.

In addition to the tasks, this application also uses the following FreeRTOS
resources:

- A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
  have nothing to do.

For additional details on FreeRTOS, refer to the FreeRTOS web page at:
http://www.freertos.org/

-------------------------------------------------------------------------------
