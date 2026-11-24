/*
 * Demo LED RGB (D3, WS2812 en PA15) totalmente bare metal.
 * Efecto: solo ROJO, VERDE y AZUL, cada uno en modo "respiracion" (breath).
 * Sin Zephyr, sin CMSIS, sin driverlib: solo registros.
 *
 * Tras el reset el MCU corre a 32 MHz (SYSOSC), que es lo que asumen los
 * tiempos del WS2812 (ver ws2812_send24). No se toca el reloj.
 */
#include <stdint.h>

#define REG(addr) (*(volatile uint32_t *)(addr))

/* GPIOA y IOMUX (mspm0l111x.h) */
#define GPIOA_BASE    0x400A0000u
#define GPIOA_PWREN   REG(GPIOA_BASE + 0x0800)
#define GPIOA_DOUTSET (GPIOA_BASE + 0x1290)
#define GPIOA_DOE     REG(GPIOA_BASE + 0x12C0)
#define GPIO_PWREN_KEY_ENABLE 0x26000001u

/* IOMUX: PA15 es PINCM37, el array PINCM empieza en IOMUX_BASE + 4 */
#define IOMUX_PINCM37 REG(0x40428000u + 4u + 36u * 4u)
#define PINCM_PC      (1u << 7) /* pin conectado */
#define PINCM_PF_GPIO 1u

#define WS_PIN        15
#define WS_MASK       (1u << WS_PIN)

/* SysTick (nucleo Cortex-M0+) para esperar sin interrupciones */
#define SYST_CSR      REG(0xE000E010u)
#define SYST_RVR      REG(0xE000E014u)
#define SYST_CVR      REG(0xE000E018u)

static void delay_ms(uint32_t ms)
{
	SYST_RVR = 32000u - 1u;  /* 1 ms a 32 MHz */
	SYST_CVR = 0;
	SYST_CSR = 5u;           /* habilitado, reloj del nucleo, sin IRQ */
	while (ms--) {
		while (!(SYST_CSR & (1u << 16))) {
			/* COUNTFLAG */
		}
	}
	SYST_CSR = 0;
}

#define NOPS(n) ".rept " #n "\nnop\n.endr\n"

/*
 * Manda 24 bits (GRB) al LED. CPU a 32 MHz (1 ciclo = 31.25 ns), periodo
 * de bit = 40 ciclos:
 *   bit 0: alto 11 ciclos (~340 ns)   bit 1: alto 22 ciclos (~690 ns)
 */
static __attribute__((noinline)) void ws2812_send24(uint32_t grb)
{
	uint32_t data = grb << 8;
	uint32_t n = 24;
	register uint32_t reg = GPIOA_DOUTSET; /* DOUTCLR = DOUTSET + 0x10 */
	register uint32_t mask = WS_MASK;

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
		: [r] "l"(reg), [m] "l"(mask)
		: "cc", "memory");
}

static void led_set(uint8_t r, uint8_t g, uint8_t b)
{
	ws2812_send24(((uint32_t)g << 16) | ((uint32_t)r << 8) | b);
}

int main(void)
{
	/* 1. Dar energia al puerto GPIOA */
	GPIOA_PWREN = GPIO_PWREN_KEY_ENABLE;
	for (volatile int i = 0; i < 100; i++) {
		/* esperar a que el periferico arranque */
	}

	/* 2. Conectar PA15 al GPIO y ponerlo como salida en bajo */
	IOMUX_PINCM37 = PINCM_PC | PINCM_PF_GPIO;
	REG(GPIOA_DOUTSET + 0x10) = WS_MASK; /* DOUTCLR */
	GPIOA_DOE |= WS_MASK;

	/* Solo rojo, verde y azul; cada color "respira" (sube y baja) */
	for (;;) {
		for (int c = 0; c < 3; c++) {
			for (int i = -40; i <= 40; i++) {
				int t = i < 0 ? -i : i;          /* 40..0..40 */
				uint32_t u = 40 - t;                      /* 0..40 */
				uint8_t v = (uint8_t)((u * u) >> 6);   /* 0..25, curva cuadratica */

				led_set(c == 0 ? v : 0, c == 1 ? v : 0, c == 2 ? v : 0);
				delay_ms(25);
			}
		}
	}
}
