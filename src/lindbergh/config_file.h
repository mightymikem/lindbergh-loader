#include <stdlib.h>

#define CONFIG_MAX_LINE_LENGTH 2048
#define MAX_ARGUMENT_AMOUNT 4
#define MAX_CONFIG_LINES 100

typedef enum
{
    CONFIG_STATUS_SUCCESS,
    CONFIG_STATUS_FAILURE,
    CONFIG_STATUS_COMMAND_NOT_FOUND,
    CONFIG_STATUS_NOT_ENOUGH_ARGUMENTS,
    CONFIG_STATUS_CANNOT_OPEN_FILE
} ConfigStatus;

typedef struct {
    char command[CONFIG_MAX_LINE_LENGTH];
    char argument[MAX_ARGUMENT_AMOUNT][CONFIG_MAX_LINE_LENGTH];
    int argumentIndex;
} ConfigPair;

typedef struct
{
    FILE *file;
    char* path;
    int configPairIndex;
    ConfigPair configPairs[MAX_CONFIG_LINES];
} Config;

char* getConfigErrorString(ConfigStatus status);


ConfigStatus processConfig(Config *config, char *path);

ConfigStatus getConfigString(Config *config, char* key, char* value);
ConfigStatus getConfigArgumentString(Config *config, char* key, int n, char* value);
ConfigStatus getConfigInt(Config *config, char* key, int* value);
ConfigStatus getConfigArgumentInt(Config *config, char* key, int n, int* value);

ConfigStatus writeConfigString(Config *config, char* key, char* value);
ConfigStatus writeConfigInt(Config *config, char* key, int value);

ConfigStatus deleteConfig(Config *config, char* key);


