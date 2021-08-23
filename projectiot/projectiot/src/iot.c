#include "iot.h"

IoTPConfig *GetConfig()
{
    //Create a static Config object.
    static IoTPConfig *config = NULL;
    //Initialize the object if its not initialized yet.
    if (config == NULL)
    {
        int rc = SetUpConfig(&config);
        if (rc != 0)
        {
            TRACE_LOG(LOG_CRIT, "Error setting up IoTPConfig rc:%d.", rc);
            exit(1);
        }
    }

    return config;
}

IoTPDevice *GetDevice()
{
    //Create a static Device object.
    static IoTPDevice *device = NULL;
    //Initialize the object if its not initialized yet.
    if (device == NULL)
    {
        int rc = SetUpDevice(GetConfig(), &device);
        if (rc != 0)
        {
            TRACE_LOG(LOG_CRIT, "Error setting up IoTPDevice rc:%d.", rc);
            exit(1);
        }
    }

    return device;
}

int InitIoT()
{
    int rc = 0;

    /* Set IoTP Client log handler */
    //Lets just let this keep printing to stderr.
    rc = IoTPConfig_setLogHandler(IoTPLog_FilePointer, stderr);
    if (rc != 0)
    {
        TRACE_LOG(LOG_CRIT, "WARN: Failed to set IoTP Client log handler: rc=%d", rc);
        exit(1);
    }
    //Setup Config file.
    GetConfig();

    //Set up Device.
    GetDevice();

    return rc;
}

#define ASSERT_SETPROPERTY(cfg, propname, value)                                         \
    if (rc = IoTPConfig_setProperty(cfg, propname, value))                               \
    {                                                                                    \
        TRACE_LOG(LOG_CRIT, "ERROR: Problem setting property: %s: rc=%d", propname, rc); \
        exit(1);                                                                         \
    }

static int SetUpConfig(IoTPConfig **config)
{
    int rc = 0;

    //Create config object.
    rc = IoTPConfig_create(config, NULL);
    if (rc == 0)
    {
        //Read configuration file on the device.
        struct iotconfig cfg = {0, 0, 0, 0};
        rc = ReadConfig(&cfg);
        if (rc)
        {
            TRACE_LOG(LOG_CRIT, "ERROR: Failed to read configuration : rc=%d", rc);
            exit(1);
        }
        //Set values from the configuration file to the object.
        ASSERT_SETPROPERTY(*config, "identity.orgId", cfg.orgid);
        ASSERT_SETPROPERTY(*config, "identity.typeId", cfg.typeid);
        ASSERT_SETPROPERTY(*config, "identity.deviceId", cfg.deviceid);
        ASSERT_SETPROPERTY(*config, "auth.token", cfg.authtoken);
    }
    else
        exit(1);

    return rc;
}

static int SetUpDevice(IoTPConfig *config, IoTPDevice **device)
{
    int rc = 0;
    /* Create IoTPDevice object */
    rc = IoTPDevice_create(device, config);
    if (rc != 0)
    {
        TRACE_LOG(LOG_CRIT, "ERROR: Failed to configure IoTP device: rc=%d", rc);
        exit(1);
    }

    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(*device, &MQTTTraceCallback);
    if (rc != 0)
    {
        TRACE_LOG(LOG_CRIT, "ERROR: Failed to configure IoTP device: rc=%d", rc);
        exit(1);
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(*device);
    if (rc != 0)
    {
        TRACE_LOG(LOG_CRIT, "ERROR: Failed to connect to Watson IoT Platform: rc=%d", rc);
        TRACE_LOG(LOG_CRIT, "ERROR: Returned error reason: %s", IOTPRC_toString(rc));
        exit(1);
    }
    return rc;
}

void MQTTTraceCallback(int level, char *message)
{
    if (level > 0)
        fprintf(stdout, "%s\n", message ? message : "NULL");
    fflush(stdout);
}

int FreeIoT()
{
    int rc = 0;

    /* Disconnect device */
    rc = IoTPDevice_disconnect(GetDevice());
    if (rc != IOTPRC_SUCCESS)
    {
        TRACE_LOG(LOG_CRIT, "ERROR: Failed to disconnect from  Watson IoT Platform: rc=%d", rc);
        exit(1);
    }

    /* Destroy client */
    IoTPDevice_destroy(GetDevice());

    /* Clear configuration */
    IoTPConfig_clear(GetConfig());

    return rc;
}

/* Macro without error checking */
//#define READVALUE(dest,name) dest = strdup(uci_lookup_option_string(ctx, s, name));

/* Macro with error checking | Two calls to the same function should get optimized. */
#define READVALUE(dest, name)                                                      \
    if (uci_lookup_option_string(ctx, s, name) == NULL)                            \
    {                                                                              \
        TRACE_LOG(LOG_CRIT, "ERROR: ReadConfig failed to lookup value :%s", name); \
        exit(1);                                                                   \
    }                                                                              \
    else                                                                           \
        dest = strdup(uci_lookup_option_string(ctx, s, name));

static int ReadConfig(struct iotconfig *cfg)
{
    int rc = 0;
    struct uci_context *ctx = NULL;
    struct uci_package *pkg = NULL;
    struct uci_element *e;
    ctx = uci_alloc_context(); //apply for a UCI context.
    if (UCI_OK != uci_load(ctx, UCI_CONFIG_FILE, &pkg))
        goto cleanup; //If the UCI file fails to open, skip to the end to clean up the UCI context.
    /* Traverse every section of UCI */
    uci_foreach_element(&pkg->sections, e)
    {
        struct uci_section *s = uci_to_section(e);

        READVALUE(cfg->orgid, "auth_orgid");
        READVALUE(cfg->typeid, "auth_typeid");
        READVALUE(cfg->deviceid, "auth_deviceid");
        READVALUE(cfg->authtoken, "auth_authtoken");
    }
    uci_unload(ctx, pkg); //release pkg
    return rc;
cleanup:
    uci_free_context(ctx);
    ctx = NULL;
    return rc;
}
