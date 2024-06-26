#
# Copyright (c) LSE
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY LSE AS IS AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL LSE BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
include ../config.mk

TARGET	= k
SRCS    = $(shell find . -name '*.c' -o -name '*.S')
TMP     = $(SRCS:.S=.S.o)
OBJS    = $(TMP:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): CPPFLAGS += -MMD -Iinclude -I ../libs/libc/include/ -I./
$(TARGET): CFLAGS += $(K_EXTRA_CFLAGS)
$(TARGET): LDFLAGS += -Wl,-Tk.lds
$(TARGET): LDLIBS = -L../libs/libc -lc
$(TARGET): $(OBJS)

%.S.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	$(INSTALL) $(TARGET) $(INSTALL_ROOT)/$(TARGET)

clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
