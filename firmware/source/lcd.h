#ifndef _LCD_H_
#define _LCD_H_

#define LCD_FLAG_NORMAL         0x00  //����
#define LCD_FLAG_INVERSE_COLOR  0x01  //��ɫ
#define LCD_FLAG_DOUBLE         0x01  //���屶�߱���

void lcd_init(void);

void lcd_display_text(int row, int column, uint8_t flag, char* fmt, ...);
void lcd_clear_line(int start, int end);
void lcd_clear_screen(void);

void lcd_shutdown(void);
void lcd_lightup(void);

#endif

