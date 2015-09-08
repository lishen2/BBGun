#ifndef _CONFIG_H_
#define _CONFIG_H_

void config_init(void);

void config_read_profile(int idx, struct logic_core_data* data);
void config_save_profile(int idx, struct logic_core_data* data);

#endif

