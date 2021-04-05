#include "MaD.h"

static bool complete;
static MachineState localState;
static bool initialRender;
enum button_names
{
    BUTTON_MOTION_MODE,
    BUTTON_CONDITION,
    BUTTON_NAVIGATION
};

static void buttonHandler(Button b)
{
    switch (b.name)
    {
    case BUTTON_MOTION_MODE:
        /* code */
        break;
    case BUTTON_CONDITION:
        /* code */
        break;
    case BUTTON_NAVIGATION:
        complete = true;
        break;
    default:
        break;
    }
}

void MAD::runManualPage()
{
    complete = false;
    initialRender = true;
    display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);

    display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
    display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X2, RA8876_TEXT_HEIGHT_ENLARGEMENT_X2);
    display.textColor(MAINTEXTCOLOR, BACKCOLOR);

    char buf[50];
    strcpy(buf, "Manual");
    int titlex = SCREEN_WIDTH / 2 - strlen(buf) * 16;
    int titley = 10;
    display.putString(titlex, titley, buf);

    display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
    display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);

    display.textColor(MAINTEXTCOLOR, BACKCOLOR);
    strcpy(buf, "Information");
    titley = 120;
    titlex = SCREEN_WIDTH / 6 - strlen(buf) * 8;
    display.putString(titlex, titley, buf);
    display.drawLine(titlex, titley + 30, titlex + strlen(buf) * 16, titley + 30, MAINTEXTCOLOR);

    display.textColor(MAINTEXTCOLOR, BACKCOLOR);
    strcpy(buf, "Control");
    titley = 300;
    titlex = SCREEN_WIDTH / 6 - strlen(buf) * 8;
    display.putString(titlex, titley, buf);
    display.drawLine(titlex, titley + 30, titlex + strlen(buf) * 16, titley + 30, MAINTEXTCOLOR);

    Button buttons[3];
    buttons[0].name = BUTTON_MOTION_MODE;
    buttons[0].xmin = SCREEN_WIDTH / 6 - 110;
    buttons[0].xmax = buttons[0].xmin + 100;
    buttons[0].ymin = 350;
    buttons[0].ymax = buttons[0].ymin + 50;
    buttons[0].onPress = buttonHandler;

    buttons[1].name = BUTTON_CONDITION;
    buttons[1].xmin = SCREEN_WIDTH / 6 + 10;
    buttons[1].xmax = buttons[1].xmin + 100;
    buttons[1].ymin = 350;
    buttons[1].ymax = buttons[1].ymin + 50;
    buttons[1].onPress = buttonHandler;

    buttons[2].name = BUTTON_NAVIGATION;
    buttons[2].xmin = SCREEN_WIDTH - 100;
    buttons[2].xmax = buttons[2].xmin + 100;
    buttons[2].ymin = 0;
    buttons[2].ymax = buttons[2].ymin + 100;
    buttons[2].onPress = buttonHandler;

    display.drawSquareFill(buttons[0].xmin, buttons[0].ymin, buttons[0].xmax, buttons[0].ymax, COLOR65K_GREEN);
    display.drawSquareFill(buttons[1].xmin, buttons[1].ymin, buttons[1].xmax, buttons[1].ymax, COLOR65K_RED);
    display.bteMemoryCopyImage(navigationImg, SCREEN_WIDTH - navigationImg.width - 5, 5);

    while (!complete)
    {
        MachineState currentState = machineState;
        display.checkButtons(buttons, 3);
        display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_16, RA8876_SELECT_8859_1);
        display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
        int textcolor = 0;
        int backcolor = 0;
        int bordercolor = 0;
        if ((localState.motion.status != currentState.motion.status) || initialRender)
        {
            switch (machineState.motion.status)
            {
            case Motion_Status::STATUS_DISABLED:
                strcpy(buf, "DISABLED");
                textcolor = COLOR65K_WHITE;
                backcolor = COLOR65K_BLACK;
                bordercolor = COLOR65K_GREEN;
                break;
            case Motion_Status::STATUS_ENABLED:
                strcpy(buf, "ENABLED");
                textcolor = COLOR65K_BLACK;
                backcolor = COLOR65K_WHITE;
                bordercolor = COLOR65K_GREEN;
                break;
            case Motion_Status::STATUS_RESTRICTED:
                strcpy(buf, "RESTRICTED");
                textcolor = COLOR65K_WHITE;
                backcolor = COLOR65K_RED;
                bordercolor = COLOR65K_DARKRED;
                break;
            default:
                break;
            }

            display.drawSquareFill(buttons[0].xmin, buttons[0].ymin, buttons[0].xmax, buttons[0].ymax, bordercolor);
            display.drawSquareFill(buttons[0].xmin + 5, buttons[0].ymin + 5, buttons[0].xmax - 5, buttons[0].ymax - 5, backcolor);
            display.textColor(textcolor, backcolor);
            display.putString(buttons[0].xmin + 50 - strlen(buf) * 4, buttons[0].ymin + 25 - 8, buf);
        }
        display.textColor(MAINTEXTCOLOR, BACKCOLOR);
        strcpy(buf, "Relative Position: 0.00");
        display.putString(SCREEN_WIDTH / 6 - strlen(buf) * 6, 160, buf);
        strcpy(buf, "Relative Force: 0.00");
        display.putString(SCREEN_WIDTH / 6 - strlen(buf) * 6, 190, buf);
        strcpy(buf, "Absolute Position: 0.00");
        display.putString(SCREEN_WIDTH / 6 - strlen(buf) * 6, 230, buf);
        strcpy(buf, "Absolute Force: 0.00");
        display.putString(SCREEN_WIDTH / 6 - strlen(buf) * 6, 260, buf);
        clock.render();
        if (initialRender)
        {
            initialRender = false;
        }
        localState = currentState;
    }
    runNavigationPage();
}