#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <uci.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
//#include <uci.h>

#include "ubuscomm.h"
#include "iot.h"
#include "logger.h"
void term_proc(int sigterm);
static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);

  enum {
          MEM_TOTAL,
          MEM_FREE,
          MEM_SHARED,
          MEM_BUFFERED,
          __MEM_MAX
  };
  
  static const struct blobmsg_policy mem_policy[] = {
          [MEM_FREE] = { .name = "memfree", .type = BLOBMSG_TYPE_INT64 },
          [MEM_TOTAL] = { .name = "memtotal", .type = BLOBMSG_TYPE_INT64 },
          [MEM_SHARED] = { .name = "memshared", .type = BLOBMSG_TYPE_INT64 },
          [MEM_BUFFERED] = { .name = "membuffered", .type = BLOBMSG_TYPE_INT64 },
  };


//UBUS Object stuff:

