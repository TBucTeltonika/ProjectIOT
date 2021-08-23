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

    TRACE_LOG(LOG_INFO,"Starting ProjectIOT");
    int rc;

    struct ubus_context *ctx;
    uint32_t id;

    //IOT setupas.
    InitIoT();

    //Interrupt setupas.
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    //UBUS SETUPAS
    ctx = ubus_connect(NULL);
    if (!ctx)
    {
        TRACE_LOG(LOG_CRIT,"Failed to connect to ubus");
        return -1;
    }

    struct memstatus memstat = {0, 0, 0, 0};
    while (deamonize)
    {
        //Use ubus to get system info data.
        if (ubus_lookup_id(ctx, "system", &id) || ubus_invoke(ctx, id, "info", NULL, board_cb, &memstat, 3000))
        {
            TRACE_LOG(LOG_CRIT,"cannot request memory info from procd");
            rc = -1;
        }
        sleep(10);

        //Send the data to the IoT.
        char str[256];
        sprintf(str, "{\"d\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld }}",
                memstat.memtotal, memstat.memfree, memstat.memshared, memstat.membuffered);

        //Printing the data to the log.
        //TRACE_LOG(LOG_INFO, str);
        rc = IoTPDevice_sendEvent(GetDevice(), "status", str, "json", QoS0, NULL);
        if (rc != 0)
        {
            TRACE_LOG(LOG_CRIT, "Problem sending the data to the server");
            return rc;
        }
        else
        {
            TRACE_LOG(LOG_INFO, "Data successfully sent to the server.");
        }
    }
    TRACE_LOG(LOG_INFO, "Publish event cycle is complete.");

    rc = FreeIoT();
    ubus_free(ctx);
    TRACE_LOG(LOG_INFO, "ProjectIOT is shutting down.");
    return 0;
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
