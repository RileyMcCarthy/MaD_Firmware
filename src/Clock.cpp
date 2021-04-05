#include "Clock.h"
//Clock
Error Clock::begin(Ra8876_Lite *theDisplay)
{
    Error error;
    display = theDisplay;
    error = rtc.begin(RTC_SCL, RTC_SDA);
    if (error == RTC_RESET) //set time
    {
        Time newTime;
        newTime = getDateTime();
        //add time check here
        rtc.setTime(&newTime);
    }
}

Error Clock::render()
{
    display->setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_16, RA8876_SELECT_8859_1);
    display->setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
    display->textColor(MAINTEXTCOLOR, BACKCOLOR);
    rtc.updateTime();
    char buf[25];
    sprintf(buf, "%04d-%02d-%02d  %02d:%02d:%02d", rtc.time.year, rtc.time.month, rtc.time.day, rtc.time.hour, rtc.time.minute, rtc.time.second);
    display->putString(2, 0, buf);
    display->textColor(COLOR65K_WHITE, MAINCOLOR);
    return SUCCESS;
}
enum buttonType
{
    submit,
    up,
    down,
    left,
    right
};

static int values[6];
static int index;
static bool complete;
static void buttonPressed(Button button)
{
    switch (button.name)
    {
    case buttonType::up:
        values[index]++;
        break;
    case buttonType::down:
        values[index]--;
        break;
    case buttonType::left:
        if (index > 0)
        {
            index--;
        }
        break;
    case buttonType::right:
        if (index < 10)
        {
            index++;
        }
        break;
    case buttonType::submit:
        complete = true;
        break;
    default:
        break;
    }
}

Time Clock::getDateTime()
{
    values[6];
    index = 0;
    values[0] = 2021;
    complete = false;

    TouchLocation touchLocation[1];
    display->setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
    display->setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
    display->drawSquareFill(256, 150, 512 + 256, 300 + 150, COLOR65K_WHITE);
    display->textColor(0x00, COLOR65K_WHITE);
    display->putString(448, 160, "Set Date");

    int width = 50;
    int padding = 10;
    Button keys[5];
    keys[0].xmin = 320;
    keys[0].xmax = keys[0].xmin + width;
    keys[0].ymin = 280;
    keys[0].ymax = keys[0].ymin + width;
    keys[0].onPress = buttonPressed;
    keys[0].name = up;
    display->drawSquareFill(keys[0].xmin, keys[0].ymin, keys[0].xmax, keys[0].ymax, 0x00);
    display->drawTriangleFill(keys[0].xmin + padding, keys[0].ymax - padding, keys[0].xmax - padding, keys[0].ymax - padding, keys[0].xmin + width / 2, keys[0].ymin + padding, 0xFFFF);

    keys[1].xmin = keys[0].xmin;
    keys[1].xmax = keys[1].xmin + width;
    keys[1].ymin = keys[0].ymin + width * 2;
    keys[1].ymax = keys[1].ymin + width;
    keys[1].onPress = buttonPressed;
    keys[1].name = down;
    display->drawSquareFill(keys[1].xmin, keys[1].ymin, keys[1].xmax, keys[1].ymax, 0x00);
    display->drawTriangleFill(keys[1].xmin + padding, keys[1].ymin + padding, keys[1].xmax - padding, keys[1].ymin + padding, keys[1].xmin + width / 2, keys[1].ymax - padding, 0xFFFF);

    keys[2].xmin = keys[0].xmin - width;
    keys[2].xmax = keys[2].xmin + width;
    keys[2].ymin = keys[0].ymin + width;
    keys[2].ymax = keys[2].ymin + width;
    keys[2].onPress = buttonPressed;
    keys[2].name = left;
    display->drawSquareFill(keys[2].xmin, keys[2].ymin, keys[2].xmax, keys[2].ymax, 0x00);
    display->drawTriangleFill(keys[2].xmin + padding, keys[2].ymin + width / 2, keys[2].xmax - padding, keys[2].ymax - padding, keys[2].xmax - padding, keys[2].ymin + padding, 0xFFFF);

    keys[3].xmin = keys[0].xmin + width;
    keys[3].xmax = keys[3].xmin + width;
    keys[3].ymin = keys[0].ymax;
    keys[3].ymax = keys[3].ymin + width;
    keys[3].onPress = buttonPressed;
    keys[3].name = right;
    display->drawSquareFill(keys[3].xmin, keys[3].ymin, keys[3].xmax, keys[3].ymax, 0x00);
    display->drawTriangleFill(keys[3].xmax - padding, keys[3].ymin + width / 2, keys[3].xmin + padding, keys[3].ymax - padding, keys[3].xmin + padding, keys[3].ymin + padding, 0xFFFF);

    keys[4].xmin = 650;
    keys[4].xmax = keys[4].xmin + 100;
    keys[4].ymin = 380;
    keys[4].ymax = keys[4].ymin + width;
    keys[4].onPress = buttonPressed;
    keys[4].name = submit;
    display->drawSquareFill(keys[4].xmin, keys[4].ymin, keys[4].xmax, keys[4].ymax, 0x00);
    display->textColor(COLOR65K_WHITE, 0x00);
    display->putString(keys[4].xmin + padding, keys[4].ymin + padding, "Enter");

    display->textColor(0x00, COLOR65K_WHITE);
    while (!complete)
    {
        display->checkButtons(keys, 5);
        char dateTime[50];
        int startingindex = 300;
        for (int i = 0; i < 6; i++)
        {
            char buf[8];
            if (i == index)
            {
                display->textColor(COLOR65K_WHITE, COLOR65K_GRAYSCALE10);
            }
            else
            {
                display->textColor(0x00, COLOR65K_WHITE);
            }
            if (i == 0)
            {
                strcpy(buf, "%04d-");
            }
            else if (i == 1)
            {
                strcpy(buf, "%02d-");
            }
            else if (i == 2)
            {
                strcpy(buf, "%02d  ");
            }
            else if (i == 3)
            {
                strcpy(buf, "%02d:");
            }
            else if (i == 4)
            {
                strcpy(buf, "%02d:");
            }
            else if (i == 5)
            {
                strcpy(buf, "%02d");
            }
            sprintf(dateTime, buf, values[i]);
            display->putString(startingindex, 220, dateTime);
            startingindex += strlen(dateTime) * 16;
        }
        waitcnt(CNT + CLKFREQ / 100);
    }
    Time newTime;
    newTime.year = values[0];
    newTime.month = values[1];
    newTime.day = values[2];
    newTime.hour = values[3];
    newTime.minute = values[4];
    newTime.second = values[5];
    return newTime;
}