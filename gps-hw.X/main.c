/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.5
        Device            :  PIC24FJ256GA705
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/uart1.h"

#include <stdio.h>
#include <string.h>

#define FCY (_XTAL_FREQ/2)
#include <libpic30.h>

#define HTTP_URL "http://example.com/position"

static void BG95_SendString(const char *s)
{
    while (*s)
    {
        while (!UART1_IsTxReady());
        UART1_Write(*s++);
    }
}

static void BG95_ReadLine(char *buffer, size_t len)
{
    size_t i = 0;

    while (i < len - 1)
    {
        while (!UART1_IsRxReady());
        char c = UART1_Read();
        buffer[i++] = c;
        if (c == '\n')
        {
            break;
        }
    }

    buffer[i] = '\0';
}

static void BG95_GetPosition(char *pos, size_t len)
{
    BG95_SendString("AT+QGPS=1\r\n");
    __delay_ms(1000);
    BG95_SendString("AT+QGPSLOC=2\r\n");
    BG95_ReadLine(pos, len);
}

static void BG95_PostPosition(const char *pos)
{
    char cmd[128];
    char body[128];

    snprintf(body, sizeof(body), "{\"pos\":\"%s\"}", pos);

    BG95_SendString("AT+QHTTPCFG=\"contenttype\",\"application/json\"\r\n");
    __delay_ms(100);
    snprintf(cmd, sizeof(cmd), "AT+QHTTPURL=%u,80\r\n", (unsigned)strlen(HTTP_URL));
    BG95_SendString(cmd);
    __delay_ms(100);
    BG95_SendString(HTTP_URL);
    BG95_SendString("\r\n");
    __delay_ms(100);
    snprintf(cmd, sizeof(cmd), "AT+QHTTPPOST=%u,80,80\r\n", (unsigned)strlen(body));
    BG95_SendString(cmd);
    __delay_ms(100);
    BG95_SendString(body);
    BG95_SendString("\r\n");
}

static void SleepOneMinute(void)
{
    RCONbits.SWDTEN = 1;
    __builtin_clrwdt();
    Sleep();
    RCONbits.SWDTEN = 0;
}

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // Configure RA4 as digital output for loop activity indication
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;

    while (1)
    {
        char position[64];

        // Toggle RA4 each reporting cycle
        LATAbits.LATA4 ^= 1;

        BG95_GetPosition(position, sizeof(position));
        BG95_PostPosition(position);
        SleepOneMinute();
    }

    return 1;
}
/**
 End of File
*/

