// NOMBRE: ALBA MARÍA RODAS VELÁSQUEZ.
// CURSO: ELECTRÓNICA DIGITAL II.
// PROYECTO III: TivaWare


// INCLUIMOS LIBRERIAS A UTILIZAR:
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS LA FRECUENCIA DE RELOJ A UTILIZAR:
// EN ESTA OCASIÓN USAREMOS 16MHz
#define XTAL 16000000
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PARA INICIAR:
char value_inicio = '0';
// PARA VER ESTADO:
uint8_t state = 0;
// DIP SWITCHES:
uint8_t DIP01_SWITCH = 0;
uint8_t DIP02_SWITCH = 0;
// SECUENCIAS:
uint8_t first_sequence = 0;
uint8_t second_sequence = 0;
// CONTADOR BINARIO:
int8_t bit04_counter = 0;
// PARA ENVIAR:
uint8_t sending = 0;
// PARA variable_7seg:
uint8_t variable_7seg = 0;
// CONTADOR HEX:
uint8_t hex_counter = 0;
// CONTROLAR PERIODO:
uint32_t variable_periodo;
// CONTROLAR PERIODO 02:
uint32_t variable_periodo02;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// INCLUIMOS LOS PROTOTIPOS DE FUNCIONES:
void config (void);
void Timer0AIntHandler(void);
void Timer1AIntHandler(void);
void InitUART(void);
void UART0ReadIntHandler(void);

