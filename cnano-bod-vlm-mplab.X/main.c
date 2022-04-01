/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#define F_CPU                  4000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define BOD_VLM_INT_EN_gc      (0x01 << 0)
#define BOD_VLM_INT_FLAG_gc    (0x01 << 0)
#define BLINK_DELAY            150

/* Fuses configuration:
- BOD enabled in Active Mode, set to level 1(2.4V)
- Oscillator in High-Frequency Mode
- UPDI pin active(WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply
- Watchdog Timer disabled
*/
FUSES =
{
    .BODCFG = ACTIVE_ENABLE_gc | LVL_BODLEVEL1_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
    .BOOTSIZE = 0x0,
    .CODESIZE = 0x0,
    .OSCCFG = CLKSEL_OSCHF_gc,
    .SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
    .SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
    .WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};


static void VLM_Init(void);
static void LED_Init(void);
static void LED_Blink(void);

static bool volatile vlm_flag = false;

int main(void) {
    
    VLM_Init();
    LED_Init();
    LED_Blink();
    
    sei();
    
    while(1)
    {
        if(vlm_flag == true)
        {
            PORTF.OUTTGL = PIN5_bm;
            vlm_flag = false;
        }
    }
}

static void VLM_Init(void)
{
    BOD.VLMCTRLA = BOD_VLMLVL_25ABOVE_gc;
    BOD.INTCTRL = BOD_VLMCFG_BOTH_gc |
                  BOD_VLM_INT_EN_gc;
}

static void LED_Init(void)
{
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
}

static void LED_Blink(void)
{
    uint8_t i;
    for( i = 0 ; i < 3 ; i++ )
    {
        _delay_ms(BLINK_DELAY);
        PORTF.OUTCLR = PIN5_bm; //turn LED ON
        
        _delay_ms(BLINK_DELAY);
        PORTF.OUTSET = PIN5_bm; //turn LED OFF
    }
}

ISR(BOD_VLM_vect)
{
    vlm_flag = true;
    BOD.INTFLAGS = BOD_VLM_INT_FLAG_gc;
}