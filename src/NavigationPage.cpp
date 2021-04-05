#include "MaD.h"

#define BUTTON_COUNT 3

enum pageNav
{
    PAGE_STATUS,
    PAGE_MANUAL,
    PAGE_AUTOMATIC,
    PAGE_SETTINGS
};
static bool complete = false;
static pageNav newPage;

void buttonHandler(Button button)
{
    switch (button.name)
    {
    case pageNav::PAGE_STATUS:
        complete = true;
        newPage = pageNav::PAGE_STATUS;
        break;
    case pageNav::PAGE_MANUAL:
        complete = true;
        newPage = pageNav::PAGE_MANUAL;
        break;
    default:
        break;
    }
}

void MAD::runNavigationPage()
{
    complete = false;
    display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);

    display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
    display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X2, RA8876_TEXT_HEIGHT_ENLARGEMENT_X2);
    display.textColor(MAINTEXTCOLOR, BACKCOLOR);

    int buttonSize = 200;

    char buf[50];
    strcpy(buf, "Navigation Menu");
    int titlex = SCREEN_WIDTH / 2 - strlen(buf) * 16;
    int titley = 30;
    display.putString(titlex, titley, buf);

    Button buttons[BUTTON_COUNT];
    buttons[0].name = pageNav::PAGE_STATUS;
    buttons[0].xmin = SCREEN_WIDTH / 6 - buttonSize / 2;
    buttons[0].xmax = buttons[0].xmin + buttonSize;
    buttons[0].ymin = 120;
    buttons[0].ymax = buttons[0].ymin + buttonSize;
    buttons[0].onPress = buttonHandler;

    buttons[1].name = pageNav::PAGE_MANUAL;
    buttons[1].xmin = buttons[0].xmax + 30;
    buttons[1].xmax = buttons[1].xmin + buttonSize;
    buttons[1].ymin = 120;
    buttons[1].ymax = buttons[1].ymin + buttonSize;
    buttons[1].onPress = buttonHandler;

    buttons[2].name = pageNav::PAGE_AUTOMATIC;
    buttons[2].xmin = buttons[1].xmax + 30;
    buttons[2].xmax = buttons[2].xmin + buttonSize;
    buttons[2].ymin = 120;
    buttons[2].ymax = buttons[2].ymin + buttonSize;
    buttons[2].onPress = buttonHandler;

    display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
    display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);

    display.drawSquareFill(buttons[0].xmin, buttons[0].ymin, buttons[0].xmax, buttons[0].ymax, MAINCOLOR);
    display.textColor(MAINTEXTCOLOR, MAINCOLOR);
    strcpy(buf, "Status");
    display.putString(buttons[0].xmin + buttonSize / 2 - strlen(buf) * 8, buttons[0].ymin + buttonSize / 2 - 12, buf);

    display.bteMemoryCopyImage(manualImg, buttons[1].xmin, buttons[1].ymin);

    display.bteMemoryCopyImage(automaticImg, buttons[2].xmin, buttons[2].ymin);

    while (!complete)
    {
        display.checkButtons(buttons, BUTTON_COUNT);
        clock.render();
    }
    switch (newPage)
    {
    case pageNav::PAGE_STATUS:
        runStatusPage();
        break;
    case pageNav::PAGE_MANUAL:
        runManualPage();
        break;
    default:
        break;
    }
    runNavigationPage();
}