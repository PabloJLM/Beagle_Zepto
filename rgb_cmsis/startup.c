/* Tabla de vectores completa del MSPM0L1117 (nombres de IRQ como en TI). */
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

#define WEAK __attribute__((weak, alias("Default_Handler")))
void NMI_Handler(void) WEAK;
void HardFault_Handler(void) WEAK;
void SVC_Handler(void) WEAK;
void PendSV_Handler(void) WEAK;
void SysTick_Handler(void) WEAK;
void GROUP0_IRQHandler(void) WEAK;
void GROUP1_IRQHandler(void) WEAK;
void TIMG8_IRQHandler(void) WEAK;
void ADC0_IRQHandler(void) WEAK;
void SPI0_IRQHandler(void) WEAK;
void UART1_IRQHandler(void) WEAK;
void UART0_IRQHandler(void) WEAK;
void TIMG0_IRQHandler(void) WEAK;
void TIMA0_IRQHandler(void) WEAK;
void TIMG1_IRQHandler(void) WEAK;
void I2C0_IRQHandler(void) WEAK;
void AES_IRQHandler(void) WEAK;
void RTC_IRQHandler(void) WEAK;
void DMA_IRQHandler(void) WEAK;

__attribute__((section(".isr_vector"), used))
void (*const vector_table[])(void) = {
	(void (*)(void))&_estack,
	Reset_Handler, NMI_Handler, HardFault_Handler,
	0, 0, 0, 0, 0, 0, 0,
	SVC_Handler, 0, 0, PendSV_Handler, SysTick_Handler,
	GROUP0_IRQHandler, GROUP1_IRQHandler, TIMG8_IRQHandler, 0,
	ADC0_IRQHandler, 0, 0, 0,
	0, SPI0_IRQHandler, 0, 0,
	0, UART1_IRQHandler, 0, UART0_IRQHandler,
	TIMG0_IRQHandler, 0, TIMA0_IRQHandler, 0,
	0, 0, TIMG1_IRQHandler, 0,
	I2C0_IRQHandler, 0, 0, 0,
	AES_IRQHandler, 0, RTC_IRQHandler, DMA_IRQHandler,
};
