#include "types.h"
#include "config.h"

#include <stdio.h>
#include <glib-2.0/glib.h>
#include <stdlib.h>

static GKeyFile *keyfile = NULL;
static GKeyFileFlags flags;
static char *currconfigfilename = NULL;

void config_writeconfigfile(void) {
	GError *error = NULL;
	FILE *f;
	gsize size;
	char *data;

	if (currconfigfilename == NULL) {
		fprintf(stderr, "config error: can't write config file, filename not set.\n");
		return;
	}

	data = g_key_file_to_data(keyfile, &size, &error);

	if (!error) {
		f = fopen(currconfigfilename, "w");
		if (f) {
			fwrite(data, 1, size, f);
			fclose(f);
		} else
			fprintf(stderr, "config error: config file (%s) is not writable.\n", currconfigfilename);
	} else
		fprintf(stderr, "config error: not saving config, can't build file data.\n");

	if (data)
		free(data);
}

flag_t config_init(char *configfilename) {
	GError *error = NULL;
	char *tmp = NULL;

	printf("config: initializing.\n");

	currconfigfilename = configfilename;

	if (keyfile != NULL)
		g_key_file_free(keyfile);

	keyfile = g_key_file_new();

	FILE *f = fopen(currconfigfilename, "r");
	if (f == NULL) {
		printf("config: config file doesn't exist, creating.\n");
		f = fopen(currconfigfilename, "w");
		fputs("[main]\n", f);
		fclose(f);
	}

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	if (!g_key_file_load_from_file(keyfile, currconfigfilename, flags, &error)) {
		fprintf(stderr, "config error: can't load config file (%s).\n", currconfigfilename);
		g_key_file_free(keyfile);
		keyfile = NULL;
		return 0;
	}

	// We read everything, a default value will be set for non-existent keys in the config file.
	tmp = config_get_runonalarm();
	free(tmp);
	config_get_eepromcounter_page();
	config_get_eepromcounter_address();

	config_writeconfigfile();
	return 1;
}

void config_deinit(void) {
	printf("config: deinitializing.\n");
	if (keyfile != NULL) {
		g_key_file_free(keyfile);
		keyfile = NULL;
	}
}

char* config_get_runonalarm(void) {
	GError *error = NULL;
	char *res = g_key_file_get_string(keyfile, "main", "runonalarm", &error);
	if (error) {
		fprintf(stderr, "config: no \"runonalarm\" setting found in config file, using default.\n");
		res = (char*)malloc(14);
		if (res) {
			snprintf(res, 14, "./naialarm.sh");
			g_key_file_set_string(keyfile, "main", "runonalarm", res);
		}
	}
	printf("config: read \"runonalarm\" setting: %s\n", res);
	return res;
}

char* config_get_runoneepromcounterincrease(void) {
	GError *error = NULL;
	char *res = g_key_file_get_string(keyfile, "main", "runoneepromcounterincrease", &error);
	if (error) {
		fprintf(stderr, "config: no \"runoneepromcounterincrease\" setting found in config file, using default.\n");
		res = (char*)malloc(18);
		if (res) {
			snprintf(res, 18, "./naieepromerr.sh");
			g_key_file_set_string(keyfile, "main", "runoneepromcounterincrease", res);
		}
	}
	printf("config: read \"runoneepromcounterincrease\" setting: %s\n", res);
	return res;
}

int config_get_eepromcounter_page(void) {
	GError *error = NULL;
	int val = g_key_file_get_integer(keyfile, "main", "eepromcounter-page", &error);
	if (error) {
		fprintf(stderr, "config: no \"eepromcounter-page\" setting found in config file, using default.\n");
		g_key_file_set_integer(keyfile, "main", "eepromcounter-page", 0);
		val = 0;
	}
	printf("config: read \"eepromcounter-page\" setting: %d\n", val);
	return val;
}

void config_set_eepromcounter_page(int page) {
	g_key_file_set_integer(keyfile, "main", "eepromcounter-page", page);
	config_writeconfigfile();
}

int config_get_eepromcounter_address(void) {
	GError *error = NULL;
	int val = g_key_file_get_integer(keyfile, "main", "eepromcounter-address", &error);
	if (error) {
		fprintf(stderr, "config: no \"eepromcounter-address\" setting found in config file, using default.\n");
		g_key_file_set_integer(keyfile, "main", "eepromcounter-address", 2);
		val = 2;
	}
	printf("config: read \"eepromcounter-address\" setting: %d\n", val);
	return val;
}

void config_set_eepromcounter_address(int address) {
	g_key_file_set_integer(keyfile, "main", "eepromcounter-address", address);
	config_writeconfigfile();
}
