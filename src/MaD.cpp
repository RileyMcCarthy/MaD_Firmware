#include "MaD.h"

void MAD::begin()
{
  printf("Starting...\n");
  //setup display
  startDisplay();

  /*State testing configuration*/
  machineState.update(); //update will pass selfcheck then go to machineCheck
  machineState.machineCheck.power = true;
  machineState.machineCheck.upperLimit = true;
  machineState.machineCheck.lowerLimit = true;
  machineState.machineCheck.esd = true;
  machineState.machineCheck.servoReady = true;
  machineState.machineCheck.forceGaugeResponding = true;
  /*end of State testing configuration*/

  //setup real time clock
  clock.begin(&display);

  //setup gpio
  gpioConfig(&gpio);

  //load assets
  loadAssets();
  getKeyboardInput(buf);
  //start status page
  //display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);
  runStatusPage();
}

Error MAD::startDisplay()
{
  Error err;
  display.setSD(MAD_SD_DO, MAD_SD_CLK, MAD_CD_DI, MAD_CD_CS); // Micro SD on MaD board

  //turn on diplay
  if ((err = display.begin(RA8876_XNSCS, RA8876_XNRESET, CLK, DATA, GT9271_INT, BACKLIGHT)) != SUCCESS)
  {
    return err;
  }
  display.displayOn(true);

  //Init display and background
  display.canvasImageStartAddress(PAGE1_START_ADDR);
  display.canvasImageWidth(SCREEN_WIDTH);
  display.activeWindowXY(0, 0);
  display.activeWindowWH(SCREEN_WIDTH, SCREEN_HEIGHT);
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);
  return Error::SUCCESS;
}

void MAD::loadAssets()
{
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, COLOR65K_BLUE);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
  display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  display.textColor(COLOR65K_WHITE, COLOR65K_BLUE);
  display.putString(SCREEN_WIDTH / 2 - 11 * 8, 300 - 16, "LOADING...");

  //display.loadImage(keyboardImg);
  display.loadImage(navigationImg);
  display.loadImage(button_wide);
  display.loadImage(manualImg);
  display.loadImage(automaticImg);
}

void drawGraph(Ra8876_Lite display);
i2cNavKey navkey(0b0010010);

void UP_Button_Pressed(i2cNavKey *p)
{
  int temp = navkey.readCounterInt() + 10;
  navkey.writeCounter(temp);
}

void DOWN_Button_Pressed(i2cNavKey *p)
{
  int temp = navkey.readCounterInt() - 10;
  navkey.writeCounter(temp);
}

void LEFT_Button_Pressed(i2cNavKey *p)
{
}

void RIGHT_Button_Pressed(i2cNavKey *p)
{
}

void CENTRAL_Button_Pressed(i2cNavKey *p)
{
}

void CENTRAL_Button_Double(i2cNavKey *p)
{
}

void Encoder_Rotate(i2cNavKey *p)
{
}

