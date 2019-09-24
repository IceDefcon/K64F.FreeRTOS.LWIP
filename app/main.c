#include "FreeRTOS.h"
#include "task.h"

#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/prot/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "enet_ethernetif.h"

#include "board.h"

#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"

//                                             // K64 Sub-Family Reference Manual
//                                             // ==================================
//                                             //                      (page number)
// #define SIM_SCGC5 (*(int *)0x40048038u)     // Clock gate 5                 (314)
// #define SIM_SCGC5_PORTB 10                  // Open gate PORTB              (314)

// #define PORTB_PCR21 (*(int *)0x4004A054u)   // Pin Control Register         (277)
// #define PORTB_PCR21_MUX 8                   // Mux "001"                    (282)

// #define GPIOB_PDDR (*(int *)0x400FF054u)    // Port Data Direction Register (1760)
// #define GPIOB_PDOR (*(int *)0x400FF040u)    // Port Data Output Register    (1759)
// #define PIN_N 21                            // PTB21 --> Blue LED  			(1761)

// void init_gpio()
// {
//     /* Enable clocks. */
//     SIM_SCGC5 |= 1 << SIM_SCGC5_PORTB;
//     /* Configure pin 21 as GPIO. */
//     PORTB_PCR21 |= 1 << PORTB_PCR21_MUX;
//     /* Configure GPIO pin 21 as output.
//      * It will have a default output value set
//      * to 0, so LED will light (negative logic).
//      */
//     GPIOB_PDDR |= 1 << PIN_N;
// }

// void blinky_task()
// {
//     while(1) {
//         GPIOB_PDOR ^= 1 << PIN_N;               // Toggle with XOR
//         vTaskDelay(500 / portTICK_PERIOD_MS);   // 500/10
//     }
// }

#define configMAC_ADDR                     \
{                                      \
    0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
}

#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS
#define EXAMPLE_CLOCK_NAME kCLOCK_CoreSysClk
#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN
#define BOARD_SW_GPIO BOARD_SW3_GPIO
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN
#define BOARD_SW_PORT BOARD_SW3_PORT
#define BOARD_SW_IRQ BOARD_SW3_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER
#define PRINT_THREAD_STACKSIZE 512
#define PRINT_THREAD_PRIO DEFAULT_THREAD_PRIO

int main()
{
    // init_gpio();

    // xTaskCreate(blinky_task, "Blinky", 100, NULL, 1, NULL); 	


    static struct netif fsl_netif0;
    ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    
    ethernetif_config_t fsl_enet_config0 = 
    {
        .phyAddress = EXAMPLE_PHY_ADDRESS,
        .clockName  = EXAMPLE_CLOCK_NAME,
        .macAddress = configMAC_ADDR,
    };

    SYSMPU_Type *base = SYSMPU;
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Disable SYSMPU. */
    base->CESR &= ~SYSMPU_CESR_VLD_MASK;

    IP4_ADDR(&fsl_netif0_ipaddr, 10U, 0U, 0U, 2U);
    IP4_ADDR(&fsl_netif0_netmask, 255U, 255U, 255U, 0U);
    IP4_ADDR(&fsl_netif0_gw, 10U, 0U, 0U, 1U);

    tcpip_init(NULL, NULL);

    netifapi_netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, &fsl_enet_config0,
                       ethernetif0_init, tcpip_input);
    netifapi_netif_set_default(&fsl_netif0);
    netifapi_netif_set_up(&fsl_netif0);

    netifapi_dhcp_start(&fsl_netif0);

    vTaskStartScheduler();
    return 0;
}
