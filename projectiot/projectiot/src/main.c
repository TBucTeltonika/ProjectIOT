#include "main.h"

volatile sig_atomic_t deamonize = 1;

void term_proc(int sigterm)
{
    deamonize = 0;
}

//CALL iwinfo devices
struct memstatus
{
    uint64_t memtotal;
    uint64_t memfree;
    uint64_t memshared;
    uint64_t membuffered;
};

/* Main program */
int main(int argc, char *argv[])
{

    TRACE_LOG(LOG_INFO, "Starting ProjectIOT");

    //Interrupt setup.
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    //Go into the main program loop.
    SendDataToIoTLoop();
    TRACE_LOG(LOG_INFO, "ProjectIOT is shutting down.");
    return 0;
}

int SendDataToIoTLoop()
{
    struct ubus_context *ctx;

    int rc = 0;

    //IOT setup.
    InitIoT();
    //UBUS SETUP
    ctx = ubus_connect(NULL);
    if (!ctx)
    {
        TRACE_LOG(LOG_CRIT, "Failed to connect to ubus");
        return -1;
    }

    while (deamonize)
    {
        char str[256];
        //Get the data.
        if (GetJSONMemData(ctx, str) == 0)
            SendData(str);

        sleep(10);
    }

    rc = FreeIoT();
    ubus_free(ctx);

    return rc;
}

int SendData(char *jsonstring)
{
    int rc = 0;
    rc = IoTPDevice_sendEvent(GetDevice(), "status", jsonstring, "json", QoS0, NULL);
    if (rc != 0)
    {
        TRACE_LOG(LOG_CRIT, "Problem sending the data to the server");
        return rc;
    }
    else
    {
        TRACE_LOG(LOG_INFO, "Data successfully sent to the server.");
    }
    return rc;
}

int GetJSONMemData(struct ubus_context *ctx, char *dest)
{
    int rc = 0;
    uint32_t id;
    struct memstatus memstat = {0, 0, 0, 0};
    //Use ubus to get system info data.
    if (ubus_lookup_id(ctx, "system", &id) || ubus_invoke(ctx, id, "info", NULL, board_cb, &memstat, 3000))
    {
        TRACE_LOG(LOG_CRIT, "cannot request memory info from procd");
        rc = -1;
        return rc;
    }

    sprintf(dest, "{\"d\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld }}",
            memstat.memtotal, memstat.memfree, memstat.memshared, memstat.membuffered);

    return rc;
}

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct memstatus *memstat = (struct memstatus *)req->priv;
    struct blob_attr *tb[__INFO_MAX];
    struct blob_attr *memory[__MEMORY_MAX];

    int rc = 0;

    blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[MEMORY_DATA])
    {
        TRACE_LOG(LOG_ALERT, "No memory data received");
        rc = -1;
        return;
    }

    blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
                  blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));

    memstat->memtotal = blobmsg_get_u64(memory[TOTAL_MEMORY]);
    memstat->memfree = blobmsg_get_u64(memory[FREE_MEMORY]);
    memstat->memshared = blobmsg_get_u64(memory[SHARED_MEMORY]);
    memstat->membuffered = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
}