void loadGraph(Ra8876_Lite display)
{
  display.textColor(COLOR65K_WHITE, MAINCOLOR);
  display.canvasImageStartAddress(PAGE4_START_ADDR);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_16, RA8876_SELECT_8859_1);
  display.drawSquareFill(492, 32, SCREEN_WIDTH - 32, 32 + 300, MAINCOLOR);
  display.drawLine(492 + 5, 32 + 5, 492 + 5, 332 - 5, COLOR65K_WHITE);
  display.drawLine(492 + 5, 182, SCREEN_WIDTH - 32 - 5, 182, COLOR65K_WHITE);
  display.putString(492 + 10, 32 + 5, "Force(N)");
  display.putString(SCREEN_WIDTH - 32 - strlen("Time(s)") * 8, 182 - 20, "Time(s)");
  display.canvasImageStartAddress(PAGE1_START_ADDR);
}
void drawGraph(Ra8876_Lite display)
{
  display.bteMemoryCopy(PAGE4_START_ADDR, SCREEN_WIDTH, 492, 32, PAGE1_START_ADDR, SCREEN_WIDTH, 492, 32, 500, 300);
}
void MAD::beginOLD()
{
  display.setSD(MAD_SD_DO, MAD_SD_CLK, MAD_CD_DI, MAD_CD_CS);
  //setup navkey
  navkey.pins(28, 29);
  navkey.reset();
  navkey.begin(i2cNavKey::INT_DATA | i2cNavKey::WRAP_ENABLE | i2cNavKey::DIRE_RIGHT | i2cNavKey::IPUP_ENABLE);

  navkey.writeCounter((int32_t)0); /* Reset the counter value */
  navkey.writeMax((int32_t)100);   /* Set the maximum threshold*/
  navkey.writeMin((int32_t)0);     /* Set the minimum threshold */
  navkey.writeStep((int32_t)1);    /* Set the step to 1*/

  navkey.writeDoublePushPeriod(30); /*Set a period for the double push of 300ms */

  navkey.onUpPush = UP_Button_Pressed;
  navkey.onDownPush = DOWN_Button_Pressed;
  navkey.onRightPush = RIGHT_Button_Pressed;
  navkey.onLeftPush = LEFT_Button_Pressed;
  navkey.onCentralPush = CENTRAL_Button_Pressed;
  navkey.onCentralDoublePush = CENTRAL_Button_Double;
  navkey.onChange = Encoder_Rotate;

  navkey.autoconfigInterrupt(); /* Enable the interrupt with the attached callback */

  //turn on diplay
  if (!display.begin(RA8876_XNSCS, RA8876_XNRESET, CLK, DATA, GT9271_INT, BACKLIGHT))
  {
    while (1)
      ;
  }
  display.displayOn(true);

  //Init display with blue background
  display.canvasImageStartAddress(PAGE1_START_ADDR);
  display.canvasImageWidth(SCREEN_WIDTH);
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);

  clock.begin(&display);

  display.activeWindowXY(0, 0);
  display.activeWindowWH(SCREEN_WIDTH, SCREEN_HEIGHT);
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, COLOR65K_BLUE);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
  display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  display.textColor(COLOR65K_WHITE, COLOR65K_BLUE);
  display.putString(SCREEN_WIDTH / 2 - 11 * 8, 300 - 16, "LOADING...");
  // display.loadImage("keyboard.bin", 3, 0, SCREEN_HEIGHT - 284, 1026, 284); //loading images adds another 10k bytes (maybe make own sd card system? or fopen)
  loadGraph(display);
  waitcnt(CNT + CLKFREQ / 2);
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);
  //setup real time clock

  //Clock
  char temp[20];
  display.textColor(COLOR65K_WHITE, BACKCOLOR);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
  display.putString(SCREEN_WIDTH / 2 - strlen("Enter Test Name") * 8, 150, "Enter Test Name");
  //getKeyboardInput(display, temp);
  display.textColor(COLOR65K_WHITE, COLOR65K_BLUE);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
  display.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE, RA8876_TEXT_WIDTH_ENLARGEMENT_X1, RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  char forceVal[100] = {10};
  display.drawSquareFill(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BACKCOLOR);
  //display.bteMemoryCopy(PAGE2_START_ADDR, SCREEN_WIDTH, (SCREEN_WIDTH - 500), 0, PAGE1_START_ADDR, SCREEN_WIDTH, (SCREEN_WIDTH - 500), 0, 500, 300);

  int samples = 0;

  display.drawSquareFill(32, 32, 32 + 428, SCREEN_HEIGHT - 32, MAINCOLOR);
  display.textColor(COLOR65K_WHITE, MAINCOLOR);
  int titlex = 246 - strlen(temp) * 8;
  int titley = 47;
  display.putString(titlex, titley, temp);
  display.drawLine(titlex, titley + 35, titlex + strlen(temp) * 16, titley + 35, COLOR65K_WHITE);

  titlex = 246 - strlen("Controls") * 8;
  titley = 47 + 240;
  display.putString(titlex, titley, "Controls");
  display.drawLine(titlex, titley + 35, titlex + strlen("Controls") * 16, titley + 35, COLOR65K_WHITE);

  display.drawSquareFill(492, 364, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, MAINCOLOR);
  titlex = 742 - strlen("Stats") * 8;
  titley = 375;
  display.textColor(COLOR65K_WHITE, MAINCOLOR);
  display.putString(titlex, titley, "Stats");
  display.drawLine(titlex, titley + 35, titlex + strlen("Stats") * 16, titley + 35, COLOR65K_WHITE);

  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_16, RA8876_SELECT_8859_1);
  display.putString(55, 47 + 50, "Mode: Constant Force");
  display.putString(55, 47 + 70, "Cycles: 20");
  display.putString(55, 47 + 90, "Sample Rate: 800sps");

  int width = 50;
  int padding = 10;
  int color1 = COLOR65K_GRAYSCALE20;
  int color2 = COLOR65K_WHITE;
  Button keys[5];
  keys[0].xmin = 100;
  keys[0].xmax = keys[0].xmin + width;
  keys[0].ymin = 350;
  keys[0].ymax = keys[0].ymin + width;
  keys[0].onPress = NULL;
  display.drawSquareFill(keys[0].xmin, keys[0].ymin, keys[0].xmax, keys[0].ymax, color1);
  display.drawTriangleFill(keys[0].xmin + padding, keys[0].ymax - padding, keys[0].xmax - padding, keys[0].ymax - padding, keys[0].xmin + width / 2, keys[0].ymin + padding, color2);

  keys[1].xmin = keys[0].xmin;
  keys[1].xmax = keys[1].xmin + width;
  keys[1].ymin = keys[0].ymin + width * 2 + padding * 2;
  keys[1].ymax = keys[1].ymin + width;
  keys[1].onPress = NULL;
  display.drawSquareFill(keys[1].xmin, keys[1].ymin, keys[1].xmax, keys[1].ymax, color1);
  display.drawTriangleFill(keys[1].xmin + padding, keys[1].ymin + padding, keys[1].xmax - padding, keys[1].ymin + padding, keys[1].xmin + width / 2, keys[1].ymax - padding, color2);

  keys[2].xmin = keys[0].xmin;
  keys[2].xmax = keys[2].xmin + width;
  keys[2].ymin = keys[0].ymin + width + padding;
  keys[2].ymax = keys[2].ymin + width;
  keys[2].onPress = NULL;
  display.drawSquareFill(keys[2].xmin, keys[2].ymin, keys[2].xmax, keys[2].ymax, color1);
  display.textColor(color2, color1);
  display.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM, RA8876_CHAR_HEIGHT_32, RA8876_SELECT_8859_1);
  display.putString(keys[2].xmin + 17, keys[2].ymin + 9, "H");

  drawGraph(display);
  float total = 0;
  int max = 0;
  int min = 0;
  while (1)
  {
    navkey.updateStatus();
    clock.render();
    display.textColor(COLOR65K_WHITE, MAINCOLOR);
    char pos[15];
    sprintf(pos, "Position: %dmm   ", navkey.readCounterInt());

    display.putString(160, 350, pos);
    int tempForce = force.readForce();
    if (tempForce != NULL)
    {
      samples++;
      total += (float)tempForce / 1000.0;
      if (tempForce > max)
      {
        max = tempForce;
      }
      else if (tempForce < min)
      {
        min = tempForce;
      }
      display.drawSquareFill(SCREEN_WIDTH - strlen(forceVal) * 8 - 8, 0, SCREEN_WIDTH, 35, COLOR65K_BLUE);
      sprintf(forceVal, "Force: %0.3f ", tempForce / 1000.0);
      display.putString(575, 420, forceVal);
      float avg = total / ((float)samples);
      sprintf(forceVal, "Average: %0.3f ", avg);
      display.putString(575, 450, forceVal);
      sprintf(forceVal, "Min: %0.3f ", min / 1000.0);
      display.putString(575, 480, forceVal);
      sprintf(forceVal, "Max: %0.3f ", max / 1000.0);
      display.putString(575, 510, forceVal);
      display.drawLine(498 + samples, 182, 498 + samples, 182 + tempForce / 34, COLOR65K_GREEN); //force is -5000 to 5000 with 300px resolution
      if (samples >= 445)
      {
        drawGraph(display);
        samples = 0;
      }
    }
    waitcnt(CNT + CLKFREQ / 100);
  }
}