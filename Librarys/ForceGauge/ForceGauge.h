#ifndef ForceGauge_H
#define ForceGauge_H

#include "propeller.h"
#include "Error.h"
#include "libpropeller/fullduplexserial/full_duplex_serial.h"
union Data_v
{
    float fval;
    int32_t val;
    uint8_t bval[4];
};
class ForceGauge
{
public:
    Error begin(int rx, int tx);
    int16_t readForce();

private:
    int _rx, _tx;
    libpropeller::FullDuplexSerial serial;
    union Data_v _tem_data;
};
#endif