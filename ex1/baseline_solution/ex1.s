.cpu cortex-m3
.thumb
.syntax unified

.include "efm32gg.s"

.section .vectors

.long stack_top     // Top of Stack
.long _reset        // Reset Handler
.long dummy_handler // NMI Handler
.long dummy_handler // Hard Fault Handler
.long dummy_handler // MPU Fault Handler
.long dummy_handler // Bus Fault Handler
.long dummy_handler // Usage Fault Handler
.long dummy_handler // Reserved
.long dummy_handler // Reserved
.long dummy_handler // Reserved
.long dummy_handler // Reserved
.long dummy_handler // SVCall Handler
.long dummy_handler // Debug Monitor Handler
.long dummy_handler // Reserved
.long dummy_handler // PendSV Handler
.long dummy_handler // SysTick Handler

// External Interrupts
.long dummy_handler
.long gpio_handler  // GPIO even handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long gpio_handler  // GPIO odd handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler
.long dummy_handler

.section .text

// R0: return value
// R1: temporary register (may also hold overflow return values)
// R2-R3: temporary registers
// R4-R10 (except R7): permanent registers
// R7: thumb frame pointer
// R11: ARM Frame pointer
// R12: temporary reister
// R13: stack
// R14: link register
// R15: program counter

    // GPIO Port A has pins 8-15
    // this is the lights, etc denoted by MODEH (mode high)
    // GPIO Port C has pis 0-7
    // this is the buttons, denoted by MODEL (mode low)

// Reset handler
	.globl  _reset
	.type   _reset, %function
	.thumb_func
_reset: 
	//Do nothing
	b init

	.thumb_func
init:
	//Configure CMU to enable GPIO
	ldr r1, =CMU_BASE
	ldr r2, [r1, #CMU_HFPERCLKEN0]
	mov r3, #1
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
	orr r2, r2, r3
	str r2, [r1, #CMU_HFPERCLKEN0]

	//Store often used addresses in reg 4-6
	ldr r4, =GPIO_PA_BASE
	ldr r5, =GPIO_PC_BASE
	ldr r6, =GPIO_BASE

	//Set all pins as push-pull with defined drive strength
	ldr r2, =0x55555555
	str r2, [r4, #GPIO_MODEH]

	//Set maximum drive strength
	mov r2, #2
	str r2, [r4, #GPIO_CTRL]
	
	//Set all LEDs off
	ldr r2, =0xFFFF
	str r2, [r4, #GPIO_DOUT]

	//Set all pins as inputs and with internal pullup
	ldr r2, =0x33333333
	str r2, [r5, #GPIO_MODEL]
	ldr r2, =0xFF
	str r2, [r5, #GPIO_DOUT]

	b mainloop	

	.thumb_func
	mainloop:
		bl readbuttons
		str r0, [r4, #GPIO_DOUT]
		b mainloop	

	.thumb_func
	readbuttons:
		ldr r0, [r5, #GPIO_DIN]
		lsl r0, r0, #8
		mov pc, lr

.thumb_func
gpio_handler:
    b .

.thumb_func
dummy_handler:
    b . // do nothing
