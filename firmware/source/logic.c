#include "public.h"
#include "ir.h"
#include "logic.h"

static void _on_trigger(void)
{

}

static void _on_reload(void)
{

}

void logic_handle_key(void)
{
    int key;
    
    key = button_get();
    if (BUTTON_FIRE == key){
        _on_trigger();
        menu_display_main();
    } else if (BUTTON_ENTER == key){
        menu_display_menu();
    } else if (BUTTON_RELOAD == key){
        _on_reload();
    }
    
    return;
}

