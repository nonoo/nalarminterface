#ifndef CONFIG_H_
#define CONFIG_H_

void config_writeconfigfile(void);
flag_t config_init(char *configfilename);
void config_deinit(void);
char* config_get_runonalarm(void);

#endif
