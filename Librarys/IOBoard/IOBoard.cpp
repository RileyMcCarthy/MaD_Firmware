#include "IOBoard.h"
#include "simpletools.h"
void testBoard()
{
    low(FORCE_TX);

    print("starting testing...\n");
    while (1)
    {

        print("direction\n");
        high(SERVO_DIR);
        low(SERVO_PUL);
        low(SERVO_ENA);
        pause(1000);
        print("pulse\n");
        high(SERVO_PUL);
        low(SERVO_DIR);
        low(SERVO_ENA);
        pause(1000);
        print("enable\n");
        high(SERVO_ENA);
        low(SERVO_PUL);
        low(SERVO_DIR);
        pause(1000);
    }
}