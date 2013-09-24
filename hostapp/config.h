#ifndef CONFIG_H_
#define CONFIG_H_

void config_writeconfigfile(void);
flag_t config_init(char *configfilename);
void config_deinit(void);
char* config_get_runonalarm(void);
char* config_get_runoneepromcounterincrease(void);
int config_get_eepromcounter_page(void);
void config_set_eepromcounter_page(int page);
int config_get_eepromcounter_address(void);
void config_set_eepromcounter_address(int address);

#endif
