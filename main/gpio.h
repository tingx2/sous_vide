#ifndef GPIO_H
#define GPIO_H

/* 
  gpio.h

  Header file which lists all the GPIOs used in this system.

*/

/**********************************************************************

  Header Files

**********************************************************************/
#include <bcm2835.h>

/**********************************************************************

  Definitions, Variables

**********************************************************************/

#define GPIO_4      (RPI_GPIO_P1_7)
#define GPIO_18     (RPI_GPIO_P1_11)
#define GPIO_25     (RPI_GPIO_P1_22)

#define GPIO_THERM  (GPIO_4)
#define GPIO_PWM    (GPIO_18)
#define GPIO_PUMP   (GPIO_25)

#define GPIO_DIRECTION_INPUT(gpio_id) do { bcm2835_gpio_fsel(gpio_id, BCM2835_GPIO_FSEL_INP); } while(0);
#define GPIO_DIRECTION_OUTPUT(gpio_id) do { bcm2835_gpio_fsel(gpio_id, BCM2835_GPIO_FSEL_OUTP); } while(0);

#define GPIO_SET(gpio_id) do { bcm2835_gpio_set(gpio_id); } while(0);
#define GPIO_CLR(gpio_id) do { bcm2835_gpio_clr(gpio_id); } while(0);

#endif // GPIO_H