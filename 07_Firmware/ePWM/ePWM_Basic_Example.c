#include "driverlib.h"
#include "device.h"

#define PWM_PERIOD 250U
#define CMPA_VALUE 125U

void main(void)
{
    //
    // Initialize device
    //
    Device_init();

    //
    // Initialize GPIO
    //
    Device_initGPIO();

    //
    // GPIO0 = EPWM1A
    //
    GPIO_setPinConfig(GPIO_0_EPWM1A);

    //
    // Disable TBCLK during configuration
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Time Base Configuration
    //
    EPWM_setTimeBasePeriod(EPWM1_BASE, PWM_PERIOD);

    EPWM_setTimeBaseCounter(
        EPWM1_BASE,
        0
    );

    EPWM_setTimeBaseCounterMode(
        EPWM1_BASE,
        EPWM_COUNTER_MODE_UP
    );

    //
    // Compare Register
    //
    EPWM_setCounterCompareValue(
        EPWM1_BASE,
        EPWM_COUNTER_COMPARE_A,
        CMPA_VALUE
    );

    //
    // Output HIGH at Counter = 0
    //
    EPWM_setActionQualifierAction(
        EPWM1_BASE,
        EPWM_AQ_OUTPUT_A,
        EPWM_AQ_OUTPUT_HIGH,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO
    );

    //
    // Output LOW at Counter = CMPA
    //
    EPWM_setActionQualifierAction(
        EPWM1_BASE,
        EPWM_AQ_OUTPUT_A,
        EPWM_AQ_OUTPUT_LOW,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA
    );

    //
    // Enable TBCLK
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    while(1)
    {

    }
}