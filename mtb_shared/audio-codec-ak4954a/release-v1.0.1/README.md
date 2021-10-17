# Audio Codec (AK4954A)

### Overview

This library provides functions to support use of the AK4954A audio codec found on the CY8CKIT-028-TFT shield.

### Quick Start
1. Create an empty application
2. Add this library to the application
3. Add the following code to your application.\n
The wave.h and wave.c files can be pulled from this example:\n
https://github.com/cypresssemiconductorco/mtb-example-psoc6-i2s

\note This example is targeted for Arduino-based boards.

```cpp
#include "cyhal.h"
#include "cybsp.h"
#include "mtb_ak4954a.h"
#include "wave.h"

#define AUDIO_SYS_CLOCK_HZ (98304000)   /* in Hz */
#define MCLK_FREQ_HZ       (4096000)    /* in Hz */
#define MCLK_DUTY_CYCLE    (50.0)       /* in %  */

static cyhal_i2c_t i2c;
cyhal_clock_t pll_clock;
cyhal_clock_t audio_clock;
cyhal_pwm_t mclk_pwm;
cyhal_i2s_t i2s;

const cyhal_i2c_cfg_t i2c_config = {
    .is_slave        = false,
    .address         = 0,
    .frequencyhal_hz = 400000
};

const cyhal_i2s_pins_t i2s_pins = {
    .sck  = CYBSP_D1,
    .ws   = CYBSP_D2,
    .data = CYBSP_D3
};

const cyhal_i2s_config_t i2s_config = {
    .is_tx_slave    = false,    /* TX is Master */
    .is_rx_slave    = false,    /* RX not used */
    .mclk_hz        = 0,        /* External MCLK not used */
    .channel_length = 32,       /* In bits */
    .word_length    = 16,       /* In bits */
    .sample_rate_hz = 48000     /* In Hz */
};

void i2s_isr_handler(void *arg, cyhal_i2s_event_t event)
{
    (void) arg;
    (void) event;

    /* Stop the I2S TX */
    cyhal_i2s_stop_tx(&i2s);
}

int main(void)
{
    cy_rslt_t rslt;

    rslt = cybsp_init();
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    __enable_irq();

    /* Initialize the I2C to use with the audio codec */
    i2c_init(&i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL);
    cyhal_i2c_configure(&i2c, &i2c_config);

    /* Initialize the PLL */
    rslt = cyhal_clock_allocate(&pll_clock, CYHAL_CLOCK_BLOCK_PLL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_clock_set_frequency(&pll_clock, AUDIO_SYS_CLOCK_HZ, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_clock_set_enabled(&pll_clock, true, true);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    /* Initialize the audio subsystem clock (HFCLK1) */
    rslt = cyhal_clock_get(&audio_clock, &CYHAL_CLOCK_HF[1]);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_clock_init(&audio_clock);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_clock_set_source(&audio_clock, &pll_clock);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_clock_set_enabled(&audio_clock, true, true);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    /* Initialize the Master Clock with a PWM */
    rslt = cyhal_pwm_init(&mclk_pwm, CYBSP_D0, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_pwm_set_duty_cycle(&mclk_pwm, MCLK_DUTY_CYCLE, MCLK_FREQ_HZ);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_pwm_start(&mclk_pwm);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    /* Wait for the MCLK to clock the audio codec */
    cyhal_system_delay_ms(1);

    /* Initialize the I2S */
    rslt = cyhal_i2s_init(&i2s, &i2s_pins, NULL, NC, &i2s_config, &audio_clock);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    cyhal_i2s_register_callback(&i2s, i2s_isr_handler, NULL);
    cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_TX_COMPLETE,
                                  CYHAL_ISR_PRIORITY_DEFAULT, true);

    rslt = mtb_ak4954a_init(&i2c);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    /* Enable the AK494A audio codec with the default volume */
    mtb_ak4954a_activate();
    mtb_ak4954a_adjust_volume(AK4954A_HP_VOLUME_DEFAULT);

    /* Setup complete */

    /* Start the I2S TX */
    cyhal_i2s_start_tx(&i2s);

    /* Initiate a transfer */
    rslt = cyhal_i2s_write_async(&i2s, wave_data, WAVE_SIZE);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    for(;;) { }
}
```

The follow blocks of code can be used to illustrate other functionality.  In the example above,
replace the code following the "Setup complete" comment with one of these code blocks.

Generate an audio beep
```cpp
mtb_ak4954a_set(AK4954A_REG_PWR_MGMT1, AK4954A_PWR_MGMT1_PMBP);
mtb_ak4954a_write_byte(AK4954A_REG_BEEP_ON_TIME, 0x40);
mtb_ak4954a_write_byte(AK4954A_REG_BEEP_FREQ, AK4954A_BEEP_FREQ_BPFR_800Hz);
mtb_ak4954a_write_byte(AK4954A_REG_BEEP_VOL_CTRL, AK4954A_BEEP_VOL_CTRL_BPLVL_m21db);
mtb_ak4954a_set(AK4954A_REG_BEEP_VOL_CTRL, AK4954A_BEEP_VOL_CTRL_BPOUT);
```

Microphone loopback mode
```cpp
mtb_ak4954a_write_byte(AK4954A_REG_DIG_FLTR_MODE, AK4954A_DIG_FLTR_MODE_PFSDO |
                                                  AK4954A_DIG_FLTR_MODE_ADCPF |
                                                  AK4954A_DIG_FLTR_MODE_PFDAC);

mtb_ak4954a_set(AK4954A_REG_SIG_SEL1, AK4954A_SIG_SEL1_PMMP);
mtb_ak4954a_set(AK4954A_REG_PWR_MGMT1, AK4954A_PWR_MGMT1_PMADL |
                                       AK4954A_PWR_MGMT1_PMPFIL);
```


### More information

* [API Reference Guide](https://cypresssemiconductorco.github.io/audio-codec-ak4954a/html/index.html)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)
* [PSoC 6 Code Examples using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)
* [PSoC 6 Middleware](https://github.com/cypresssemiconductorco/psoc6-middleware)
* [PSoC 6 Resources - KBA223067](https://community.cypress.com/docs/DOC-14644)

---
© Cypress Semiconductor Corporation, 2019-2020.
