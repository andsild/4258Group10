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
    mov r2, #0
    str r2, [r4, #GPIO_CTRL]
    
    //Set all LEDs off
    ldr r2, =0xFFFF
    str r2, [r4, #GPIO_DOUT]

    //Set all pins as inputs and with internal pullup
    ldr r2, =0x33333333
    str r2, [r5, #GPIO_MODEL]
    ldr r2, =0xFF
    str r2, [r5, #GPIO_DOUT]

    //Configure interrupt for buttons
    ldr r2, =0x22222222
    str r2, [r6, #GPIO_EXTIPSELL]
    ldr r2, =0xFF
    str r2, [r6, #GPIO_EXTIRISE]
    str r2, [r6, #GPIO_EXTIFALL]
    str r2, [r6, #GPIO_IEN]
    ldr r2, =0x802
    ldr r1, =ISER0
    str r2, [r1]
    
    //Configure SCR
    ldr r1, =SCR
    ldr r2, =0b00110
    str r2, [r1]

    LDR r7, =0xef00
    str r7, [r4, #GPIO_DOUT]

    b mainloop  

    .thumb_func
    mainloop:
        wfi 
        b mainloop  

// GPIO handler
// The CPU will jump here when there is a GPIO interrupt

    .thumb_func
gpio_handler:  
    ldr r0, [r5, #GPIO_DIN]
    lsl r0, r0, #8
    PUSH {lr}
    bl switch_button    
    str r7, [r4, #GPIO_DOUT]
    ldr r1, [r6, #GPIO_IF]
    str r1, [r6, #GPIO_IFC]
    POP {pc}

.thumb_func
switch_button:
    PUSH {lr}

    LDR R3, =0xf700 // btn down
    CMP R0, R3
    IT eq
    BLEQ drive_down

    LDR R3, =0xfe00 // btn left
    CMP R0, R3
    IT eq
    BEQ jump_left

    SUB R3, 0x0100 // btn up
    CMP R0, R3
    IT eq 
    BLEQ drive_up

    SUB R3, 0x0200  // btn right
    CMP R0, R3
    IT eq
    BEQ jump_right

    SUB R3, 0x0C00 // btn left on the right side (0xEF00)
    CMP R0, R3
    IT eq
    BEQ increase_numberof_leds

    SUB R3, 0x1000 // btn up on the right side
    CMP R0, R3
    IT eq
    BEQ lightup_all_leds

    SUB R3, 0x2000 // btn right on the right side
    CMP R0, R3
    IT eq
    BEQ decrease_numberof_leds

    LDR R3, =0x7f00 
    CMP R0, R3
    IT eq
    BEQ return_to_default

    POP {pc}

.thumb_func
jump_right:
    AND R2, R7, 0x8000

    CMP R2, #0
    IT EQ
    POPEQ {pc}

    LSL R7, #1 
    ORR R7, 0x0100
    AND R7, 0xFF00

    POP {pc}


.thumb_func
jump_left:
    AND R2, R7, 0x0100

    CMP R2, #0 // avoid border
    IT EQ
    POPEQ {pc}

    LSR R7, #1 
    ORR R7, 0x8000
    AND R7, 0xFF00

    POP {pc}

.thumb_func
drive_up:
    ldr r1, [r4, #GPIO_CTRL]

    // Keep in mind that these if checks needs to happen in this order
    // (since we mov values and then do later checks on the same value
    CMP R1, #0
    IT eq
    MOVEQ R1, #2

    CMP R1, #3
    IT eq
    MOVEQ R1, #0

    CMP R1, #1
    IT eq
    MOVEQ R1, #3

    STR R1, [r4, #GPIO_CTRL]
    POP {pc}

.thumb_func
drive_down:
    LDR R1, [R4, #GPIO_CTRL]

    CMP R1, #3
    IT eq
    MOVEQ R1, #1

    CMP R1, #0
    IT eq
    MOVEQ R1, #3

    CMP R1, #2
    IT eq
    MOVEQ R1, #0


    STR R1, [R4, #GPIO_CTRL]
    POP {pc}

.thumb_func
increase_numberof_leds:
    MOV R2, R7
    LSR R7, #1 
    ORR R7, 0x8000
    AND R7, R2, R7
    AND R7, 0xFF00

    POP {pc}

.thumb_func
decrease_numberof_leds:
    MOV R2, R7
    MOV R3, R7

    LSL R3, #1 
    ORR R3, 0x0100
    ORR R3, R2, R3
    AND R3, 0xFF00

    CMP R3, 0xFF00 // prevent lighting out all lights
    IT NE
    MOVNE R7, R3


    POP {pc}

.thumb_func
lightup_all_leds:
    LDR R7, =0x00FF
    POP {pc}

.thumb_func
return_to_default:
    LDR R7, =0xEF00
    POP {pc}

    .thumb_func

dummy_handler:
    b . // do nothing
