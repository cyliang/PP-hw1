# Author: Chih-Yung Liang
# Available options are the followings:
#
# target: the file name of the output executable, default is hw1_pthread
# THREAD: explicitly set the number of threads in compilation time, ignore to dynamically detect in run time.
#
# Usage: make [target=<output_name>] [THREAD=<number_of_thread>] [develop]
#
# Set the build target to "develop" to include debug information and use the pseudo random result
#


target = hw1_pthread

INCLUDE = -lpthread
CFLAGS := $(INCLUDE) -Wno-pointer-to-int-cast

ifdef THREAD
	CFLAGS += -DTHREAD_COUNT=$(THREAD)
endif

all: $(target)

develop: CFLAGS += -DPSEUDO_RAND -g
develop: $(target)

$(target): pi.c
	cc -o $(target) $(CFLAGS) pi.c

.PHONY: clean
clean:
	@rm -f $(target)
	@echo "Cleaned."

