#ifndef UBUSCOMM_H
#define UBUSCOMM_H
#include <libubox/blobmsg_json.h>
#include <libubus.h>

//Ubus invoke structures.
enum
{
	TOTAL_MEMORY,
	FREE_MEMORY,
	SHARED_MEMORY,
	BUFFERED_MEMORY,
	__MEMORY_MAX,
};

enum
{
	MEMORY_DATA,
	__INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY] = {.name = "total", .type = BLOBMSG_TYPE_INT64},
	[FREE_MEMORY] = {.name = "free", .type = BLOBMSG_TYPE_INT64},
	[SHARED_MEMORY] = {.name = "shared", .type = BLOBMSG_TYPE_INT64},
	[BUFFERED_MEMORY] = {.name = "buffered", .type = BLOBMSG_TYPE_INT64},
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = {.name = "memory", .type = BLOBMSG_TYPE_TABLE},
};

#endif