.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler
.global USART2_Handler

// Stack and memory section pointers from linker script
.word _sidata
.word _sdata
.word _edata
.word __bss_start__
.word __bss_end__

// Ensure proper alignment for all sections
.align 4
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    // Load and verify stack pointer
    ldr r0, =_estack
    ldr r1, =__stack_limit
    cmp r0, r1
    bls StackError
    mov sp, r0

    // Initialize system
    bl SystemInit

    // Copy .data section to SRAM
    ldr r0, =_sdata     // Destination
    ldr r1, =_edata     // End of destination
    ldr r2, =_sidata    // Source
    
    // Skip if no data to copy
    cmp r0, r1
    beq .L_bss_init
    
.L_copy_data:
    ldr r3, [r2], #4
    str r3, [r0], #4
    cmp r0, r1
    bne .L_copy_data

.L_bss_init:    
    // Zero .bss section
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    movs r2, #0
    
    // Skip if no BSS to clear
    cmp r0, r1
    beq .L_init_arrays
    
.L_zero_bss:
    str r2, [r0], #4
    cmp r0, r1
    bne .L_zero_bss

.L_init_arrays:    
    // Call C++ static constructors
    bl __libc_init_array
    
    // Enter main
    bl main

StackError:
.L_infinite:
    b .L_infinite
    
.size Reset_Handler, .-Reset_Handler

// Properly aligned default handler section
.align 4
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
    b Default_Handler
.size Default_Handler, .-Default_Handler

// Vector table with proper alignment
.align 8
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    
    // Remaining interrupt vectors
    .rept 38
    .word Default_Handler
    .endr
    
    .word USART2_Handler

    .rept 45
    .word Default_Handler
    .endr

// Default implementation for SystemInit
.section .text.SystemInit
.weak SystemInit
.type SystemInit, %function
SystemInit:
    bx lr
.size SystemInit, .-SystemInit

// Weak aliases for all handlers
.weak NMI_Handler
.thumb_set NMI_Handler,Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler,Default_Handler
.weak MemManage_Handler
.thumb_set MemManage_Handler,Default_Handler
.weak BusFault_Handler
.thumb_set BusFault_Handler,Default_Handler
.weak UsageFault_Handler
.thumb_set UsageFault_Handler,Default_Handler
.weak SVC_Handler
.thumb_set SVC_Handler,Default_Handler
.weak DebugMon_Handler
.thumb_set DebugMon_Handler,Default_Handler
.weak PendSV_Handler
.thumb_set PendSV_Handler,Default_Handler
.weak SysTick_Handler
.thumb_set SysTick_Handler,Default_Handler
.weak USART2_Handler
.thumb_set USART2_Handler,Default_Handler