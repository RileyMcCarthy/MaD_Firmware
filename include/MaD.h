
#ifndef MaD_H
#define MaD_H
#include "simpletools.h"
#include "Clock.h"
#include "IOBoard.h"
#include "MCP23017.h"
#include "DS3231.h"
#include "State.h"
#include "Style.h"
#include "Ra8876_Lite.h"
#include "i2cNavKey.h"
#include "ForceGauge.h"

#define GT9271_INT 7
#define CLK 28
#define DATA 29
#define RA8876_XNSCS 10
#define RA8876_XNRESET 9
#define BACKLIGHT 8

#define MAD_SD_DO 22
#define MAD_SD_CLK 23
#define MAD_CD_DI 24
#define MAD_CD_CS 25

class MAD
{
public:
    MAD()
    {
        strcpy(keyboardImg.name, "keyboard.bin");
        keyboardImg.page = 3;
        keyboardImg.width = 1026;
        keyboardImg.height = 284;
        keyboardImg.x0 = 0;
        keyboardImg.y0 = SCREEN_HEIGHT - keyboardImg.height;
        keyboardImg.backgroundColor = NULL;

        strcpy(navigationImg.name, "nav.bin");
        navigationImg.page = 3;
        navigationImg.width = 100;
        navigationImg.height = 100;
        navigationImg.x0 = 0;
        navigationImg.y0 = 0;
        navigationImg.backgroundColor = 0xf800;

        strcpy(button_wide.name, "btnw.bin");
        button_wide.page = 3;
        button_wide.width = 100;
        button_wide.height = 50;
        button_wide.x0 = 150;
        button_wide.y0 = 0;
        button_wide.backgroundColor = 0xf800;

        strcpy(manualImg.name, "manual.bin");
        manualImg.page = 3;
        manualImg.width = 200;
        manualImg.height = 200;
        manualImg.x0 = 300;
        manualImg.y0 = 0;
        manualImg.backgroundColor = 0xf800;

        strcpy(automaticImg.name, "auto.bin");
        automaticImg.page = 3;
        automaticImg.width = 200;
        automaticImg.height = 200;
        automaticImg.x0 = 600;
        automaticImg.y0 = 0;
        automaticImg.backgroundColor = 0xf800;
    }
    void begin();
    void runStatusPage();
    void runNavigationPage();
    void runManualPage();
    void getKeyboardInput(char *buf);
    void loadAssets();

private:
    Error startDisplay();

    //utility objects
    Ra8876_Lite display;
    ForceGauge force;
    Clock clock;
    MachineState machineState;
    MCP23017 gpio;
    char buf[50];

    void beginOLD(); //tmep

    //Images
    Image keyboardImg;
    Image navigationImg;
    Image button_wide;
    Image button_wide_border;
    Image manualImg;
    Image automaticImg;
};
#endif