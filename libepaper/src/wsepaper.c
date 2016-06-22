/*
 * wsepaper.c
 *
 *  Created on: Jun 21, 2016
 *      Author: robin
 */

#include "wsepaper.h"

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define WSE_BUF_SIZE 1024

/*
 command define
 */
#define WSE_CMD_HANDSHAKE                      0x00                                                     //handshake
#define WSE_CMD_SET_BAUD                       0x01                                                     //set baud
#define WSE_CMD_READ_BAUD                      0x02                                                     //read baud
#define WSE_CMD_MEMORYMODE                     0x07                                                     //set memory mode
#define WSE_CMD_STOPMODE                       0x08                                                     //enter stop mode
#define WSE_CMD_UPDATE                         0x0A                                                     //update
#define WSE_CMD_SCREEN_ROTATION                0x0D                                                     //set screen rotation
#define WSE_CMD_LOAD_FONT                      0x0E                                                     //load font
#define WSE_CMD_LOAD_PIC                       0x0F                                                     //load picture

#define WSE_CMD_SET_COLOR                      0x10                                                     //set color
#define WSE_CMD_SET_EN_FONT                    0x1E                                                     //set english font
#define WSE_CMD_SET_CH_FONT                    0x1F                                                     //set chinese font

#define WSE_CMD_DRAW_PIXEL                     0x20                                                     //set pixel
#define WSE_CMD_DRAW_LINE                      0x22                                                     //draw line
#define WSE_CMD_FILL_RECT                      0x24                                                     //fill rectangle
#define WSE_CMD_DRAW_CIRCLE                    0x26                                                     //draw circle
#define WSE_CMD_FILL_CIRCLE                    0x27                                                     //fill circle
#define WSE_CMD_DRAW_TRIANGLE                  0x28                                                     //draw triangle
#define WSE_CMD_FILL_TRIANGLE                  0x29                                                     //fill triangle
#define WSE_CMD_CLEAR                          0x2E                                                     //clear screen use back color

#define WSE_CMD_DRAW_STRING                    0x30                                                     //draw string

#define WSE_CMD_DRAW_BITMAP                    0x70                                                     //draw bitmap

struct wse_t {
	int fd;
	unsigned char parity;
	char buf[WSE_BUF_SIZE];
};

static const unsigned char _frame_header[] = { 0xA5, 0, 0, 0 };
static const unsigned char _frame_trailer[] = { 0xCC, 0x33, 0xC3, 0x3C, 0 };

static unsigned char _update_parity(struct wse_t *epd, size_t len) {
	unsigned char p = epd->parity;
	char *buf = epd->buf;
	while(len-- > 0)
		p ^= *(buf++);
	return (epd->parity = p);
}

static int _write_header(struct wse_t *epd, size_t plen, char cmd) {
	char *buf = epd->buf;
	epd->parity = 0;

	size_t flen = sizeof(_frame_header) + plen + sizeof(_frame_trailer);

	memcpy(buf, _frame_header, sizeof(_frame_header)); buf++;
	*(buf++) = 0xFF & (flen >> 8);
	*(buf++) = 0xFF & flen;
	*(buf++) = cmd;

	_update_parity(epd, buf - epd->buf);
	return write(epd->fd, epd->buf, buf - epd->buf);
}

static int _write_param(struct wse_t *epd, size_t plen) {
	_update_parity(epd, plen);
	return write(epd->fd, epd->buf, plen);
}

static int _write_trailer(struct wse_t *epd) {
	char *buf = epd->buf;

	memcpy(buf, _frame_trailer, sizeof(_frame_trailer)); buf += sizeof(_frame_trailer) - 1;
	*buf = _update_parity(epd, buf - epd->buf); buf++;

	return write(epd->fd, epd->buf, buf - epd->buf);
}

#define _put_byte(epd, s, c) { \
		epd->buf[s++] = (c); \
}
#define _put_short(epd, s, i) { \
		epd->buf[s++] = 0xFF & ((i) >> 8); \
		epd->buf[s++] = 0xFF & (i); \
}
#define _put_string(epd, s, str) { \
	strcpy(epd->buf + s, str); \
	s += strlen(str) + 1; \
}

#define _try_write_header(epd, flen, cmd) if(_write_header(epd, flen, cmd)) { return -1; }
#define _try_write_param(epd, plen) if(_write_param(epd, plen)) { return -1; }
#define _try_write_trailer(epd) if(_write_trailer(epd)) { return -1; }

int wse_open(struct wse_t *epd, const char *tty_dev) {
	int fd;
	if(tty_dev) {
		if((fd = open(tty_dev, O_WRONLY | O_NOCTTY)) < 0)
			return -1;
	} else
		fd = STDOUT_FILENO;

	epd->fd = fd;

	return 0;
}

int wse_open_fd(struct wse_t *epd, int fd) {
	epd->fd = fd;
	return 0;
}

int wse_close(struct wse_t *epd) {
	return close(epd->fd);
}

