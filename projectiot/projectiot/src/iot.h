#ifndef IOT_H
#define IOT_H
#include <iotp_device.h>
#include <uci.h>
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include "logger.h"


#define UCI_CONFIG_FILE "projectiot"
struct iotconfig{
    char* orgid;
    char* typeid;
    char* deviceid;
    char* authtoken;
};
// youer declarations (and certain types of definitions) here
 
IoTPConfig* GetConfig();
IoTPDevice* GetDevice();
int InitIoT();
int FreeIoT();
static int SetUpConfig(IoTPConfig **config);
static int SetUpDevice(IoTPConfig *config, IoTPDevice** device);
void logCallback (int level, char * message);
static void MQTTTraceCallback (int level, char * message);
static int ReadConfig(struct iotconfig* cfg);
#endif