void GPIOIntHandler (void);
unsigned short map(uint32_t val, uint32_t in_min, uint32_t in_max, unsigned short out_min, unsigned short out_max);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN CODE:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

    // LLAMAMOS A LA CONFIGURACION:
    config();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Loop:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // INICIAMOS EL CICLO:
    while (1)
    {

        // CASO: DIP #1 --> ON
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0x7B)==0x7B)
        {
            // THEN, EL DIP 1 SE ENCIENDE.
            DIP01_SWITCH = 1;
        }

        // CASO: DIP #1 --> OFF
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0x02)==0x00)
        {
            // THEN, EL DIP 1 SE APAGA.
            DIP01_SWITCH = 0;
        }

        // CASO: DIP #2 --> ON
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0x04)==0x04)
        {
            // THEN, EL DIP 2 SE ENCIENDE.
            DIP02_SWITCH = 1;
        }

        // CASO: DIP #2 --> OFF
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0x04)==0x00)
        {
            // THEN, EL DIP 1 SE APAGA.
            DIP02_SWITCH = 0;
        }

        // PARA SABER EN QUE ESTADO ESTOY, ENCIENDO LOS LEDS INTEGRADOS DE LA TivaC:

        // ESTADO UNO: AZUL
        if ((DIP01_SWITCH == 0)&&(DIP02_SWITCH == 0))
        {
            state = 1;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
        }
        // ESTADO DOS: VERDE
        else if ((DIP01_SWITCH == 0)&&(DIP02_SWITCH == 1))
        {
            state = 2;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);
        }
        // ESTADO TRES: ROJO
        else if ((DIP01_SWITCH == 1)&&(DIP02_SWITCH == 0))
        {
            state = 3;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);
        }
        // ESTADO 4: ROJO + AZUL + VERDE = BLANCO
        else if ((DIP01_SWITCH == 1)&&(DIP02_SWITCH == 1))
        {
            state = 4;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ESTADO #1:
        // INTRODUCIR DESDE EL SERIAL LOS VALORES A MOSTRAR EN EL DISPLAY Y LEDS:
        // PARA LEDS:
            // a = LED ROJO
            // w = LED AMARILLO
            // d = LED AZUL
            // z = LED VERDE
        // PARA 7 SEGMENTOS:
            // NÚMEROS DEL 1-9.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // ESTADO # 1:                                                                                   //
        // DIP #1 y DIP #2 == OFF                                                                        //
        // SE ENVÍAN VALORES POR LA TERMINAL SERIAL DE CODE COMPOSER, PARA CAMBIAR EL VALOR DEL DISPLAY  //
        // Y LEDS PRESENTES EN EL CIRCUITO.                                                              //
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        if (state == 1)
        {

            // ENVIAMOS 'a':
            if (value_inicio == 'a')
            {
                value_inicio = 'v';
                // ENCENDEMOS LED CORRESPONDIENTE A VALOR INGRESADO:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x10);
                }
            }

            // ENVIAMOS 'w':
            if (value_inicio == 'w')
            {
                value_inicio = 'v';
                // ENCENDEMOS LED CORRESPONDIENTE A VALOR INGRESADO:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x20);
                }
            }

            // ENVIAMOS 'd':
            if (value_inicio == 'd')
            {
                value_inicio = 'v';
                // ENCENDEMOS LED CORRESPONDIENTE A VALOR INGRESADO:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x00);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);
                }
            }

            // ENVIAMOS 'z':
            if (value_inicio == 'z')
            {
                value_inicio = 'v';
                // ENCENDEMOS LED CORRESPONDIENTE A VALOR INGRESADO:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x80);
                }
            }

            // ENCENDEMOS SEGMENTOS CORRESPONDIENTES DE DISPLAY, DEPENDIENDO VALORES INGRESADO:
            // SE INGRESA = 0; EN EL UART:
            if (value_inicio == '0')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 0;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A CERO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111111);
            }

            // ENCENDEMOS SEGMENTOS CORRESPONDIENTES DE DISPLAY, DEPENDIENDO VALORES INGRESADO:
            // SE INGRESA = 1; EN EL UART:
            if (value_inicio == '1')

            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 1;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000110);
            }

            // SE INGRESA = 2; EN EL UART:
            if (value_inicio == '2')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 2;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01011011);
            }

            // SE INGRESA = 3; EN EL UART:
            if (value_inicio == '3')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 3;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            // SE INGRESA = 4; EN EL UART:
            if (value_inicio == '4')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 4;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100110);
            }

            // SE INGRESA = 5; EN EL UART:
            if (value_inicio == '5')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 5;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            // SE INGRESA = 6; EN EL UART:
            if (value_inicio == '6')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 6;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111101);
            }

            // SE INGRESA = 7; EN EL UART:
            if (value_inicio == '7')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 7;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000111);
            }

            // SE INGRESA = 8; EN EL UART:
            if (value_inicio == '8')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 8;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            // SE INGRESA = 9; EN EL UART:
            if (value_inicio == '9')
            {
                value_inicio = 'v';
                // ACTUALIZAMOS VALOR DE 7-SEG
                variable_7seg = 9;
                // ENCENDEMOS SEGMENTOS CORRESPONDIENTES A UNO EN EL DISPLAY.
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101111);
            }


            // ACCIÓN DE PRESIONAR 'ENTER':
            // MOSTRAMOS EN EL SERIAL, EL ESTADO ACTUAL DE LAS LEDs EN EL CIRCUITO:
            if (sending == 1)
            {
                // ACTUALIZACIÓN DE ESTADO DE LED ROJA:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, '.');
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'P');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');

                }
                // ACTUALIZACIÓN DE ESTADO DE LED AZUL:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, '.');

                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'P');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                }
                // ACTUALIZACIÓN DE ESTADO DE LED AMARILLA:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, '.');
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'P');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                }
                // ACTUALIZACIÓN DE ESTADO DE LED VERDE:
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, '.');
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, '#');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'P');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'A');
                }

                if (variable_7seg == 0)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');
                }

                if (variable_7seg == 1)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                }

                if (variable_7seg == 2)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2');
                }

                if (variable_7seg == 3)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                }

                if (variable_7seg == 4)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                }

                if (variable_7seg == 5)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                }

                if (variable_7seg == 6)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                }

                if (variable_7seg == 7)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                }

                if (variable_7seg == 8)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                }

                if (variable_7seg == 9)
                {
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'G');
                    UARTCharPut(UART0_BASE, 'M');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                }
                // DEJAMOS DE ENVIAR:
                sending = 0;
            }

        }
