#include "oled.h"
#include "main.h"
#include <stdint.h>
#include <string.h>

/* SH1106 内部是 132 列，常见 128x64 屏幕需要偏移 2 列 如有偏移改回即可 */
#define OLED_COLUMN_OFFSET  2

// DC CS RST引脚操作封装

static void oled_select(OledTypedef *oled)
{
	HAL_GPIO_WritePin(oled->CS_port, oled->CS_pin, GPIO_PIN_RESET);
}

static void oled_deselect(OledTypedef *oled)
{
	HAL_GPIO_WritePin(oled->CS_port, oled->CS_pin, GPIO_PIN_SET);
}

static void oled_set_commandmode(OledTypedef *oled)
{
	HAL_GPIO_WritePin(oled->DC_port, oled->DC_pin, GPIO_PIN_RESET);
}

static void oled_set_datamode(OledTypedef *oled)
{
	HAL_GPIO_WritePin(oled->DC_port, oled->DC_pin, GPIO_PIN_SET);
}

static void oled_reset(OledTypedef *oled)
{
	HAL_GPIO_WritePin(oled->RST_port, oled->RST_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(oled->RST_port, oled->RST_pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

// SPI发送

static int oled_send_command(OledTypedef *oled,  uint8_t cmd, const uint8_t *arg, uint16_t size)
{
	oled_set_commandmode(oled);
	oled_select(oled);

	if (HAL_SPI_Transmit(oled->hspi, &cmd, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		oled_deselect(oled);
		return -1;
	}
	if (size > 0 && arg != NULL)
	{
		if (HAL_SPI_Transmit(oled->hspi, (uint8_t *)arg, size, HAL_MAX_DELAY) != HAL_OK)
		{
			oled_deselect(oled);
			return -1;
		}
	}

	oled_deselect(oled);
	return 0;
}

static int oled_send_data(OledTypedef *oled, const uint8_t *data, uint16_t size)
{
	oled_set_datamode(oled);
	oled_select(oled);

	if (HAL_SPI_Transmit(oled->hspi, (uint8_t *)data, size, HAL_MAX_DELAY) != HAL_OK)
	{
		oled_deselect(oled);
		return -1;
	}

	oled_deselect(oled);
	return 0;
}

// OLED初始化

int oled_init(OledTypedef *oled, OledInitTypeDef *init)
{
	uint8_t arg;

	if (oled == NULL || init == NULL)
	{
		return -1;
	}

	oled->hspi = init->hspi;

	oled->CS_port = init->CS_port;
	oled->CS_pin  = init->CS_pin;

	oled->DC_port = init->DC_port;
	oled->DC_pin  = init->DC_pin;

	oled->RST_port = init->RST_port;
	oled->RST_pin  = init->RST_pin;

	oled_deselect(oled);
	oled_reset(oled);

	memset(oled->buffer, 0x00, OLED_BUFFER_SIZE);

	/* SH1106 初始化命令 */

	/* 关闭显示 */
	if (oled_send_command(oled, 0xAE, NULL, 0) != 0) return -1;

	/* 设置显示起始行 */
	if (oled_send_command(oled, 0x40, NULL, 0) != 0) return -1;

	/* 设置页地址起始页，一般从 page 0 开始 */
	if (oled_send_command(oled, 0xB0, NULL, 0) != 0) return -1;

	/* 设置列地址低 4 位 */
	if (oled_send_command(oled, 0x02, NULL, 0) != 0) return -1;

	/* 设置列地址高 4 位 */
	if (oled_send_command(oled, 0x10, NULL, 0) != 0) return -1;

	/* 设置段重映射 */
	if (oled_send_command(oled, 0xA1, NULL, 0) != 0) return -1;

	/* 正常显示，不反色 */
	if (oled_send_command(oled, 0xA6, NULL, 0) != 0) return -1;

	/* 设置多路复用率，64 行为 0x3F */
	arg = 0x3F;
	if (oled_send_command(oled, 0xA8, &arg, 1) != 0) return -1;

	/* 显示内容来自 RAM */
	if (oled_send_command(oled, 0xA4, NULL, 0) != 0) return -1;

	/* 设置显示偏移 */
	arg = 0x00;
	if (oled_send_command(oled, 0xD3, &arg, 1) != 0) return -1;

	/* 设置显示时钟分频 */
	arg = 0x80;
	if (oled_send_command(oled, 0xD5, &arg, 1) != 0) return -1;

	/* 设置预充电周期 */
	arg = 0x1F;
	if (oled_send_command(oled, 0xD9, &arg, 1) != 0) return -1;

	/* 设置 COM 扫描方向 */
	if (oled_send_command(oled, 0xC8, NULL, 0) != 0) return -1;

	/* 设置 COM 引脚配置 */
	arg = 0x12;
	if (oled_send_command(oled, 0xDA, &arg, 1) != 0) return -1;

	/* 设置 VCOMH */
	arg = 0x40;
	if (oled_send_command(oled, 0xDB, &arg, 1) != 0) return -1;

	/* 设置对比度 */
	arg = 0x7F;
	if (oled_send_command(oled, 0x81, &arg, 1) != 0) return -1;

	/* 打开 DC-DC */
	if (oled_send_command(oled, 0xAD, NULL, 0) != 0) return -1;
	arg = 0x8B;
	if (oled_send_command(oled, arg, NULL, 0) != 0) return -1;

	/* 打开显示 */
	if (oled_send_command(oled, 0xAF, NULL, 0) != 0) return -1;

	oled_clear_buffer(oled);

	if (oled_refresh(oled) != 0)
	{
		return -1;
	}

	return 0;
}// cv的oled驱动芯片sh(ssd?)1106初始化函数(固定的)

// 剩余对外接口实现

int oled_show_frame(OledTypedef *oled, const uint8_t *frame) {
	
	uint8_t page;
	uint8_t column;

	if (oled == NULL || frame == NULL) {
		return -1;
	}

	memcpy(oled->buffer, frame, OLED_BUFFER_SIZE);

	return oled_refresh(oled);
}

void oled_clear_buffer(OledTypedef *oled)
{
	if (oled == NULL)
	{
		return;
	}

	memset(oled->buffer, 0x00, OLED_BUFFER_SIZE);
}

int oled_refresh(OledTypedef *oled)
{
	uint8_t page;
	uint8_t column;
	column = OLED_COLUMN_OFFSET;

	if (oled == NULL)
	{
		return -1;
	}

	for (page = 0; page < OLED_PAGES; page++)
	{
		if (oled_send_command(oled, 0xB0 + page, NULL, 0) != 0)
		{
			return -1;
		}

		column = OLED_COLUMN_OFFSET;

		if (oled_send_command(oled, 0x00 + (column & 0x0F), NULL, 0) != 0)
		{
			return -1;
		}

		if (oled_send_command(oled, 0x10 + ((column >> 4) & 0x0F), NULL, 0) != 0)
		{
			return -1;
		}

		if (oled_send_data(oled, &oled->buffer[page * OLED_WIDTH], OLED_WIDTH) != 0)
		{
			return -1;
		}
	}

	return 0;
}
