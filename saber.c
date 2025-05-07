//*****************************************************************************
//
// hello.c - Simple hello world example.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "fips202.h"
#include "api.h"
#include "SABER_indcpa.h"
#include <stdlib.h>
#include "TM4C1294NCPDT.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``Hello World!''
//! on the UART and is a starting point for more complicated applications.
//!
//! Open a terminal with 115,200 8-N-1 to see the output for this demo.
//
//*****************************************************************************

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void SYSCLK_Init(void); // Initialize system clock to local oscillator


//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

// Converts a hexadecimal string input character to its integer value
int charToInt(char *x) {

    if (*x >= 'A' && *x <= 'F') {
        return *x - 'A' + 10;
    }
    else if (*x >= 'a' && *x <= 'f') {
        return *x - 'a' + 10;
    }
    else if (*x >= '0' && *x <= '9') {
        return *x - '0';
    }

    UARTprintf("charToInt failed, input out of bounds\n");

    return 0;
}

//*****************************************************************************
//
// Print "Hello World!" to the UART on the Intelligent UART Module.
//
//*****************************************************************************
int
main(void)
{
    //
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);

    //
    // Configure the device pins.
    //
    PinoutSet(false, false);

    //
    // Enable the GPIO pins for the LED D1 (PN1).
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // Initialize the UART.
    //
    ConfigureUART();

    //
    // Hello!
    //
    UARTprintf("Hello, world!\n");


    /* String Hex to unsigned char array */
    /*char seedString[65] = "061550234D158C5EC95595FE04EF7A25767F2E24CC2BC479D09D86DC9ABCFDE7";

    unsigned char seed[32];

    int i;
    for (i = 0; i < 32; i++) {

        seed[i] = charToInt(&seedString[i*2])*16 + charToInt(&seedString[i*2 + 1]);
    }

    UARTprintf("Before shake128\n");

    for (i = 0; i < 32; i++) {
        UARTprintf("%02X", seed[i]);
    }
    UARTprintf("\n");

    shake128(seed, 32, seed, 32); // for not revealing system RNG state

    UARTprintf("After shake128\n");

    for (i = 0; i < 32; i++) {
        UARTprintf("%02X", seed[i]);
    }
    UARTprintf("\n");*/

    /* End String Hex to unsigned char array */

    /* START SABER */
    
    srand(0); // Initialize random, can replace 0 with current time for random seed

    uint8_t *pk = (uint8_t *)malloc(CRYPTO_PUBLICKEYBYTES * sizeof(uint8_t));   // public key
    uint8_t *sk = (uint8_t *)malloc(CRYPTO_SECRETKEYBYTES * sizeof(uint8_t));   // secret key
    uint8_t *ct = (uint8_t *)malloc(CRYPTO_CIPHERTEXTBYTES * sizeof(uint8_t));  // cipher text
    uint8_t *ss_a = (uint8_t *)malloc(CRYPTO_BYTES * sizeof(uint8_t));          // shared secret a
    uint8_t *ss_b = (uint8_t *)malloc(CRYPTO_BYTES * sizeof(uint8_t));          // shared secret b

    if (pk == NULL || sk == NULL) {

        if (pk == NULL)
            UARTprintf("pk is NULL\n");

        if (sk == NULL)
            UARTprintf("sk is NULL\n");

        UARTprintf("Malloc Failed.\n");
    }
    else {
        
        //indcpa_kem_keypair(pk, sk);
        
        //Generation of secret key sk and public key pk pair
	    crypto_kem_keypair(pk, sk);

	    //Key-Encapsulation call; input: pk; output: ciphertext c, shared-secret ss_a;	
	    //crypto_kem_enc(ct, ss_a, pk);

        char pt[64] = "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";

        unsigned char seed[32];

        int32_t i;
        for (i = 0; i < 32; i++) {

            seed[i] = charToInt(&pt[i*2])*16 + charToInt(&pt[i*2 + 1]);
        }

        //Key-Encapsulation call; input: pk, plaintext pt; output: ciphertext c, shared-secret ss_a;
        crypto_kem_enc(ct, ss_a, pk, seed);

        unsigned char plaintext[32];
	    //Key-Decapsulation call; input: sk, ct; output: shared-secret ss_b;	
	    crypto_kem_dec(ss_b, ct, sk, plaintext);

        UARTprintf("pk = ");
        for (i = 0; i < CRYPTO_PUBLICKEYBYTES; i++) {
            UARTprintf("%02X", pk[i]);
        }
        UARTprintf("\n\n");

        UARTprintf("sk = ");
        for (i = 0; i < CRYPTO_SECRETKEYBYTES; i++) {
            UARTprintf("%02X", sk[i]);
        }
        UARTprintf("\n\n");

        UARTprintf("ct = ");
        for (i = 0; i < CRYPTO_CIPHERTEXTBYTES; i++) {
            UARTprintf("%02X", ct[i]);
        }
        UARTprintf("\n\n");

        UARTprintf("ss_a = ");
        for (i = 0; i < CRYPTO_BYTES; i++) {
            UARTprintf("%02X", ss_a[i]);
        }
        UARTprintf("\n");

        UARTprintf("ss_b = ");
        for (i = 0; i < CRYPTO_BYTES; i++) {
            UARTprintf("%02X", ss_b[i]);
        }
        UARTprintf("\n");

        UARTprintf("pt = ");
        for (i = 0; i < 32; i++) {
            UARTprintf("%02X", plaintext[i]);
        }
        UARTprintf("\n");

        free(pk);
        pk = NULL;

        free(sk);
        sk = NULL;

        free(ct);
        ct = NULL;

        free(ss_a);
        ss_a = NULL;

        free(ss_b);
        ss_b = NULL;

    }
    
    /* END SABER */

    //
    // We are finished.  Hang around flashing D1.
    //
    while(1)
    {
        //
        // Turn on D1.
        //
        LEDWrite(CLP_D1, 1);

        //
        // Delay for a bit.
        //
        SysCtlDelay(g_ui32SysClock / 10 / 3);

        //
        // Turn off D1.
        //
        LEDWrite(CLP_D1, 0);

        //
        // Delay for a bit.
        //
        SysCtlDelay(g_ui32SysClock / 10 / 3);
    }
}

void SYSCLK_Init(void)
{
	//use main oscillator (XTAL @16 MHz)
	//SYSCTL->RCC = 0x3D40;

	//use main osciallator 5 MHZ, SYS DIV = 2 (expected frequency 2.5MHZ)
  //SYSCTL->RCC = 0x803A 40;
}