//////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // ESTADO DOS:                                                          //
        // DIP #1 = ON y DIP #2 = OFF                                           //
        // CONTADOR BINARIO CON LEDs, INCREMENTA Y DECREMENTA CON BOTONES PUSH. //
        // CONTADOR HEXADECIMAL EN 7 SEGMENTOS:                                 //
        //////////////////////////////////////////////////////////////////////////
        else if (state == 2)
        {

            // MOSTRAMOS EL ESTADO DEL 7 SEGMENTOS EN DISPLAY:
            if (hex_counter == 0)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 0
                variable_7seg = 0;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111111);
            }

            if (hex_counter == 1)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 1
                variable_7seg = 1;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000110);
            }

            if (hex_counter == 2)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 2
                variable_7seg = 2;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01011011);
            }

            if (hex_counter == 3)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 3
                variable_7seg = 3;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            if (hex_counter == 4)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 4
                variable_7seg = 4;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100110);
            }

            if (hex_counter == 5)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 5
                variable_7seg = 5;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            if (hex_counter == 6)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 6
                variable_7seg = 6;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111101);
            }

            if (hex_counter == 7)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 7
                variable_7seg = 7;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000111);
            }

            if (hex_counter == 8)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 8
                variable_7seg = 8;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            if (hex_counter == 9)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 9
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101111);
            }

            if (hex_counter == 10)
            {
                // ACTUALIZAMOS EL VALOR DE LOS 7-SEG, CASO = 9 --> SE MANTIENE EL 9 EN DISPLAY Y CAMBIAN LOS LEDS:
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }

            if (hex_counter == 11)
            {
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111100);
            }

            if (hex_counter == 12)
            {
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111001);
            }

            if (hex_counter == 13)
            {
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01011110);
            }

            if (hex_counter == 14)
            {
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111001);
            }

            if (hex_counter == 15)
            {
                variable_7seg = 9;
                // ENCENDEMOS LOS SEGMENTOS DEL DISPLAY:
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110001);
            }

            // CONFIGURACIONES PARA EL CONTADO DE 4 BITS --> LEDS

            if (bit04_counter == 0)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
            }
            else if (bit04_counter == 1)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x10);
            }
            else if (bit04_counter == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x20);
            }
            else if (bit04_counter == 3)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x30);
            }
            else if (bit04_counter == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x40);
            }
            else if (bit04_counter == 5)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x50);
            }
            else if (bit04_counter == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x60);
            }
            else if (bit04_counter == 7)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x70);
            }
            else if (bit04_counter == 8)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x80);
            }
            else if (bit04_counter == 9)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x90);
            }
            else if (bit04_counter == 10)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xA0);
            }
            else if (bit04_counter == 11)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xB0);
            }
            else if (bit04_counter == 12)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xC0);
            }
            else if (bit04_counter == 13)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xD0);
            }
            else if (bit04_counter == 14)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xE0);
            }
            else if (bit04_counter == 15)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xF0);
            }

            // PARA MOSTRAR EN LA TERMINAL SERIAL, LOS VALORES DE AMBOS CONTADORES:
            if (sending == 1)
            {
                if (hex_counter == 0)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');
                }

                if (hex_counter == 1)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                }

                if (hex_counter == 2)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2');
                }

                if (hex_counter == 3)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                }

                if (hex_counter == 4)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                }

                if (hex_counter == 5)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                }

                if (hex_counter == 6)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                }

                if (hex_counter == 7)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                }

                if (hex_counter == 8)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                }

                if (hex_counter == 9)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                }

                if (hex_counter == 10)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                }

                if (hex_counter == 11)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'B');
                }

                if (hex_counter == 12)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                }

                if (hex_counter == 13)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'D');
                }

                if (hex_counter == 14)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                }

                if (hex_counter == 15)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'F');
                }


                // CONFIGURACIÓN DE CONTADOR BINARIO + HEX:

                if (bit04_counter == 0)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');

                }

                if (bit04_counter == 1)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                }

                if (bit04_counter == 2)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2);
                }

                if (bit04_counter == 3)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                }

                if (bit04_counter == 4)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                }

                if (bit04_counter == 5)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                }

                if (bit04_counter == 6)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                }

                if (bit04_counter == 7)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                }

                if (bit04_counter == 8)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                }

                if (bit04_counter == 9)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                }

                if (bit04_counter == 10)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '0');
                }

                if (bit04_counter == 11)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '1');
                }

                if (bit04_counter == 12)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '2');
                }

                if (bit04_counter == 13)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '3');
                }

                if (bit04_counter == 14)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '4');
                }

                if (bit04_counter == 15)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'U');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'T');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, '-');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'I');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '5');
                }
                // CUANDO DEJAMOS DE ENVIAR:
                sending = 0;
            }

        }

        ///////////////////////////////////////////////////////////////////////////
        // ESTADO TRES:                                                          //
        // DIP #1 = OF y DIP #2 = ON                                             //
        //      - ANIMACIÓN EN LEDS                                              //
        //      - ANIMACIÓN EN 7 SEGMENTOS.                                      //
        ///////////////////////////////////////////////////////////////////////////

        else if (state == 3)
        {

            // ANIMACIÓN EN DISPLAY:
            if (first_sequence == 5)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10100000);
            }

            if (first_sequence == 4)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01000001);
            }

            if (first_sequence == 3)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10001000);
            }

            if (first_sequence == 2)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000101);
            }

            if (first_sequence == 1)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10000010);
            }

            if (first_sequence == 0)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00010001);
            }

            // FIN DE ANIMACIÓN DE DISPLAY

            // ANIMACIÓN DE 4 LEDs:
            if (second_sequence == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b10000000);
            }

            if (second_sequence == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b01000000);
            }

            if (second_sequence == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00100000);
            }

            if (second_sequence == 0)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00010000);
            }
            // FIN DE ANIMACIÓN DE LEDs.
        }

        ///////////////////////////////////////////////////////////////////////////
        // ESTADO CUATRO:                                                        //
        // DIP #1 = ON y DIP #2 = ON                                             //
        //      - ANIMACIÓN EN LEDS                                              //
        //      - ANIMACIÓN EN 7 SEGMENTOS.                                      //
        ///////////////////////////////////////////////////////////////////////////

        else if (state == 4)
        {

         }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURATIONS:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INICIAN LAS RUTINAS - CONFIGURACIONES:
    void config (void)
{
    // OSCILADOR INTERNO: 16MHz
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // COMUNICACIÓN UART:
    InitUART();

    // PINES DE ENTRADA:
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);

    // PINES DE SALIDA:
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

    // PUERTOS A UTLIZAR:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // HABILITACIÓN DE PUERTO 'E':
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // HABILITACIÓN DE TIMER 1, TIMER 0:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // INTERRUPCIONES DEL UART:
    IntEnable(INT_UART0);

    // FIFO:
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    // REPECTIÓN DEL UART:
    UARTIntEnable(UART0_BASE, UART_INT_RX);

    // INTERRUPCIONES DE PUERTO: 'E', 'D'
    IntEnable(INT_GPIOD);
    IntEnable(INT_GPIOE);

    // INTERRUPCIÓN PUERTO D:
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_INT_PIN_6, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

    // INTERRUPCIÓN PUERTO E:
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);


    // CONFIGURACIÓN DEL FUNCIONAMIENTO PERIODICO DEL TIMER 0:
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // CONFIGURACIÓN DEL FUNCIONAMIENTO PERIODICO DEL TIMER 1:
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    // PERIODO DE OSCILADORES:
    variable_periodo = (SysCtlClockGet());
    variable_periodo02 = (SysCtlClockGet() / 10);

    // PERIODO DE TIMERS:
    // TIMER 0:
    TimerLoadSet(TIMER0_BASE, TIMER_A, variable_periodo - 1);
    // TIMER 1:
    TimerLoadSet(TIMER1_BASE, TIMER_A, variable_periodo02 - 1);

    // HABILITACIÓN DE INTERRUPCIONES GLOBALES:
    IntMasterEnable();

    // INTERRUPCIÓN DE TEMPORIZADORES:
    // TIMER 0:
    IntEnable(INT_TIMER0A);
    // TIMER 1:
    IntEnable(INT_TIMER1A);

    // INTERRUPCIÓN DEL TIMEOUT:
    // TIMER 0:
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // TIMER 1:
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // HABILITAR INTERRCIONES
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS:
///////////////////////////////////////////////////////////////////////////////////////////////////////

