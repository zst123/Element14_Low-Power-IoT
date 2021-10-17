# Light Sensor

### Overview

This library provides functions to support use of a phototransistor ambient light sensor such as the TEMT6000 as used on the CY8CKIT-028-TFT shield. It uses an ADC to read the relative light level which is returned a a percentage of maximum brightness (0-100).

This library is generic and can be used with any ADC based light sensor.

### Quick Start
1. Create an empty application
2. Add this library to the application
3. Add the following code to your application. This example initializes the Light Sensor and reads the relative brightness.

\note This example is targeted for Arduino-based boards.  Define LIGHT_SENSOR_PIN as appropriate for your hardware.

```cpp
#include "cyhal.h"
#include "cybsp.h"
#include "mtb_light_sensor.h"

#define LIGHT_SENSOR_PIN (CYBSP_A0)

cyhal_adc_t adc;
mtb_light_sensor_t light_sensor;

int main(void)
{
    cy_rslt_t rslt;

    rslt = cybsp_init();
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    rslt = cyhal_adc_init(&adc, LIGHT_SENSOR_PIN, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    rslt = mtb_light_sensor_init(&light_sensor, &adc, LIGHT_SENSOR_PIN);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    uint8_t light = mtb_light_sensor_light_level(&light_sensor);

    for(;;) { }
}
```

### More information

* [API Reference Guide](https://cypresssemiconductorco.github.io/light-sensor/html/index.html)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)
* [PSoC 6 Code Examples using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)
* [PSoC 6 Middleware](https://github.com/cypresssemiconductorco/psoc6-middleware)
* [PSoC 6 Resources - KBA223067](https://community.cypress.com/docs/DOC-14644)

---
© Cypress Semiconductor Corporation, 2019-2021.
