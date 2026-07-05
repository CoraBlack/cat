#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "stm32f4xx_hal_spi.h"
#include <stdint.h>
	
#define OLED_WIDTH 			128
#define OLED_HEIGHT 		64
#define OLED_PAGES 			8
#define OLED_BUFFER_SIZE 	(OLED_WIDTH * OLED_PAGES)	

typedef struct {
	SPI_HandleTypeDef *hspi;

	GPIO_TypeDef *CS_port;
    uint16_t CS_pin;

    GPIO_TypeDef *DC_port;
    uint16_t DC_pin;

    GPIO_TypeDef *RST_port;
    uint16_t RST_pin;

    uint8_t buffer[OLED_BUFFER_SIZE];
}  OledTypedef;//存储oled当前状态

typedef struct
{
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *CS_port;
    uint16_t CS_pin;

    GPIO_TypeDef *DC_port;
    uint16_t DC_pin;

    GPIO_TypeDef *RST_port;
    uint16_t RST_pin;

} OledInitTypeDef;//oled初始化结构体

/**
 * @brief  初始化 OLED 屏幕
 * @param  oled OLED 设备对象指针，用于保存 SPI 句柄、GPIO 引脚和显存缓冲区等信息
 * @param  init OLED 初始化配置结构体指针，用于传入 SPI 句柄、CS/DC/RST 引脚信息
 * @retval 0 初始化成功
 * @retval -1 初始化失败
 */
int oled_init(OledTypedef *oled, OledInitTypeDef *init);

/**
 * @brief  清除 OLED 显示缓冲区
 * @param  oled OLED 设备对象指针
 * @retval 0 清除成功
 * @retval -1 清除失败
 */
void oled_clear_buffer(OledTypedef *oled);

/**
 * @brief  把缓冲区的刷新到屏幕上
 * @param  oled OLED 设备对象指针
 * @retval 0 清除成功
 * @retval -1 清除失败
 */
int oled_refresh(OledTypedef *oled);

/**
 * @brief  显示一帧完整位图
 * @param  oled OLED 设备对象指针
 * @param  frame 整屏位图数据指针，大小必须为 OLED_BUFFER_SIZE 字节
 * 				128 * 64 / 8 = 1024 字节
 * @retval 0 显示成功
 * @retval -1 显示失败
 * @note   frame是按页模式组织的，每页128字节，共8页，每一字节对应8个垂直像素，最低位在上，最高位在下。
 */
int oled_show_frame(OledTypedef *oled, const uint8_t *frame);

#endif

