#include "ForceGauge.h"

Error ForceGauge::begin(int rx, int tx)
{
    serial.Start(rx, tx, 1, 19200);
}

int16_t ForceGauge::readForce()
{
    int16_t ret = 0;
    int rec = serial.Get(-1);
    if (rec == -1)
    {
        return NULL;
    }
    _tem_data.bval[0] = rec;
    rec = serial.Get(-1);
    _tem_data.bval[1] = rec;
    if (rec == -1)
    {
        return NULL;
    }
    if (_tem_data.val >= 8192)
    {
        ret = 8192 - _tem_data.val;
    }
    else
    {
        ret = _tem_data.val;
    }
    return (ret);
}