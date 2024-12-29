#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_TEMP_FILE_NAME ".lindbergh.conf.tmp"

#include "config_file.h"

char *getConfigErrorString(ConfigStatus status)
{
    switch (status)
    {
    case CONFIG_STATUS_COMMAND_NOT_FOUND:
        return "The key you looked for could not be found";
    case CONFIG_STATUS_FAILURE:
        return "There was a generic failure";
    case CONFIG_STATUS_SUCCESS:
        return "The operation was successful";
    case CONFIG_STATUS_NOT_ENOUGH_ARGUMENTS:
        return "The config file did not have enough arguments to complete your request";
    case CONFIG_STATUS_CANNOT_OPEN_FILE:
        return "The config file could not be opened";
    default:
        return "This config status enum doesn't have an error string associated.";
    }
}

static char *getNextToken(char *buffer, char *seperator, char **saveptr)
{
    char *token = strtok_r(buffer, seperator, saveptr);
    if (token == NULL)
        return NULL;

    for (int i = 0; i < (int)strlen(token); i++)
    {
        if ((token[i] == '\n') || (token[i] == '\r'))
        {
            token[i] = 0;
        }
    }
    return token;
}

ConfigStatus processConfig(Config *config, char *path)
{
    config->path = path;
    config->file = fopen(config->path, "r");
    config->configPairIndex = 0;

    if (config->file == NULL)
    {
        return CONFIG_STATUS_CANNOT_OPEN_FILE;
    }

    char buffer[CONFIG_MAX_LINE_LENGTH];
    char *saveptr = NULL;

    while (fgets(buffer, CONFIG_MAX_LINE_LENGTH, config->file))
    {
        // Ignore any lines that start with a comment
        if (buffer[0] == '#' || buffer[0] == 0 || buffer[0] == ' ' || buffer[0] == '\r' || buffer[0] == '\n')
            continue;

        ConfigPair *configPair = &config->configPairs[config->configPairIndex];

        strcpy(configPair->command, getNextToken(buffer, " ", &saveptr));

        configPair->argumentIndex = 0;
        char *argument = getNextToken(NULL, " ", &saveptr);
        while (argument != NULL && argument[0] != 0)
        {
            strcpy(configPair->argument[configPair->argumentIndex++], argument);
            argument = getNextToken(NULL, " ", &saveptr);
        }

        config->configPairIndex++;
    }

    fclose(config->file);

    return CONFIG_STATUS_SUCCESS;
}

ConfigStatus getConfigArgumentString(Config *config, char *key, int n, char *value)
{
    for (int i = 0; i < config->configPairIndex; i++)
    {
        if (strcmp(config->configPairs[i].command, key) == 0)
        {
            if (n >= config->configPairs[i].argumentIndex)
                return CONFIG_STATUS_NOT_ENOUGH_ARGUMENTS;

            strcpy(value, config->configPairs[i].argument[n]);

            return CONFIG_STATUS_SUCCESS;
        }
    }

    return CONFIG_STATUS_COMMAND_NOT_FOUND;
}

ConfigStatus getConfigArgumentInt(Config *config, char *key, int n, int *value)
{
    for (int i = 0; i < config->configPairIndex; i++)
    {
        if (strcmp(config->configPairs[i].command, key) == 0)
        {
            if (n >= config->configPairs[i].argumentIndex)
                return CONFIG_STATUS_NOT_ENOUGH_ARGUMENTS;

            *value = atoi(config->configPairs[i].argument[i]);

            return CONFIG_STATUS_SUCCESS;
        }
    }

    return CONFIG_STATUS_COMMAND_NOT_FOUND;
}

ConfigStatus getConfigString(Config *config, char *key, char *value)
{
    return getConfigArgumentString(config, key, 0, value);
}

ConfigStatus getConfigInt(Config *config, char *key, int *value)
{
    return getConfigArgumentInt(config, key, 0, value);
}

ConfigStatus writeConfigString(Config *config, char *key, char *value)
{
    config->file = fopen(config->path, "r");

    if (config->file == NULL)
    {
        return CONFIG_STATUS_FAILURE;
    }

    FILE *tempFile = fopen(CONFIG_TEMP_FILE_NAME, "w");
    if (tempFile == NULL)
    {
        return CONFIG_STATUS_FAILURE;
    }

    char buffer[CONFIG_MAX_LINE_LENGTH];
    char *saveptr = NULL;
    char inspectionBuffer[CONFIG_MAX_LINE_LENGTH];

    char writeBuffer[CONFIG_MAX_LINE_LENGTH];
    strcpy(writeBuffer, key);
    strcat(writeBuffer, " ");
    strcat(writeBuffer, value);
    strcat(writeBuffer, "\n");

    int lineWasReplaced = 0;

    while (fgets(buffer, CONFIG_MAX_LINE_LENGTH, config->file))
    {
        strcpy(inspectionBuffer, buffer);
        char *token = getNextToken(inspectionBuffer, " ", &saveptr);

        if (strcmp(token, key) == 0)
        {
            lineWasReplaced = 1;

            // If there is no value just delete the line
            if (strcmp(value, "") == 0)
                continue;

            fputs(writeBuffer, tempFile);
            continue;
        }

        fputs(buffer, tempFile);
    }

    // If the file doesn't end with a new line, write one.
    if (buffer[strlen(buffer) - 1] != '\n')
        fputs("\n", tempFile);

    // If no line was replaced add it in at the end
    if (!lineWasReplaced)
        fputs(writeBuffer, tempFile);

    fclose(config->file);
    fclose(tempFile);

    // Now re-write the old file with the new temp file name
    remove(config->path);
    rename(CONFIG_TEMP_FILE_NAME, config->path);

    return processConfig(config, config->path);
}

ConfigStatus writeConfigInt(Config *config, char *key, int value)
{
    char stringValue[CONFIG_MAX_LINE_LENGTH];
    sprintf(stringValue, "%d", value);
    return writeConfigString(config, key, stringValue);
}

ConfigStatus deleteConfig(Config *config, char *key)
{
    return writeConfigString(config, key, "");
}