#ifndef Error_H
#define Error_H
enum Error //Error codes:0=success, 1-50=display, 51-100=real time clock
{
    SUCCESS, //Successfully completed operation
    FAIL,    //Generic failure

    DISPLAY_NOT_FOUND = 2, //Display RA8876 chip not responding
    DISPLAY_PLL_FAIL,      //PLL status fail
    DISPLAY_SDRAM_FAIL,    //SDRAM status fail

    RTC_RESET = 51, //RTC power was lost, reset time
    RTC_NOT_FOUND,
};

#endif