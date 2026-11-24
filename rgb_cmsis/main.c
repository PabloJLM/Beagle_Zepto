/*
 * Demo LED RGB (D3, WS2812 en PA15) con CMSIS + driverlib de TI.
 * Efecto: COMBINACIONES FIJAS (amarillo, cian, magenta, naranja, blanco), 2 s cada una.
 *
 *  - CMSIS (core_cm0plus.h): SysTick_Config, __disable_irq/__enable_irq,
 *    __WFI, SystemCoreClock.
 *  - driverlib (ti/driverlib/dl_gpio.h): energia y configuracion del pin,
 *    set/clear de pines.
 *  - Solo la rutina de bits del WS2812 va en ensamblador por los tiempos.
 *
 * Reloj: 32 MHz (SYSOSC por defecto tras el reset).
 */
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_gpio.h>

#define CPU_HZ        32000000u
#define WS_PORT       GPIOA
#define WS_PIN        DL_GPIO_PIN_15     /* PA15 */
#define WS_IOMUX      IOMUX_PINCM37      /* PINCM de PA15 en el L1117 */

/* ---- Base de tiempo con SysTick (CMSIS) ---- */
static volatile uint32_t ms_ticks;

void SysTick_Handler(void)
{
	ms_ticks++;
}

static void delay_ms(uint32_t ms)
{
	uint32_t start = ms_ticks;

	while ((ms_ticks - start) < ms) {
		__WFI();
	}
}

/* ---- WS2812 ---- */
#define NOPS(n) ".rept " #n "\nnop\n.endr\n"

/*
 * Periodo de bit = 40 ciclos de 31.25 ns.
 *   bit 0: alto 11 ciclos (~340 ns)   bit 1: alto 22 ciclos (~690 ns)
 * DOUTCLR31_0 queda 0x10 bytes despues de DOUTSET31_0, por eso [r, #16].
 */
static __attribute__((noinline)) void ws2812_send24(uint32_t grb)
{
	uint32_t data = grb << 8;
	uint32_t n = 24;
	register volatile uint32_t *set = &WS_PORT->DOUTSET31_0;
	register uint32_t mask = WS_PIN;

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
		: [r] "l"(set), [m] "l"(mask)
		: "cc", "memory");
}

static void led_set(uint8_t r, uint8_t g, uint8_t b)
{
	__disable_irq();   /* los tiempos no toleran interrupciones */
	ws2812_send24(((uint32_t)g << 16) | ((uint32_t)r << 8) | b);
	__enable_irq();
}

int main(void)
{
	/* GPIOA: energia + PA15 como salida digital en bajo */
	DL_GPIO_reset(WS_PORT);
	DL_GPIO_enablePower(WS_PORT);
	for (volatile int i = 0; i < 100; i++) {
		/* esperar a que el periferico arranque */
	}
	DL_GPIO_initDigitalOutput(WS_IOMUX);
	DL_GPIO_clearPins(WS_PORT, WS_PIN);
	DL_GPIO_enableOutput(WS_PORT, WS_PIN);

	/* SysTick a 1 kHz */
	SysTick_Config(CPU_HZ / 1000u);

	/* Combinaciones fijas, cada una encendida 2 segundos */
	static const uint8_t combos[][3] = {
		{32, 32, 0},   /* amarillo */
		{0, 32, 32},   /* cian */
		{32, 0, 32},   /* magenta */
		{32, 12, 0},   /* naranja */
		{32, 32, 32},  /* blanco */
	};

	for (;;) {
		for (unsigned i = 0; i < sizeof(combos) / sizeof(combos[0]); i++) {
			led_set(combos[i][0], combos[i][1], combos[i][2]);
			delay_ms(2000);
		}
	}
}
