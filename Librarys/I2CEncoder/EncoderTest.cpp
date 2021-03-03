#include "i2cEncoderLibV2.h"

const int IntPin = 3;
i2cEncoderLibV2 Encoder(0x01);

void testEncoder()
{
    print("**** I2C Encoder V2 basic example ****\n");
    /*
    INT_DATA= The register are considered integer.
    WRAP_DISABLE= The WRAP option is disabled
    DIRE_LEFT= Encoder left direction increase the value
    IPUP_ENABLE= INT pin have the pull-up enabled.
    RMOD_X1= Encoder configured as X1.
    RGB_ENCODER= type of encoder is RGB, change to STD_ENCODER in case you are using a normal rotary encoder.
  */
    Encoder.reset();
    Encoder.begin(
        i2cEncoderLibV2::INT_DATA | i2cEncoderLibV2::WRAP_DISABLE | i2cEncoderLibV2::DIRE_LEFT | i2cEncoderLibV2::IPUP_ENABLE | i2cEncoderLibV2::RMOD_X1 | i2cEncoderLibV2::RGB_ENCODER);

    // Use this in case of standard encoder!
    //  Encoder.begin(i2cEncoderLibV2::INT_DATA | i2cEncoderLibV2::WRAP_DISABLE | i2cEncoderLibV2::DIRE_LEFT | i2cEncoderLibV2::IPUP_ENABLE | i2cEncoderLibV2::RMOD_X1 | i2cEncoderLibV2::STD_ENCODER);

    // try also this!
    //  Encoder.begin(i2cEncoderLibV2::INT_DATA | i2cEncoderLibV2::WRAP_ENABLE | i2cEncoderLibV2::DIRE_LEFT | i2cEncoderLibV2::IPUP_ENABLE | i2cEncoderLibV2::RMOD_X1 | i2cEncoderLibV2::RGB_ENCODER);

    Encoder.writeCounter((int32_t)0);    /* Reset the counter value */
    Encoder.writeMax((int32_t)10);       /* Set the maximum threshold*/
    Encoder.writeMin((int32_t)-10);      /* Set the minimum threshold */
    Encoder.writeStep((int32_t)1);       /* Set the step to 1*/
    Encoder.writeInterruptConfig(0xff);  /* Enable all the interrupt */
    Encoder.writeAntibouncingPeriod(20); /* Set an anti-bouncing of 200ms */
    Encoder.writeDoublePushPeriod(50);   /*Set a period for the double push of 500ms */
    while (1)
    {
        if (input(IntPin) == 0)
        {
            if (Encoder.updateStatus())
            {
                if (Encoder.readStatus(i2cEncoderLibV2::RINC))
                {
                    print("Increment: %d\n", Encoder.readCounterByte());
                    /* Write here your code */
                }
                if (Encoder.readStatus(i2cEncoderLibV2::RDEC))
                {
                    print("Increment: %d\n", Encoder.readCounterByte());

                    /* Write here your code */
                }

                if (Encoder.readStatus(i2cEncoderLibV2::RMAX))
                {
                    print("Maximum threshold: %d\n", Encoder.readCounterByte());

                    /* Write here your code */
                }

                if (Encoder.readStatus(i2cEncoderLibV2::RMIN))
                {
                    print("Minimum threshold: %d\n", Encoder.readCounterByte());

                    /* Write here your code */
                }

                if (Encoder.readStatus(i2cEncoderLibV2::PUSHR))
                {
                    print("Push button Released\n");

                    /* Write here your code */
                }

                if (Encoder.readStatus(i2cEncoderLibV2::PUSHP))
                {
                    print("Push button Pressed\n");

                    /* Write here your code */
                }

                if (Encoder.readStatus(i2cEncoderLibV2::PUSHD))
                {
                    print("Double push!\n");

                    /* Write here your code */
                }
            }
        }
    }
}