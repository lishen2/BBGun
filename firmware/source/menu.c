#include "public.h"
#include "menu.h"
#include "button.h"

static int g_menuTimeout = 0;

/* 界面生成函数 */
typedef void (*menu_cb_pf)(uint32_t);
struct menu_entry
{
    char *text;
    uint32_t data;
    menu_cb_pf cmd_pf;
};

//根据显示菜单
static void _display_menu(struct menu_entry* entries, int entry_count, int timeout)
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
    
        //显示应用名称
        for (n = dis_pos, m = 0; m < WITHOUT_TITLE_DISPLAY_LINES && n < entry_count; ++n, ++m)
        {
            if (0 == select_pos){
                lcd_display_text(m, 0, LCD_FLAG_INVERSE_COLOR, entries[n].text);
            } else {
                lcd_display_text(m, 0, LCD_FLAG_NORMAL, entries[n].text);
            }
        }
    
        //更新定时器
        timeout += g_jiffies;
    
        //读取按键
        while(1){
            //如果超时，则直接将按键设置为取消
            if (time_after(g_jiffies, timeout)){
                key = BUTTON_CANCEL;
                g_menuTimeout = 1;
                break;
            }

            //读取按键
            key = button_get();
            if(BUTTON_NOKEY != key)
            {
                break;
            }
        }

        //处理按键
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
            if (NULL != entries[select_pos].cmd_pf){
                entries[select_pos].cmd_pf(entries[select_pos].data);
            }
        }
        else if (BUTTON_CANCEL == key)
        {
            break;
        }
    }

    return;
}

static void _display_status(uint32_t unused)
{
    
}

static void _menu_score(uint32_t unused)
{

}

static void _handle_game_start(uint32_t unused)
{
    int key;
    
    lcd_clear_screen();
    lcd_display_text(0, 0, LCD_FLAG_NORMAL, "Trigger to send");
    lcd_display_text(1, 0, LCD_FLAG_NORMAL, "  START GAME");
    lcd_display_text(3, 0, LCD_FLAG_NORMAL, "Cancel to EXIT");

    key = BUTTON_NOKEY;
    while(BUTTON_CANCEL != key){
        key = button_get();
        if (BUTTON_FIRE == key){
            ir_send_message(IR_BYTE1_COMMAND, IR_BYTE2_COMMAND_START_GAME, NULL, 0);
        }
    }

    return;
}

static void _handle_game_end(uint32_t unused)
{
    int key;
    
    lcd_clear_screen();
    lcd_display_text(0, 0, LCD_FLAG_NORMAL, "Trigger to send");
    lcd_display_text(1, 0, LCD_FLAG_NORMAL, "   END GAME");
    lcd_display_text(3, 0, LCD_FLAG_NORMAL, "Cancel to EXIT");

    key = BUTTON_NOKEY;
    while(BUTTON_CANCEL != key){
        key = button_get();
        if (BUTTON_FIRE == key){
            ir_send_message(IR_BYTE1_COMMAND, IR_BYTE2_COMMAND_END_GAME, NULL, 0);
        }
    }

    return;
}

static void _handle_game_new(uint32_t unused)
{
    int key;
    
    lcd_clear_screen();
    lcd_display_text(0, 0, LCD_FLAG_NORMAL, "Trigger to send");
    lcd_display_text(1, 0, LCD_FLAG_NORMAL, "  NEW GAME");
    lcd_display_text(3, 0, LCD_FLAG_NORMAL, "Cancel to EXIT");

    key = BUTTON_NOKEY;
    while(BUTTON_CANCEL != key){
        key = button_get();
        if (BUTTON_FIRE == key){
            ir_send_message(IR_BYTE1_COMMAND, IR_BYTE2_COMMAND_NEW_GAME_IMMED, NULL, 0);
        }
    }

    return;    
}

static void _menu_game(uint32_t unused)
{
    struct menu_entry entries[] = {
        {
            "Start",
            0,
            _handle_game_start,
        },
        {
            "End",
            0,
            _handle_game_end,
        },        
        {
            "New",
            0,
            _handle_game_new,
        },        
    };

    _display_menu(entries, sizeof(entries)/sizeof(struct menu_entry), 30);

    return;
}

static void _menu_admin(uint32_t unused)
{
    struct menu_entry entries[] = {
        {
            "Profile",
            0,
            NULL,
        },
        {
            "Game",
            0,
            _menu_score,
        },
    };

    _display_menu(entries, sizeof(entries)/sizeof(struct menu_entry), 30);

    return;
}

static void _display_version(unint32_t unused)
{

}

void menu_display_menu(void)
{
    struct menu_entry entries[] = {
        {
            "Status",
            0,
            _display_status,
        },
        {
            "Score",
            0,
            _menu_score,
        },
        {
            "Admin"
            0,
            _menu_admin,
        },
        {
            "Version"
            0,
            _display_version,            
        }
    };

    _display_menu(entries, sizeof(entries)/sizeof(struct menu_entry), 30);

    return;
}

void menu_display_main(void)
{
    struct logic_core_data* data;
    
    data = logic_get_core_data();
    lcd_clear_screen();    
    lcd_display_text(0, 0, LCD_FLAG_DOUBLE, "A:%hhu", data->cur_clip);
    lcd_display_text(1, 8, LCD_FLAG_NORMAL, "/%hhu",  data->clip_size * data->clips);
    lcd_display_text(2, 0, LCD_FLAG_DOUBLE, "H:%hhu", data->health);
    lcd_display_text(3, 8, LCD_FLAG_NORMAL, "/%hhu",  data->armor);
    
    return;
}

