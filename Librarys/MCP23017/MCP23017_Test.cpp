#include "MCP23017.h"
MCP23017 mcp;

void testPins()
{
    pause(1000);
    mcp.begin(0, 26, 0); // use default address 0
    mcp.dira(0x00);
    while (1)
    {
        //   print("looping\n");
        pause(2000);
        mcp.outa(0x00);
        pause(2000);
        mcp.outa(0xFF);
    }
}