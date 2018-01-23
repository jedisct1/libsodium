LIB_FUZZING_ENGINE ?= standalone_fuzz_taget_runner.o

# Values for CC, CFLAGS, CXX, CXXFLAGS are provided by OSS-Fuzz.
# Outside of OSS-Fuzz use the ones you prefer or rely on the default values.
# You may add extra compiler flags like this:
CXXFLAGS += -std=c++11

SODIUM_INCLUDE = ../include

clean:
	rm -fv *.a *.o *_fuzzer

all: secret_key_auth_fuzzer secretbox_easy_fuzzer

# Continuos integration system should run "make clean && make check-fuzz"
check-fuzz: all
	./secret_key_auth_fuzzer secret_key_auth_corpus/*
	./secretbox_easy_fuzzer secretbox_easy_corpus/*

# Fuzz target, links against $LIB_FUZZING_ENGINE, so that
# you may choose which fuzzing engine to use. For travis, this means that we use
# the standalone fuzzer, for google, we let it pick.
libsodium.a:
	cd .. && ./configure --enable-static && $(MAKE) clean && $(MAKE) -j$(nproc) all

secret_key_auth_fuzzer: secret_key_auth_fuzzer.cc libsodium.a standalone_fuzz_taget_runner.o
	${CXX} ${CXXFLAGS} ${SODIUM_INCLUDE} libsodium.a -l${LIB_FUZZING_ENGINE} -o $@

secretbox_easy_fuzzer: secretbox_easy_fuzzer.cc libsodium.a standalone_fuzz_taget_runner.o
	${CXX} ${CXXFLAGS} ${SODIUM_INCLUDE} libsodium.a -l${LIB_FUZZING_ENGINE} -o $@

standalone_runner.o: standalone_runner.cc
