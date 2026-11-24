/* LED RGB (WS2812 en PA15): rotacion continua de color. */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec ws =
	GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), ws2812_gpios);

#define DOUTSET31_0 0x1290

#define NOPS(n) ".rept " #n "\nnop\n.endr\n"

/* Bit WS2812 a 32 MHz, periodo 40 ciclos: bit 0 = 11 ciclos en alto, bit 1 = 22. */
static __attribute__((noinline)) void ws2812_send24(uintptr_t set_reg, uint32_t mask, uint32_t grb)
{
	uint32_t data = grb << 8;
	uint32_t n = 24;

	__asm volatile(
		".syntax unified\n"
		"1:\n"
		"str  %[m], [%[r], #0]\n"
		"lsls %[d], %[d], #1\n"
		"bcs  2f\n"
		NOPS(7)
		"str  %[m], [%[r], #16]\n"
		NOPS(22)
		"b    3f\n"
		"2:\n"
		NOPS(17)
		"str  %[m], [%[r], #16]\n"
		NOPS(13)
		"3:\n"
		"subs %[n], %[n], #1\n"
		"bne  1b\n"
		: [d] "+l"(data), [n] "+l"(n)
		: [r] "l"(set_reg), [m] "l"(mask)
		: "cc", "memory");
}

static void ws2812_set(uint8_t r, uint8_t g, uint8_t b)
{
	uintptr_t set_reg = DT_REG_ADDR(DT_GPIO_CTLR(DT_PATH(zephyr_user), ws2812_gpios)) +
			    DOUTSET31_0;
	uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
	unsigned int key = irq_lock();

	ws2812_send24(set_reg, BIT(ws.pin), grb);
	irq_unlock(key);
}

static void wheel(uint8_t p, uint8_t *r, uint8_t *g, uint8_t *b)
{
	if (p < 85) {
		*r = 255 - p * 3; *g = p * 3; *b = 0;
	} else if (p < 170) {
		p -= 85;
		*r = 0; *g = 255 - p * 3; *b = p * 3;
	} else {
		p -= 170;
		*r = p * 3; *g = 0; *b = 255 - p * 3;
	}
}

int main(void)
{
	if (!gpio_is_ready_dt(&ws)) {
		return 0;
	}
	gpio_pin_configure_dt(&ws, GPIO_OUTPUT_INACTIVE);

	for (uint8_t h = 0;; h++) {
		uint8_t r, g, b;

		wheel(h, &r, &g, &b);
		ws2812_set(r >> 3, g >> 3, b >> 3);
		k_msleep(20);
	}
}
