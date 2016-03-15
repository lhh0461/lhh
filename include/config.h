#ifndef __CONFIG_H__
#define __CONFIG_H__

void config_init();
void __init_config(void);
void config_set_int(const char *key, int value);
int config_get_int(const char *key);
void config_set_string(const char *key, const char *value);
const char *config_get_string(const char *key);

#endif
