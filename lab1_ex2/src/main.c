/*
 * BKIT ARM4 - Lab 1, Exercise 2: Coordinated LED3 and Output Y0
 * - thread_led_blinky: drives LED3 (PE4) with 2 s ON, 2 s OFF pattern and
 *   publishes the current LED3 state in the shared variable led_state.
 * - thread_y0_follow: only READS led_state (it never times the 2 s itself),
 *   polls it every 10 ms and updates the Output Y0 status LED (PE5) whenever
 *   the state changes, so Y0 is ON when LED3 is ON and OFF when LED3 is OFF.
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>

#define LED3_ON_MS       2000
#define LED3_OFF_MS      2000
#define POLL_INTERVAL_MS 10

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

/* Shared state between the two threads: 1 = LED3 ON, 0 = LED3 OFF */
static atomic_t led_state = ATOMIC_INIT(0);

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
		/* LED3 ON for 2 seconds, then publish the new state */
		gpio_pin_set_dt(&led3, 1);
		atomic_set(&led_state, 1);
		printf("LED3: ON\n");
		k_msleep(LED3_ON_MS);

		/* LED3 OFF for 2 seconds, then publish the new state */
		gpio_pin_set_dt(&led3, 0);
		atomic_set(&led_state, 0);
		printf("LED3: OFF\n");
		k_msleep(LED3_OFF_MS);
	}
}

void thread_y0_follow(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	if (!gpio_is_ready_dt(&y0)) {
		printf("Error: Y0 GPIO device is not ready\n");
		return;
	}

	if (gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0) {
		printf("Error: failed to configure Y0 pin\n");
		return;
	}

	int last_state = -1; /* force an update on the first poll */

	while (1) {
		int current_state = (int)atomic_get(&led_state);

		/* Update Y0 only when the shared state changes */
		if (current_state != last_state) {
			gpio_pin_set_dt(&y0, current_state);
			printf("Y0: %s\n", current_state ? "ON" : "OFF");
			last_state = current_state;
		}

		k_msleep(POLL_INTERVAL_MS);
	}
}

K_THREAD_DEFINE(led_blinky_id, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(y0_follow_id,  1024, thread_y0_follow,  NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	printf("BKIT ARM4 - Lab 1 Exercise 2 started!\n");
	return 0;
}
