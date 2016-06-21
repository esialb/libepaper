/*********************************************************************************************************
*
* File                : epd.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* By                  : V
*
*                                  (c) Copyright 2005-2015, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/
#ifndef    EPD_H
  #define  EPD_H
	

#define    CMD_SIZE                           512	

/*
frame format
*/
#define    FRAME_B                            0xA5
#define    FRAME_E0                           0xCC
#define    FRAME_E1                           0x33
#define    FRAME_E2                           0xC3
#define    FRAME_E3                           0x3C


/*
color define
*/
#define    WHITE                              0x03
#define    GRAY                               0x02
#define    DARK_GRAY                          0x01
#define    BLACK                              0x00

/*
command define
*/
#define    CMD_HANDSHAKE                      0x00                                                     //handshake
#define    CMD_SET_BAUD                       0x01                                                     //set baud
#define    CMD_READ_BAUD                      0x02                                                     //read baud
#define    CMD_MEMORYMODE                     0x07                                                     //set memory mode
#define    CMD_STOPMODE                       0x08                                                     //enter stop mode 
#define    CMD_UPDATE                         0x0A                                                     //update
#define    CMD_SCREEN_ROTATION                0x0D                                                     //set screen rotation
#define    CMD_LOAD_FONT                      0x0E                                                     //load font
#define    CMD_LOAD_PIC                       0x0F                                                     //load picture

#define    CMD_SET_COLOR                      0x10                                                     //set color
#define    CMD_SET_EN_FONT                    0x1E                                                     //set english font
#define    CMD_SET_CH_FONT                    0x1F                                                     //set chinese font

#define    CMD_DRAW_PIXEL                     0x20                                                     //set pixel
#define    CMD_DRAW_LINE                      0x22                                                     //draw line
#define    CMD_FILL_RECT                      0x24                                                     //fill rectangle
#define    CMD_DRAW_CIRCLE                    0x26                                                     //draw circle
#define    CMD_FILL_CIRCLE                    0x27                                                     //fill circle
#define    CMD_DRAW_TRIANGLE                  0x28                                                     //draw triangle
#define    CMD_FILL_TRIANGLE                  0x29                                                     //fill triangle
#define    CMD_CLEAR                          0x2E                                                     //clear screen use back color

#define    CMD_DRAW_STRING                    0x30                                                     //draw string

#define    CMD_DRAW_BITMAP                    0x70                                                     //draw bitmap


/*
FONT
*/
#define    GBK32                              0x01
#define    GBK48                              0x02
#define    GBK64                              0x03
	
#define    ASCII32                            0x01
#define    ASCII48                            0x02
#define    ASCII64                            0x03



/*
Memory Mode
*/
#define    MEM_NAND                           0
#define    MEM_TF                             1

/*
set screen rotation
*/
#define    EPD_NORMAL                         0                                                        //screen normal
#define    EPD_INVERSION                      1                                                        //screen inversion      

struct epd_t {
	int tty_fd;
	unsigned char cmd[CMD_SIZE];
};

int epd_open(struct epd_t *epd, const char *tty_dev);
int epd_open_fd(struct epd_t *epd, int tty_fd);

int epd_init(struct epd_t *epd);
int epd_enter_stopmode(struct epd_t *epd);

int epd_set_memory(struct epd_t *epd, unsigned char mode);
int epd_udpate(struct epd_t *epd);
int epd_screen_rotation(struct epd_t *epd, unsigned char mode);
int epd_load_font(struct epd_t *epd);
int epd_load_pic(struct epd_t *epd);


int epd_set_color(struct epd_t *epd, unsigned char color, unsigned char bkcolor);
int epd_set_en_font(struct epd_t *epd, unsigned char font);
int epd_set_ch_font(struct epd_t *epd, unsigned char font);

int epd_draw_pixel(struct epd_t *epd, int x0, int y0);
int epd_draw_line(struct epd_t *epd, int x0, int y0, int x1, int y1);
int epd_fill_rect(struct epd_t *epd, int x0, int y0, int x1, int y1);
int epd_draw_circle(struct epd_t *epd, int x0, int y0, int r);
int epd_fill_circle(struct epd_t *epd, int x0, int y0, int r);
int epd_draw_triangle(struct epd_t *epd, int x0, int y0, int x1, int y1, int x2, int y2);
int epd_fill_triangle(struct epd_t *epd, int x0, int y0, int x1, int y1, int x2, int y2);
int epd_clear(struct epd_t *epd);

int epd_disp_char(struct epd_t *epd, unsigned char ch, int x0, int y0);
int epd_disp_string(struct epd_t *epd, const void * p, int x0, int y0);

int epd_disp_bitmap(struct epd_t *epd, const void * p, int x0, int y0);


	
#endif




























