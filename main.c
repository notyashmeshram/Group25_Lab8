#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

uint8_t message = 0x00;
uint8_t PORTF_Interrupt = 0x00;

void GPIO_PORTF_Interrupt();

int main(void)
{
    SYSCTL_RCGC2_R |= 0x00000020;       

    // GPIO registers port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;    
    GPIO_PORTF_CR_R = 0x1F;             
    GPIO_PORTF_DEN_R = 0x1F;            
    GPIO_PORTF_DIR_R = 0x0E;            
    GPIO_PORTF_PUR_R = 0x11;            

    NVIC_EN0_R = 0x40000000; 
    GPIO_PORTF_IS_R = 0x00; 
    GPIO_PORTF_IEV_R = 0x00; 
    GPIO_PORTF_IM_R = 0x10; 

    SYSCTL_RCGCUART_R |= 0x01; 
    SYSCTL_RCGCGPIO_R |= 0x21; 

    // GPIO Registers port A
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     
    GPIO_PORTA_CR_R = 0x03;             
    GPIO_PORTA_DEN_R = 0x03;
    GPIO_PORTA_AFSEL_R = 0x03; 
    GPIO_PORTA_PCTL_R = 0x11; 
    GPIO_PORTA_DIR_R = 0x02;
    GPIO_PORTA_PUR_R = 0x02;



    // UART contol registers
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;
    UART0_LCRH_R |= 0x62;
    UART0_CC_R = 0x00;
    UART0_CTL_R |= 0x01; 
    uint8_t rx_reg = 0x00;

    while(1){
        NVIC_EN0_R = 0x40000000; 
        GPIO_PORTF_IM_R = 0x11; 

        UART0_DR_R = message; 
        while (UART0_FR_R & 0x08){
            ; // wait till transmission is complete
        }
        
        rx_reg = UART0_DR_R & 0xFFF; // read least significant byte
        if ((rx_reg & 0xFF) == 0x42){
            GPIO_PORTF_DATA_R &= 0x04;
            GPIO_PORTF_DATA_R ^= 0x04;
            message = 0x42;
            delay(500000);
        }
        else if ((rx_reg & 0xFF) == 0x47){
            GPIO_PORTF_DATA_R &= 0x08;
            GPIO_PORTF_DATA_R ^= 0x08;
            message = 0x47;
            delay(500000);
        }
        else if ((rx_reg & 0xFF) == 0x52){
            GPIO_PORTF_DATA_R &= 0x02;
            GPIO_PORTF_DATA_R ^= 0x02;
            message = 0x72;
            delay(500000);
        }

        else{ 
            GPIO_PORTF_DATA_R &= 0x00;
            message = 0x3F;
        }

    }

    return 0;
}

void delay(int us){
    NVIC_ST_RELOAD_R = 16*us;
    NVIC_ST_CTRL_R = 0x00000005;
    while( (NVIC_ST_CTRL_R & 0x00010000) != 0x00010000 ){;} 
    NVIC_ST_CTRL_R = 0x00000000;
}

// not asked in Lab8 but still continuation from Lab7 for switch press
void GPIO_PORTF_Interrupt(){
    PORTF_Interrupt = GPIO_PORTF_RIS_R & 0x11; 

    
    NVIC_EN0_R = 0x00000000; 
    GPIO_PORTF_IM_R = 0x00; 
    if (PORTF_Interrupt == 0x01){ // switch was pressed
        GPIO_PORTF_ICR_R = 0x11; 
        message = 0xAA;
    }
    if (PORTF_Interrupt == 0x10){ // switch was pressed
        GPIO_PORTF_ICR_R = 0x11; 
        message = 0xF0;
    }

}
