#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -O2 -g -Wall -fpic #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
DYNLDFLAGS += -shared

LIB_OBJS = libconfparse.o
LIB_FILE = libconfparse.so

all: $(LIB_FILE)

$(LIB_FILE): $(LIB_OBJS)
	$(LD) $(DYNLDFLAGS) $(LIB_OBJS) -o $(LIB_FILE)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f $(LIB_OBJS) $(LIB_FILE)

install:


.PHONY: all install clean

# end of development makefile
