/*********************************************************************************************************
 *
 * File                : epd.c
 * Hardware Environment:
 * Build Environment   : RealView MDK-ARM  Version: 4.74
 * Version             : V1.0
 * By                  : V
 *
 *                                  (c) Copyright 2005-2014, WaveShare
 *                                       http://www.waveshare.net
 *                                          All Rights Reserved
 *
 *********************************************************************************************************/
#include "epd.h"

#include <string.h>

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include <fcntl.h>

static const unsigned char _cmd_read_baud[8] = {0xA5, 0x00, 0x09, CMD_READ_BAUD, 0xCC, 0x33, 0xC3, 0x3C};				//CMD_READ_BAUD
static const unsigned char _cmd_stopmode[8] = {0xA5, 0x00, 0x09, CMD_STOPMODE, 0xCC, 0x33, 0xC3, 0x3C}; 				//CMD_STOPMODE
static const unsigned char _cmd_update[8] = {0xA5, 0x00, 0x09, CMD_UPDATE, 0xCC, 0x33, 0xC3, 0x3C}; 					//CMD_UPDATE
static const unsigned char _cmd_load_font[8] = {0xA5, 0x00, 0x09, CMD_LOAD_FONT, 0xCC, 0x33, 0xC3, 0x3C};				//CMD_LOAD_FONT
static const unsigned char _cmd_load_pic[8] = {0xA5, 0x00, 0x09, CMD_LOAD_PIC, 0xCC, 0x33, 0xC3, 0x3C}; 				//CMD_LOAD_PIC


/*
private function
 */
static unsigned char _verify(const void * ptr, int n);

/*******************************************************************************
 * Function Name  : static unsigned char _verify(const void * ptr, int n)
 * Description    :
 * Input          : ptr
                   n        
 * Output         : None
 * Return         :
 *******************************************************************************/
static unsigned char _verify(const void * ptr, int n)
{
	int i;
	unsigned char * p = (unsigned char *)ptr;
	unsigned char result;

	for(i = 0, result = 0; i < n; i++)
	{
		result ^= p[i];
	}

	return result;
}

int epd_open(struct epd_t *epd, const char *tty_dev) {
	int tty_fd;

	if(tty_dev == NULL)
		tty_fd = STDOUT_FILENO;
	else
		tty_fd = open(tty_dev, O_RDWR | O_NOCTTY);

	if(tty_fd < 0)
		return -1;

	epd->tty_fd = tty_fd;

	return 0;
}

int epd_open_fd(struct epd_t *epd, int tty_fd) {
	epd->tty_fd = tty_fd;
	return 0;
}

/*******************************************************************************
 * Function Name  : void epd_init(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_init(struct epd_t *epd)
{
	struct termios tty;

	bzero(&tty, sizeof(tty));

	/* Error Handling */
	if(tcgetattr(epd->tty_fd, &tty) != 0)
		return -1;

	cfsetspeed(&tty, B115200);

	/* Make raw */
	cfmakeraw(&tty);

	tty.c_cflag |= HUPCL;

	if(tcsetattr(epd->tty_fd, TCSADRAIN, &tty) != 0)
		return -1;

	return 0;
}

