/*
 * BKIT ARM4 - Lab 1, Exercise 5: Traffic Light System Simulation
 * - Thread 1 (thread_led_blinky): LED3 (PE4) blinks 1 s ON / 1 s OFF as a
 *   "system alive" indicator.
 * - Thread 2 (thread_traffic_light): Y0 (PE5) and Y1 (PE6) simulate a traffic light:
 *     - RED   (only Y0 ON)      for 7 seconds
 *     - GREEN (only Y1 ON)      for 5 seconds
 *     - AMBER (Y0 + Y1 both ON) for 2 seconds
 *   Prints phase transitions via printf("[LIGHT] <NAME> - <SEC> s\n").
 */
#include <stdio.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

#define ALIVE_BLINK_MS 1000

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

struct phase {
	const char *name;
	int y0;
	int y1;
	int ms;
};

static const struct phase traffic_phases[] = {
	{ "RED",   1, 0, 7000 },
	{ "GREEN", 0, 1, 5000 },
	{ "AMBER", 1, 1, 2000 },
};

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
		gpio_pin_set_dt(&led3, 1);
		k_msleep(ALIVE_BLINK_MS);

		gpio_pin_set_dt(&led3, 0);
		k_msleep(ALIVE_BLINK_MS);
	}
}

void thread_traffic_light(void *p1, void *p2, void *p3)
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
		for (size_t i = 0; i < ARRAY_SIZE(traffic_phases); i++) {
			gpio_pin_set_dt(&y0, traffic_phases[i].y0);
			gpio_pin_set_dt(&y1, traffic_phases[i].y1);
			printf("[LIGHT] %s - %d s\n",
			       traffic_phases[i].name,
			       traffic_phases[i].ms / 1000);
			k_msleep(traffic_phases[i].ms);
		}
	}
}

K_THREAD_DEFINE(led_blinky_id,    1024, thread_led_blinky,    NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(traffic_light_id, 1024, thread_traffic_light, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	printf("BKIT ARM4 - Lab 1 Exercise 5 started!\n");
	return 0;
}
