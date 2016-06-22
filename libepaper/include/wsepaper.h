/*
 * wsepaper.h
 *
 *  Created on: Jun 21, 2016
 *      Author: robin
 */

#ifndef WSEPAPER_H_
#define WSE_WSEPAPER_H_

/*
 color define
 */
#define WSE_WHITE                              0x03
#define WSE_GRAY                               0x02
#define WSE_DARK_GRAY                          0x01
#define WSE_BLACK                              0x00

/*
 FONT
 */
#define WSE_GBK32                              0x01
#define WSE_GBK48                              0x02
#define WSE_GBK64                              0x03

#define WSE_ASCII32                            0x01
#define WSE_ASCII48                            0x02
#define WSE_ASCII64                            0x03

/*
 Memory Mode
 */
#define WSE_MEM_NAND                           0
#define WSE_MEM_TF                             1

/*
 set screen rotation
 */
#define WSE_EPD_NORMAL                         0                                                        //screen normal
#define WSE_EPD_INVERSION                      1                                                        //screen inversion

struct wse_t;

int wse_open(struct wse_t *epd, const char *tty_dev);
int wse_open_fd(struct wse_t *epd, int fd);
int wse_close(struct wse_t *epd);

int wse_term_init(struct wse_t *epd);
int wse_enter_stopmode(struct wse_t *epd);

int wse_set_memory(struct wse_t *epd, unsigned char mode);
int wse_udpate(struct wse_t *epd);
int wse_screen_rotation(struct wse_t *epd, unsigned char mode);
int wse_load_font(struct wse_t *epd);
int wse_load_pic(struct wse_t *epd);


int wse_set_color(struct wse_t *epd, unsigned char color, unsigned char bkcolor);
int wse_set_en_font(struct wse_t *epd, unsigned char font);
int wse_set_ch_font(struct wse_t *epd, unsigned char font);

int wse_draw_pixel(struct wse_t *epd, int x0, int y0);
int wse_draw_line(struct wse_t *epd, int x0, int y0, int x1, int y1);
int wse_fill_rect(struct wse_t *epd, int x0, int y0, int x1, int y1);
int wse_draw_circle(struct wse_t *epd, int x0, int y0, int r);
int wse_fill_circle(struct wse_t *epd, int x0, int y0, int r);
int wse_draw_triangle(struct wse_t *epd, int x0, int y0, int x1, int y1, int x2, int y2);
int wse_fill_triangle(struct wse_t *epd, int x0, int y0, int x1, int y1, int x2, int y2);
int wse_clear(struct wse_t *epd);

int wse_disp_char(struct wse_t *epd, unsigned char ch, int x0, int y0);
int wse_disp_string(struct wse_t *epd, const void * p, int x0, int y0);

int wse_disp_bitmap(struct wse_t *epd, const void * p, int x0, int y0);


#endif /* WSEPAPER_H_ */