// UART, INICIALIZACIÓN:
void InitUART(void)
{
    // HABILITAMOS PUERTO A:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // HABILITAMOS UART:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // HABILITAMOS UART - PERIFERICOS:
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // CONFIGURAMOS UART ASISTIDO DEL SYS:
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

}


////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPTIONS:
///////////////////////////////////////////////////////////////////////////////////////////////////

void UART0ReadIntHandler(void)
{
    // LIMPIAR INTERRUPCIÓN DEL UART;
    UARTIntClear(UART0_BASE, UART_INT_RX);

    // SAVE EL CARACTER QUE SE RECIBE:
    value_inicio = UARTCharGet(UART0_BASE);

    // MOSTRAMOS UN TEXTO QUE PERMITA SABER VALOR INTRODUJO EN LA TERMINAL SERIAL:
    UARTCharPut(UART0_BASE, 'V');
    UARTCharPut(UART0_BASE, 'A');
    UARTCharPut(UART0_BASE, 'L');
    UARTCharPut(UART0_BASE, 'O');
    UARTCharPut(UART0_BASE, 'R');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'I');
    UARTCharPut(UART0_BASE, 'N');
    UARTCharPut(UART0_BASE, 'T');
    UARTCharPut(UART0_BASE, 'R');
    UARTCharPut(UART0_BASE, 'O');
    UARTCharPut(UART0_BASE, 'D');
    UARTCharPut(UART0_BASE, 'U');
    UARTCharPut(UART0_BASE, 'C');
    UARTCharPut(UART0_BASE, 'I');
    UARTCharPut(UART0_BASE, 'D');
    UARTCharPut(UART0_BASE, 'O');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, '');
    UARTCharPut(UART0_BASE, value_inicio);

}
void Timer0AIntHandler(void)
{
    // CLEAR A LA INTERRUPCION DEL TIMER 0:
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (state == 2)
    {
        // ++ CONTADOR HEXADECIMAL:
        hex_counter++;

        // ACTIVO BANDERA, SE MUESTRA DATO EN LA TERMINAL:
        sending = 1;

        // PARA EL OVERFLOW:
        if (hex_counter == 0x10)
        {
            // VOLVER A CERO:
            hex_counter = 0;
        }
    }
}

