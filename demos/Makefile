
TARGETS = \
	auth \
	box \
	box_detached \
	generichash \
	generichash_stream \
	shorthash \
	sign

SODIUM_CFLAGS != pkg-config --cflags libsodium
SODIUM_LIBS != pkg-config --libs libsodium
CFLAGS += $(SODIUM_CFLAGS)
LDFLAGS += $(SODIUM_LIBS)

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

distclean: clean