int wse_term_init(struct wse_t *epd) {
	struct termios tty;

	bzero(&tty, sizeof(tty));

	if(tcgetattr(epd->fd, &tty) != 0)
		return -1;

	cfsetspeed(&tty, B115200);
	cfmakeraw(&tty);

	tty.c_cflag |= HUPCL;

	if(tcsetattr(epd->fd, TCSADRAIN, &tty) != 0)
		return -1;

	return 0;
}

int wse_enter_stopmode(struct wse_t *epd) {
	_try_write_header(epd, 0, WSE_CMD_STOPMODE);
	_try_write_trailer(epd);
	return 0;
}

int wse_set_memory(struct wse_t *epd, unsigned char mode) {
	_try_write_header(epd, 1, WSE_CMD_STOPMODE);

	size_t plen = 0;
	_put_byte(epd, plen, mode);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}

int wse_udpate(struct wse_t *epd) {
	_try_write_header(epd, 0, WSE_CMD_UPDATE);
	_try_write_trailer(epd);
	return 0;
}

int wse_screen_rotation(struct wse_t *epd, unsigned char mode) {
	_try_write_header(epd, 1, WSE_CMD_SCREEN_ROTATION);

	size_t plen = 0;
	_put_byte(epd, plen, mode);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}

int wse_load_font(struct wse_t *epd) {
	_try_write_header(epd, 0, WSE_CMD_LOAD_FONT);
	_try_write_trailer(epd);
	return 0;
}
int wse_load_pic(struct wse_t *epd) {
	_try_write_header(epd, 0, WSE_CMD_LOAD_PIC);
	_try_write_trailer(epd);
	return 0;
}


int wse_set_color(struct wse_t *epd, unsigned char color, unsigned char bkcolor) {
	_try_write_header(epd, 1 + 1, WSE_CMD_SET_COLOR);

	size_t plen = 0;
	_put_byte(epd, plen, color);
	_put_byte(epd, plen, bkcolor);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_set_en_font(struct wse_t *epd, unsigned char font) {
	_try_write_header(epd, 1, WSE_CMD_SET_EN_FONT);

	size_t plen = 0;
	_put_byte(epd, plen, font);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_set_ch_font(struct wse_t *epd, unsigned char font) {
	_try_write_header(epd, 1, WSE_CMD_SET_CH_FONT);

	size_t plen = 0;
	_put_byte(epd, plen, font);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}

int wse_draw_pixel(struct wse_t *epd, int x0, int y0) {
	_try_write_header(epd, 2 + 2, WSE_CMD_DRAW_PIXEL);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_draw_line(struct wse_t *epd, int x0, int y0, int x1, int y1) {
	_try_write_header(epd, 2 + 2 + 2 + 2, WSE_CMD_DRAW_LINE);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, x1);
	_put_short(epd, plen, y1);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_fill_rect(struct wse_t *epd, int x0, int y0, int x1, int y1) {
	_try_write_header(epd, 2 + 2 + 2 + 2, WSE_CMD_FILL_RECT);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, x1);
	_put_short(epd, plen, y1);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_draw_circle(struct wse_t *epd, int x0, int y0, int r) {
	_try_write_header(epd, 2 + 2 + 2, WSE_CMD_DRAW_CIRCLE);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, r);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_fill_circle(struct wse_t *epd, int x0, int y0, int r) {
	_try_write_header(epd, 2 + 2 + 2, WSE_CMD_FILL_CIRCLE);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, r);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_draw_triangle(struct wse_t *epd, int x0, int y0, int x1, int y1, int x2, int y2) {
	_try_write_header(epd, 2 + 2 + 2 + 2 + 2 + 2, WSE_CMD_DRAW_TRIANGLE);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, x1);
	_put_short(epd, plen, y1);
	_put_short(epd, plen, x2);
	_put_short(epd, plen, y2);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_fill_triangle(struct wse_t *epd, int x0, int y0, int x1, int y1, int x2, int y2) {
	_try_write_header(epd, 2 + 2 + 2 + 2 + 2 + 2, WSE_CMD_FILL_TRIANGLE);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_short(epd, plen, x1);
	_put_short(epd, plen, y1);
	_put_short(epd, plen, x2);
	_put_short(epd, plen, y2);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
int wse_clear(struct wse_t *epd) {
	_try_write_header(epd, 0, WSE_CMD_CLEAR);
	_try_write_trailer(epd);
	return 0;
}

int wse_disp_char(struct wse_t *epd, unsigned char ch, int x0, int y0) {
	unsigned char buf[2] = { ch, 0 };
	return wse_disp_string(epd, buf, x0, y0);
}
int wse_disp_string(struct wse_t *epd, const void * p, int x0, int y0) {
	_try_write_header(epd, 2 + 2 + strlen(p) + 1, WSE_CMD_DRAW_STRING);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_string(epd, plen, p);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}

int wse_disp_bitmap(struct wse_t *epd, const void * p, int x0, int y0) {
	_try_write_header(epd, 2 + 2 + strlen(p) + 1, WSE_CMD_DRAW_BITMAP);

	size_t plen = 0;
	_put_short(epd, plen, x0);
	_put_short(epd, plen, y0);
	_put_string(epd, plen, p);
	_try_write_param(epd, plen);

	_try_write_trailer(epd);
	return 0;
}
