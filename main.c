#include "stdint.h"
#include "stdio.h"
#include "tm4c123gh6pm.h"

void ADC0_Init(void);
void uDMA_Init(void);

volatile static uint16_t adc_val = 0;

uint16_t DMA_Control_Table[1024];

uint16_t DMA_Primary_Receive_Buffer[64];
uint16_t DMA_Alternate_Receive_Buffer[64];


int main(void)
{
    ADC0_Init();

    while(1);

    return 0;
}

/*==============================================================================================/
 * ADC0_Init
 * Description: Initializes ADC0 on TM4C123GH6PM
 * Configuration: SS3, 1Msps, Interrupt triggered, Continuous transfer
 * Parameters: N/A
 * Returns: N/A
/==============================================================================================*/
void ADC0_Init()
{
// ======= MODULE INIT =======
    SYSCTL_RCGCADC_R |= (1<<0);     // Enable ADC0 clocK
    SYSCTL_RCGCGPIO_R |= (1<<4);    // Enable GPIO clock PORTE

    GPIO_PORTE_DIR_R &= ~(1<<3);    // Set PE3 as input

    GPIO_PORTE_AFSEL_R |= (1<<3);   // Set alternate function for ADC input (PE3)
    GPIO_PORTE_DEN_R &= ~(1<<3);    // Configure AINx bits for analog input (clear DEN for PE3)
    GPIO_PORTE_AMSEL_R |= (1<<3);   // Disable analog isolation for ADC input pins (PE3)

// ======= Sample Sequencer Config =======
    ADC0_ACTSS_R &= ~(1<<3);        // Disable Sequence Sampler 3
    ADC0_EMUX_R |= 0xF000;          // Configure Trigger Event (continuous)
    ADC0_SSMUX3_R = 0x0;            // Configure source for sample AIN0 = PE3
    ADC0_SSCTL3_R = 0x6;            // Sample Interrupt Enable, END0 bit
    ADC0_IM_R = (1<<3);             // Enable Interrupts
    ADC0_ACTSS_R |= (1<<3);         // Re-enable Sequence Sampler 3
    ADC0_ISC_R |= (1<<3);           // Reset Interrupt Clear Flag

    NVIC_EN0_R |= (1<<17);          // Enable ADC0_SS3 interrupt in the NVIC
}

//==============================================================================================//
void ADC0_Handler()
{
    adc_val = ADC0_SSFIFO3_R & 0xFFF; // Read ADC Value (12 bit)
    ADC0_ISC_R |= (1<<3);             // Reset Interrupt Clear Flag
    return;
}
//==============================================================================================//






/*==============================================================================================/
 * uDMA_Init
 * Description: Initializes uDMA input stream on TM4C123GH6PM
 * Configuration: Continuous Ping Pong Receive, DMA Channel 8
 * Parameters: N/A
 * Returns: N/A
/==============================================================================================*/
void uDMA_Init()
{
        // Receive stream (Peripheral to Memory)
        // ======= MODULE INIT ======= //
        SYSCTL_RCGCDMA_R |= 0x1;        // Enable the uDMA clock
        UDMA_CFG_R |= 0x1;              // Enable the uDMA controller
        //------                        // Program the location of the channel control table. The base address must be 1024 aligned


        // ==== Configure the Channel Attributes ==== //
        UDMA_PRIOSET_R |= (1<<8);       // Configure bit 8 to set the channel to High priority.
        UDMA_ALTCLR_R |= (1<<8);        // Set bit 8 to select the primary channel control structure for this transfer.
        UDMA_USEBURSTCLR_R |= (1<<8);   // Set bit 8 to allow the uDMA controller to respond to single and burst requests.
        UDMA_REQMASKCLR_R |= (1<<8);    // Set bit 8 to allow the uDMA controller to recognize requests for this channel.x

        // ==== Configure the Channel Control Structure ==== //

        // ==== Configure the Peripheral Interrupt ==== //

        // ==== Enable the uDMA Channel ==== //
        UDMA_ENASET_R |= (1<<8);        // Enable channel 8


        return;
}

//==============================================================================================//
void uDMA_Handler(){}
//==============================================================================================//






