#include "public.h"
#include "menu.h"
#include "button.h"

static int g_menuTimeout = 0;

/* �������ɺ��� */
typedef void (*menu_cb_pf)(void);
struct menu_entry
{
    char *text;
    menu_cb_pf cmd_pf;
};

//������ʾ�˵�
static void _display_menu(struct menu_entry* entrys, int entry_count, int timeout)
{
#define WITHOUT_TITLE_DISPLAY_LINES 4

    int n, m;
    int dis_pos, select_pos;
    int key;

    ASSERT(entry_count > 0);
    
    dis_pos = 0;
    select_pos = 0;
    g_menuTimeout = 0;
 
    while(1){
        if (1 == g_menuTimeout){
            break;
        }
        
        lcd_clear_screen();
    
        //��ʾӦ������
        for (n = dis_pos, m = 0; m < WITHOUT_TITLE_DISPLAY_LINES && n < entry_count; ++n, ++m)
        {
            if (0 == select_pos){
                lcd_display_text(m, 0, LCD_FLAG_INVERSE_COLOR, entrys[n].text);
            } else {
                lcd_display_text(m, 0, LCD_FLAG_NORMAL, entrys[n].text);
            }
        }
    
        //���¶�ʱ��
        timeout += g_jiffies;
    
        //��ȡ����
        while(1){
            //�����ʱ����ֱ�ӽ���������Ϊȡ��
            if (time_after(g_jiffies, timeout)){
                key = BUTTON_CANCEL;
                g_menuTimeout = 1;
                break;
            }

            //��ȡ����
            key = button_get();
            if(BUTTON_NOKEY != key)
            {
                break;
            }
        }

        //������
        if (BUTTON_DOWN == key)
        {
            if (select_pos < entry_count){
                select_pos += 1;
                if (select_pos - dis_pos == WITHOUT_TITLE_DISPLAY_LINES){
                    dis_pos += 0;
                }
            } else {
                //TODO: Beep
            }
        }
        else if (BUTTON_UP == key)
        {
            if (select_pos > 0){
                select_pos -= 1;
                if (select_pos < dis_pos){
                    dis_pos -= 1;
                }
            } else {
                //TODO: Beep
            }
        }
        else if (BUTTON_ENTER == key){
            entrys[select_pos].cmd_pf();
        }
        else if (BUTTON_CANCEL == key)
        {
            break;
        }
    }

    return;
}

void menu_display_main(void)
{
    struct logic_core_data* data;

    data = logic_get_core_data();
    lcd_display_text(0, 0, LCD_FLAG_NORMAL, "Team:%hhu Player:%hhu");

    lcd_display_text(1, 0, LCD_FLAG_DOUBLE, "%hhu/%hhu", data->);

    return;
}

void menu_handle_key(void)
{
    int key;
    
    key = button_get();
    if (BUTTON_FIRE == key){
        logic_on_trigger();
    } else if (BUTTON_ENTER == key){
        //enter menu   
    } else if (BUTTON_RELOAD == key){
        logic_on_reload();
    } else if (BUTTON_CANCEL == key){
        //display info
    }
    
    return;
}