/*******************************************************************************
 * Function Name  : void epd_enter_stopmode(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_enter_stopmode(struct epd_t *epd)
{
	memcpy(epd->cmd, _cmd_stopmode, 8);
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}

/*******************************************************************************
 * Function Name  : void epd_read_baud(struct epd_t *epd)
 * Description    : read uart baud
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_read_baud(struct epd_t *epd)
{
	memcpy(epd->cmd, _cmd_read_baud, 8);
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}
/*******************************************************************************
 * Function Name  : void epd_set_memory(unsigned char mode)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_set_memory(struct epd_t *epd, unsigned char mode)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0A;

	epd->cmd[3] = CMD_MEMORYMODE;

	epd->cmd[4] = mode;

	epd->cmd[5] = FRAME_E0;
	epd->cmd[6] = FRAME_E1;
	epd->cmd[7] = FRAME_E2;
	epd->cmd[8] = FRAME_E3;
	epd->cmd[9] = _verify(epd->cmd, 9);

	return write(epd->tty_fd, epd->cmd, 10);
}

/*******************************************************************************
 * Function Name  : void epd_udpate(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_udpate(struct epd_t *epd)
{
	memcpy(epd->cmd, _cmd_update, 8);
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}
/*******************************************************************************
 * Function Name  : void epd_screen_rotation(unsigned char mode)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_screen_rotation(struct epd_t *epd, unsigned char mode)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0A;

	epd->cmd[3] = CMD_SCREEN_ROTATION;

	epd->cmd[4] = mode;

	epd->cmd[5] = FRAME_E0;
	epd->cmd[6] = FRAME_E1;
	epd->cmd[7] = FRAME_E2;
	epd->cmd[8] = FRAME_E3;
	epd->cmd[9] = _verify(epd->cmd, 9);

	return write(epd->tty_fd, epd->cmd, 10);
}
/*******************************************************************************
 * Function Name  : void epd_load_font(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_load_font(struct epd_t *epd)
{
	memcpy(epd->cmd, _cmd_load_font, 8);
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}
/*******************************************************************************
 * Function Name  : void epd_load_pic(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_load_pic(struct epd_t *epd)
{
	memcpy(epd->cmd, _cmd_load_pic, 8);
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}
/*******************************************************************************
 * Function Name  : void epd_set_color(unsigned char color, unsigned char bkcolor)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_set_color(struct epd_t *epd, unsigned char color, unsigned char bkcolor)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0B;

	epd->cmd[3] = CMD_SET_COLOR;

	epd->cmd[4] = color;
	epd->cmd[5] = bkcolor;

	epd->cmd[6] = FRAME_E0;
	epd->cmd[7] = FRAME_E1;
	epd->cmd[8] = FRAME_E2;
	epd->cmd[9] = FRAME_E3;
	epd->cmd[10] = _verify(epd->cmd, 10);

	return write(epd->tty_fd, epd->cmd, 11);
}
/*******************************************************************************
 * Function Name  : void epd_set_en_font(unsigned char font)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_set_en_font(struct epd_t *epd, unsigned char font)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0A;

	epd->cmd[3] = CMD_SET_EN_FONT;

	epd->cmd[4] = font;

	epd->cmd[5] = FRAME_E0;
	epd->cmd[6] = FRAME_E1;
	epd->cmd[7] = FRAME_E2;
	epd->cmd[8] = FRAME_E3;
	epd->cmd[9] = _verify(epd->cmd, 9);

	return write(epd->tty_fd, epd->cmd, 10);
}
/*******************************************************************************
 * Function Name  : void epd_set_ch_font(unsigned char font)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_set_ch_font(struct epd_t *epd, unsigned char font)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0A;

	epd->cmd[3] = CMD_SET_CH_FONT;

	epd->cmd[4] = font;

	epd->cmd[5] = FRAME_E0;
	epd->cmd[6] = FRAME_E1;
	epd->cmd[7] = FRAME_E2;
	epd->cmd[8] = FRAME_E3;
	epd->cmd[9] = _verify(epd->cmd, 9);

	return write(epd->tty_fd, epd->cmd, 10);
}
/*******************************************************************************
 * Function Name  : void epd_draw_pixel(int x0, int y0)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_draw_pixel(struct epd_t *epd, int x0, int y0)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0D;

	epd->cmd[3] = CMD_DRAW_PIXEL;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;

	epd->cmd[8] = FRAME_E0;
	epd->cmd[9] = FRAME_E1;
	epd->cmd[10] = FRAME_E2;
	epd->cmd[11] = FRAME_E3;
	epd->cmd[12] = _verify(epd->cmd, 12);

	return write(epd->tty_fd, epd->cmd, 13);
}
/*******************************************************************************
 * Function Name  : void epd_draw_line(int x0, int y0, int x1, int y1)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_draw_line(struct epd_t *epd, int x0, int y0, int x1, int y1)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x11;

	epd->cmd[3] = CMD_DRAW_LINE;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (x1 >> 8) & 0xFF;
	epd->cmd[9] = x1 & 0xFF;
	epd->cmd[10] = (y1 >> 8) & 0xFF;
	epd->cmd[11] = y1 & 0xFF;

	epd->cmd[12] = FRAME_E0;
	epd->cmd[13] = FRAME_E1;
	epd->cmd[14] = FRAME_E2;
	epd->cmd[15] = FRAME_E3;
	epd->cmd[16] = _verify(epd->cmd, 16);

	return write(epd->tty_fd, epd->cmd, 17);
}
/*******************************************************************************
 * Function Name  : void epd_fill_rect(int x0, int y0, int x1, int y1)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_fill_rect(struct epd_t *epd, int x0, int y0, int x1, int y1)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x11;

	epd->cmd[3] = CMD_FILL_RECT;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (x1 >> 8) & 0xFF;
	epd->cmd[9] = x1 & 0xFF;
	epd->cmd[10] = (y1 >> 8) & 0xFF;
	epd->cmd[11] = y1 & 0xFF;

	epd->cmd[12] = FRAME_E0;
	epd->cmd[13] = FRAME_E1;
	epd->cmd[14] = FRAME_E2;
	epd->cmd[15] = FRAME_E3;
	epd->cmd[16] = _verify(epd->cmd, 16);

	return write(epd->tty_fd, epd->cmd, 17);
}
/*******************************************************************************
 * Function Name  : void epd_draw_circle(int x0, int y0, int r)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_draw_circle(struct epd_t *epd, int x0, int y0, int r)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0F;

	epd->cmd[3] = CMD_DRAW_CIRCLE;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (r >> 8) & 0xFF;
	epd->cmd[9] = r & 0xFF;


	epd->cmd[10] = FRAME_E0;
	epd->cmd[11] = FRAME_E1;
	epd->cmd[12] = FRAME_E2;
	epd->cmd[13] = FRAME_E3;
	epd->cmd[14] = _verify(epd->cmd, 14);

	return write(epd->tty_fd, epd->cmd, 15);
}
/*******************************************************************************
 * Function Name  : void epd_fill_circle(int x0, int y0, int r)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_fill_circle(struct epd_t *epd, int x0, int y0, int r)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x0F;

	epd->cmd[3] = CMD_FILL_CIRCLE;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (r >> 8) & 0xFF;
	epd->cmd[9] = r & 0xFF;


	epd->cmd[10] = FRAME_E0;
	epd->cmd[11] = FRAME_E1;
	epd->cmd[12] = FRAME_E2;
	epd->cmd[13] = FRAME_E3;
	epd->cmd[14] = _verify(epd->cmd, 14);

	return write(epd->tty_fd, epd->cmd, 15);
}
/*******************************************************************************
 * Function Name  : void epd_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_draw_triangle(struct epd_t *epd, int x0, int y0, int x1, int y1, int x2, int y2)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x15;

	epd->cmd[3] = CMD_DRAW_TRIANGLE;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (x1 >> 8) & 0xFF;
	epd->cmd[9] = x1 & 0xFF;
	epd->cmd[10] = (y1 >> 8) & 0xFF;
	epd->cmd[11] = y1 & 0xFF;
	epd->cmd[12] = (x2 >> 8) & 0xFF;
	epd->cmd[13] = x2 & 0xFF;
	epd->cmd[14] = (y2 >> 8) & 0xFF;
	epd->cmd[15] = y2 & 0xFF;

	epd->cmd[16] = FRAME_E0;
	epd->cmd[17] = FRAME_E1;
	epd->cmd[18] = FRAME_E2;
	epd->cmd[19] = FRAME_E3;
	epd->cmd[20] = _verify(epd->cmd, 20);

	return write(epd->tty_fd, epd->cmd, 21);
}
/*******************************************************************************
 * Function Name  : void epd_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_fill_triangle(struct epd_t *epd, int x0, int y0, int x1, int y1, int x2, int y2)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x15;

	epd->cmd[3] = CMD_FILL_TRIANGLE;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;
	epd->cmd[8] = (x1 >> 8) & 0xFF;
	epd->cmd[9] = x1 & 0xFF;
	epd->cmd[10] = (y1 >> 8) & 0xFF;
	epd->cmd[11] = y1 & 0xFF;
	epd->cmd[12] = (x2 >> 8) & 0xFF;
	epd->cmd[13] = x2 & 0xFF;
	epd->cmd[14] = (y2 >> 8) & 0xFF;
	epd->cmd[15] = y2 & 0xFF;

	epd->cmd[16] = FRAME_E0;
	epd->cmd[17] = FRAME_E1;
	epd->cmd[18] = FRAME_E2;
	epd->cmd[19] = FRAME_E3;
	epd->cmd[20] = _verify(epd->cmd, 20);

	return write(epd->tty_fd, epd->cmd, 21);
}
/*******************************************************************************
 * Function Name  : void epd_clear(struct epd_t *epd)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_clear(struct epd_t *epd)
{
	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = 0x00;
	epd->cmd[2] = 0x09;

	epd->cmd[3] = CMD_CLEAR;

	epd->cmd[4] = FRAME_E0;
	epd->cmd[5] = FRAME_E1;
	epd->cmd[6] = FRAME_E2;
	epd->cmd[7] = FRAME_E3;
	epd->cmd[8] = _verify(epd->cmd, 8);

	return write(epd->tty_fd, epd->cmd, 9);
}

/*******************************************************************************
 * Function Name  : void epd_disp_char(unsigned char ch, int x0, int y0);
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_disp_char(struct epd_t *epd, unsigned char ch, int x0, int y0)
{
	unsigned char buff[2];

	buff[0] = ch;
	buff[1] = 0;

	return epd_disp_string(epd, buff, x0, y0);
}
/*******************************************************************************
 * Function Name  : void epd_disp_string(const void * p, int x0, int y0)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_disp_string(struct epd_t *epd, const void * p, int x0, int y0)
{
	int string_size;
	unsigned char * ptr = (unsigned char *)p;


	string_size = strlen((const char *)ptr);
	string_size += 14;

	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = (string_size >> 8) & 0xFF;
	epd->cmd[2] = string_size & 0xFF;

	epd->cmd[3] = CMD_DRAW_STRING;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;

	strcpy((char *)(&epd->cmd[8]), (const char *)ptr);

	string_size -= 5;

	epd->cmd[string_size] = FRAME_E0;
	epd->cmd[string_size + 1] = FRAME_E1;
	epd->cmd[string_size + 2] = FRAME_E2;
	epd->cmd[string_size + 3] = FRAME_E3;
	epd->cmd[string_size + 4] = _verify(epd->cmd, string_size + 4);

	return write(epd->tty_fd, epd->cmd, string_size + 5);
}
/*******************************************************************************
 * Function Name  : void epd_disp_bitmap(const void * p, int x0, int y0)
 * Description    :
 * Input          :
 * Output         : None
 * Return         :
 * Attention		   : None
 *******************************************************************************/
int epd_disp_bitmap(struct epd_t *epd, const void * p, int x0, int y0)
{
	int string_size;
	unsigned char * ptr = (unsigned char *)p;

	string_size = strlen((const char *)ptr);
	string_size += 14;

	epd->cmd[0] = FRAME_B;

	epd->cmd[1] = (string_size >> 8) & 0xFF;
	epd->cmd[2] = string_size & 0xFF;

	epd->cmd[3] = CMD_DRAW_BITMAP;

	epd->cmd[4] = (x0 >> 8) & 0xFF;
	epd->cmd[5] = x0 & 0xFF;
	epd->cmd[6] = (y0 >> 8) & 0xFF;
	epd->cmd[7] = y0 & 0xFF;

	strcpy((char *)(&epd->cmd[8]), (const char *)ptr);

	string_size -= 5;

	epd->cmd[string_size] = FRAME_E0;
	epd->cmd[string_size + 1] = FRAME_E1;
	epd->cmd[string_size + 2] = FRAME_E2;
	epd->cmd[string_size + 3] = FRAME_E3;
	epd->cmd[string_size + 4] = _verify(epd->cmd, string_size + 4);

	return write(epd->tty_fd, epd->cmd, string_size + 5);
}




































