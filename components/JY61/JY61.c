#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "Led.h"
#include "JY61.h"


#define UART1_TXD  (UART_PIN_NO_CHANGE)
#define UART1_RXD  (GPIO_NUM_35)
#define UART1_RTS  (UART_PIN_NO_CHANGE)
#define UART1_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE    100


static const char *TAG = "JY61";

void JY61_Read_Task(void* arg);


void JY61_Init(void)
{
     //配置GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask =  1ULL << UART1_RXD;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART1_TXD, UART1_RXD, UART1_RTS, UART1_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    xTaskCreate(&JY61_Read_Task, "JY61_Read_Task", 2048, NULL, 10, NULL);
}


void JY61_Read_Task(void* arg)
{
    uint8_t data_u1[BUF_SIZE];
    int i;
    float x,y,z;
    uint16_t RollL,RollH,PitchL,PitchH,YawL,YawH;
    while(1)
    {
        int len1 = uart_read_bytes(UART_NUM_1, data_u1, BUF_SIZE, 10 / portTICK_RATE_MS);
        if(len1!=0)  //读取到数据
        {
            // for(int i=0;i<len1;i++)
            // {
            //     printf("0x%x ",data_u1[i]);
            // }
            // printf("\r\n");
            // printf("\r\n");
            // printf("\r\n");
            for(i=0;i<len1;i++)
            {
                if((data_u1[i]==0x55)&&(data_u1[i+1]==0x53))
                {
                    if((len1-i)>10)
                    {
                        RollL=data_u1[i+2];
                        RollH=data_u1[i+3];
                        PitchL=data_u1[i+4];
                        PitchH=data_u1[i+5];
                        YawL=data_u1[i+6];
                        YawH=data_u1[i+7];

                        //printf("RollL=0x%x,RollH=0x%x,PitchL=0x%x,PitchH=0x%x,YawL=0x%x,YawH=0x%x\r\n",RollL,RollH,PitchL,PitchH,YawL,YawH);

                        x = ((RollH<<8)|RollL)/32768.0*180;
                        y = ((PitchH<<8)|PitchL)/32768.0*180;
                        z = 0;
                        //x=(float)(((uint16_t)RollH<<8)|RollL)/32768*180;
                        //y=(float)(((uint16_t)PitchH<<8)|PitchL)/32768*180;
                        //z=(float)(((uint16_t)YawH<<8)|YawL)/32768*180;

                        printf("x=%3.2f,y=%3.2f\r\n",x,y);
                        break;
                    }
                }
            }
            len1=0;
            bzero(data_u1,sizeof(data_u1));                 
        }  
        vTaskDelay(5 / portTICK_RATE_MS);
    }   
}


