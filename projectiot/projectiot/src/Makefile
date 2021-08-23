BIN:=projectiot
LDFLAGS:=$(LDFLAGS) -liotp-as-device -lblobmsg_json -lubox -lubus -luci

.PHONY: all clean

all: $(BIN)

$(BIN): main.c iot.c logger.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)