/*
 * BKIT ARM4 - Lab 1, Exercise 4: One Thread Function — Many Lights (Thread Parameters)
 * - LED3 (PE4): 2 s ON, 2 s OFF
 * - Y0   (PE5): 0.5 s ON, 0.5 s OFF
 * - Y1   (PE6): 3 s ON, 1 s OFF
 *
 * Explanation (Why three separate thread functions are not needed):
 * All three LEDs share the exact same blinking algorithm (configure the GPIO pin,
 * turn it ON for on_ms, turn it OFF for off_ms, and repeat forever) and only differ
 * in which GPIO pin they control and how long they stay ON/OFF. By bundling the pin
 * specification and timing parameters into a struct blink_cfg and passing a pointer
 * to each thread instance via K_THREAD_DEFINE's p1 argument, a single parameterized
 * thread_blink() function can run multiple independent thread instances without code duplication.
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

struct blink_cfg {
	struct gpio_dt_spec led;
	int on_ms;
	int off_ms;
};

static const struct blink_cfg led3_cfg = {
	.led    = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
	.on_ms  = 2000,
	.off_ms = 2000,
};

static const struct blink_cfg y0_cfg = {
	.led    = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
	.on_ms  = 500,
	.off_ms = 500,
};

static const struct blink_cfg y1_cfg = {
	.led    = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
	.on_ms  = 3000,
	.off_ms = 1000,
};

void thread_blink(void *p1, void *p2, void *p3)
{
	const struct blink_cfg *cfg = (const struct blink_cfg *)p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	if (!gpio_is_ready_dt(&cfg->led)) {
		printf("Error: GPIO device is not ready\n");
		return;
	}

	if (gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE) < 0) {
		printf("Error: failed to configure LED pin %d\n", cfg->led.pin);
		return;
	}

	while (1) {
		gpio_pin_set_dt(&cfg->led, 1);
		k_msleep(cfg->on_ms);

		gpio_pin_set_dt(&cfg->led, 0);
		k_msleep(cfg->off_ms);
	}
}

K_THREAD_DEFINE(blink_led3_id, 1024, thread_blink, (void *)&led3_cfg, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(blink_y0_id,   1024, thread_blink, (void *)&y0_cfg,   NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(blink_y1_id,   1024, thread_blink, (void *)&y1_cfg,   NULL, NULL, 7, 0, 0);

int main(void)
{
	printf("BKIT ARM4 - Lab 1 Exercise 4 started!\n");
	return 0;
}