void Timer1AIntHandler(void)
{
    // CLEAR A LA INTERRUPCIÓN DEL TIMER 1:
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // CONTAMOS CON DOS VELOCIDADES PARA EL SHOW DE LUCES:
    first_sequence++;
    second_sequence++;

    // AL ENTRAR AL MODO, RESET AL COUNTER:
    if (second_sequence == 8)
    {
        second_sequence = 0;
    }

    // AL ENTRAR AL MODO, RESET AL COUNTER:
    if (first_sequence == 6)
    {
        first_sequence = 0;
    }

}
void GPIOIntHandler(void)
{
   // PUSH BUTTON #1:
   if ((GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6)&0x40)==0x40)
   {
       // CLEAR A LA INTERRUPCIÓN:
       GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

       if (state == 1)
       {
           // ESTADO ACTUAL DE LEDS Y 7-SEGMENTOS AL UART:
           sending = 1;
       }
       // CUIDADO CON EL OVERFLOW:
       // AL ESTAR EN EL ESTADO #2:
       else if (state == 2)
       {
           // ++ AL CONTADOR BINARIO:
           bit04_counter++;

           // PARA EL OVERFLOW:
           if (bit04_counter == 16)
           {
               // AL LLEGAR A 15, VOLVER A CERO:
           }
               bit04_counter = 0;
           }
       }

   }
   // PUSH BUTTON #2:
   if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4)&0x10)==0x10)
      {
           // CLEAR A LA INTERRUPCIÓN:
           GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_4);

           if (state == 1)
          {
              // ESTADO ACTUAL DE LEDS Y 7-SEGMENTOS AL UART:
              sending = 1;
          }

          else if (state == 2)
          {
              // -- AL CONTADOR BINARIO:
              bit04_counter--;
              // PARA EL OVERFLOW:
              if (bit04_counter == -1)
              {
                  // AL LLEGAR A 0, VOLVER A 15:
                  bit04_counter = 15;
              }
          }
      }

}
