/*
 * BKIT ARM4 - Lab 1, Exercise 1: Asymmetric LED Blink
 * LED3 (DEBUG LED, pin PE4): ON for 1 second, OFF for 2 seconds, repeating forever.
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_ON_TIME_MS  1000
#define LED_OFF_TIME_MS 2000

/* Get LED3 (PE4) from the "led0" alias in app.overlay */
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void thread_led_blinky(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	if (!gpio_is_ready_dt(&led3)) {
		printf("Error: LED3 GPIO device is not ready\n");
		return;
	}

	if (gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE) < 0) {
		printf("Error: failed to configure LED3 pin\n");
		return;
	}

	while (1) {
		/* Turn LED3 ON for 1 second */
		gpio_pin_set_dt(&led3, 1);
		printf("LED3: ON\n");
		k_msleep(LED_ON_TIME_MS);

		/* Turn LED3 OFF for 2 seconds */
		gpio_pin_set_dt(&led3, 0);
		printf("LED3: OFF\n");
		k_msleep(LED_OFF_TIME_MS);
	}
}

K_THREAD_DEFINE(led_blinky_id, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	printf("BKIT ARM4 - Lab 1 Exercise 1 started!\n");
	return 0;
}
