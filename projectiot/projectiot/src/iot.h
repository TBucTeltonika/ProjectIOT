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
struct iotconfig
{
    char *orgid;
    char *typeid;
    char *deviceid;
    char *authtoken;
};

/*Get IoTPConfig object. IoTPConfig object is created when the function is first called.
Returns: IoTPConfig*/
IoTPConfig *GetConfig();

/*Get IoTPDevice object. Device is created when the function is first called.
Will initialize IoTPConfig object if its not yet initialized.
Returns: IoTPDevice*/
IoTPDevice *GetDevice();

/*Initializes logging and IoTPConfig and IoTPDevice objects.
Returns: 0 - OK, !=0 - Error code.*/
int InitIoT();

/*Breaks the connection and frees IoTPDevice and IoTPConfig objects.
Returns: 0 - OK, !=0 - Error code.*/
int FreeIoT();

/*Internal function that reads config file on the device and initializes a IoTPConfig object
ARGUMENTS: IoTPConfig **config - config file pointer to write the value of the newly created object.
Returns: 0 - OK, !=0 - Error code.*/
static int SetUpConfig(IoTPConfig **config);

/*Internal function that creates a device and connects it to the IoT using passed config object.
ARGUMENTS: IoTPConfig *config - config object to use, IoTPDevice** device - where to write the new object address.
Returns: 0 - OK, !=0 - Error code.*/
static int SetUpDevice(IoTPConfig *config, IoTPDevice **device);

/*Internal function to handle MQTT Trace callback. */
static void MQTTTraceCallback(int level, char *message);

/*Intenal function to read the configuration file on the router.
ARGUMENTS: struct iotconfig* cfg - config structure to write the values to.
Returns: 0 - OK, !=0 - Error code.*/
static int ReadConfig(struct iotconfig *cfg);
#endif
