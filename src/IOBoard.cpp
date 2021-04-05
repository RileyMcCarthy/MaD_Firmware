#include "IOBoard.h"

void gpioConfig(MCP23017 *gpio)
{
    gpio->begin(GPIO_ADDR, GPIO_SDA, GPIO_SCL); // use default address 0
    gpio->dira(GPIO_DIR_MASKA);
    gpio->dirb(GPIO_DIR_MASKB);
}

uint8_t gpi(MCP23017 *gpio, int pin)
{
    if (pin >= 9 && pin <= 12) //reg a
    {
        return (gpio->outa() & (1 << (pin - 9))) >> (pin - 9);
    }
    else if (pin >= 1 && pin <= 8) //reg b
    {
        return (gpio->outb() & (1 << (pin - 1))) >> (pin - 1);
    }
    return NULL;
}