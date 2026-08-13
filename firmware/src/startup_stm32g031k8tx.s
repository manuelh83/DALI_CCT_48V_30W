.syntax unified
.cpu cortex-m0plus
.thumb

.global _start
.global Reset_Handler
.global g_pfnVectors

.extern _estack
.extern SystemInit
.extern main
.extern Default_Handler
.extern TIM14_IRQHandler

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word Default_Handler
  .word TIM14_IRQHandler

.text
.thumb_func
Reset_Handler:
_start:
  bl SystemInit
  bl main
1:
  b 1b
