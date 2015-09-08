#ifndef _FONT_H_
#define _FONT_H_

typedef enum{
    FONT_ASCII_6X8,
    FONT_ASCII_8X16,
    FONT_GB2312_16X16,
}font_type;

void font_getdot(font_type font, char *code, uint8_t *dot);

#endif

