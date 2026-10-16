# libsns_sc1035_universal.so — universal SC1035 sensor library
# 720p/960p (window from VI introspection), fps 5..30 via VTS formula.
# See README.md for details.

CC      ?= cc
DEFS    := -Dhi3518 -DHICHIP=0x35180100 -DISP_V1
CFLAGS  := -Wall -Os -fPIC $(DEFS) -Iinclude
TARGET  := libsns_sc1035_universal.so
SRCS    := src/sc1035_cmos.c src/sc1035_sensor_ctl.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -shared -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
