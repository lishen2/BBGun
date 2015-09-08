#ifndef _LCD_H_
#define _LCD_H_

#define LCD_FLAG_NORMAL         0x00
#define LCD_FLAG_INVERSE_COLOR  0x01
#define LCD_FLAG_DOUBLE         0x01  //double width and height

void lcd_init(void);

//display text at specific row and column
void lcd_display_text(int row, int column, uint8_t flag, char* fmt, ...);

//clear screen by lines
void lcd_clear_line(int start, int end);

//clear whole screen
void lcd_clear_screen(void);

//draw a image at specific location
void lcd_draw_image(int x, int y, uint8_t* img, int width, int height);

//shutdown and lightup LCD
void lcd_shutdown(void);
void lcd_lightup(void);

#endif

