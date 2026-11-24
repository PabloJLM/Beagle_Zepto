/* Tabla de vectores y Reset_Handler minimos para Cortex-M0+ */
#include <stdint.h>

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;
int main(void);

void Reset_Handler(void)
{
	uint32_t *src = &_sidata, *dst = &_sdata;

	while (dst < &_edata) {
		*dst++ = *src++;
	}
	for (dst = &_sbss; dst < &_ebss;) {
		*dst++ = 0;
	}
	main();
	for (;;) {
	}
}

void Default_Handler(void)
{
	for (;;) {
	}
}

/* Handlers del nucleo; los que no se usan caen en Default_Handler */
void NMI_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)   __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector"), used))
void (*const vector_table[])(void) = {
	(void (*)(void))&_estack, /* SP inicial */
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	0, 0, 0, 0, 0, 0, 0,      /* reservados */
	SVC_Handler,
	0, 0,
	PendSV_Handler,
	SysTick_Handler,
	/* IRQ0..: no se usan interrupciones de perifericos */
};
