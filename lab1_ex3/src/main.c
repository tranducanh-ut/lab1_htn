/*
 * BKIT ARM4 - Lab 1, Exercise 3: Independent Blinking Patterns
 * - thread_led_blinky: drives LED3 (PE4) with 1.5 s ON / 0.5 s OFF cycle.
 * - thread_output_sequence: drives Y0 (PE5) and Y1 (PE6) through 3 states:
 *     1) Only Y0 ON for 1 s
 *     2) Only Y1 ON for 2 s
 *     3) Both Y0 and Y1 ON for 2 s
 * Both threads run completely independently without shared variables.
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED3_ON_MS  1500
#define LED3_OFF_MS 500

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

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
		/* LED3 ON for 1.5 seconds */
		gpio_pin_set_dt(&led3, 1);
		printf("LED3: ON (1.5 s)\n");
		k_msleep(LED3_ON_MS);

		/* LED3 OFF for 0.5 seconds */
		gpio_pin_set_dt(&led3, 0);
		printf("LED3: OFF (0.5 s)\n");
		k_msleep(LED3_OFF_MS);
	}
}

void thread_output_sequence(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	if (!gpio_is_ready_dt(&y0) || !gpio_is_ready_dt(&y1)) {
		printf("Error: Y0/Y1 GPIO device is not ready\n");
		return;
	}

	if (gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0 ||
	    gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE) < 0) {
		printf("Error: failed to configure Y0/Y1 pins\n");
		return;
	}

	while (1) {
		/* State 1: Only Y0 ON for 1 second */
		gpio_pin_set_dt(&y0, 1);
		gpio_pin_set_dt(&y1, 0);
		printf("SEQ: Only Y0 ON (1 s)\n");
		k_msleep(1000);

		/* State 2: Only Y1 ON for 2 seconds */
		gpio_pin_set_dt(&y0, 0);
		gpio_pin_set_dt(&y1, 1);
		printf("SEQ: Only Y1 ON (2 s)\n");
		k_msleep(2000);

		/* State 3: Both Y0 and Y1 ON for 2 seconds */
		gpio_pin_set_dt(&y0, 1);
		gpio_pin_set_dt(&y1, 1);
		printf("SEQ: Both Y0 and Y1 ON (2 s)\n");
		k_msleep(2000);
	}
}

K_THREAD_DEFINE(led_blinky_id,  1024, thread_led_blinky,       NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(output_seq_id,  1024, thread_output_sequence,  NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	printf("BKIT ARM4 - Lab 1 Exercise 3 started!\n");
	return 0;
}
