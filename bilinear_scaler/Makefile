BATCH_BUILD ?= 0

#-- for local build, set BATCH_BUILD=0 to configurate toolchain
# ifeq ($(BATCH_BUILD), 0)
# BATCH_DIR = ../..
# DIR_IPCAM_SRC = $(BATCH_DIR)/../ipcam

# CROSS_COMPILE = $(BATCH_DIR)/../../rtl819x-sdk-v3.4.11b/rtl819x/toolchain/msdk-4.4.7-mips-EL-3.10-u0.9.33-m32t-140827/bin/mips-linux-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ls
AR = $(CROSS_COMPILE)ar
# endif

#-----------------------------------------
# Here is the modules of the library
#-----------------------------------------

#-- Bi-Linear Scaler
C_SRCS = fast_blscaler.c blscaler_float.c bin2bmp.c unit_test.c
CPP_SRCS = common.cpp
BL_OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)
BL_TEST = bl_test

# MAIN_C_SRCS = main.c bin2bmp.c blscaler.c
# MAIN_C_OBJS = $(MAIN_C_SRCS:.c=.o)

# MAIN_CPP_SRCS = common.cpp
# MAIN_CPP_OBJS = $(MAIN_CPP_SRCS:.cpp=.o)

# OBJS = $(MAIN_C_OBJS) $(MAIN_CPP_OBJS)

DEPS = $(BL_OBJS:.o=.d)

MY_CFLAGS 	:= -O2 -Wall
MY_CFLAGS	+= -Wno-maybe-uninitialized -Wno-sign-compare
MY_CFLAGS	+= -Wno-format -Wno-strict-aliasing
MY_CFLAGS	+= -Wno-write-string -Wno-unused-result
MY_CFLAGS	+= -Wno-unused-variable -Wno-unused-but-set-variable

INCLUDE_CFLAGS	=

CFLAGS    += $(MY_CFLAG) $(INCLUDE_CFLAGS)

COMMON_DIR		= ../COMMON
COMMON_INC_DIR	= $(COMMON_DIR)/include
OUTPUT_LIB_DIR	= $(COMMON_DIR)/lib

#=== Implicit rules
%.o:%.c
	@(echo "")
	@(echo "[C] Compiling $< ...")
	@$(CC) $(CFLAGS) -std=c99 -c $< -o $@

%.o:%.cpp
	@(echo "")
	@(echo "[CXX] Compiling $< ...")
	@$(CXX) $(CFLAGS) -c $< -o $@


#=== Project programs
.PHONY: all clean exec_test

all: exec_test
exec_test: $(BL_TEST)

$(BL_TEST): $(BL_OBJS)
	$(info *********************************)
	$(info ** Building $@ ...)
	$(info *********************************)
	$(CXX) $^ -o $@


clean:
	$(RM) -f $(DEPS) $(BL_OBJS) $(BL_TEST)


sinclude $(DEPS)
