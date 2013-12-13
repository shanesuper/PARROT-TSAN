ifdef PARROT
ifeq ($(PARROT), 1)
	DIR = llvm-parrot
else
	DIR = llvm
endif
else
	DIR = llvm
endif
CC 		:= $(XTERN_ROOT)/../tsan2/$(DIR)/build/Debug+Asserts/bin/clang
CXX		:= $(XTERN_ROOT)/../tsan2/$(DIR)/build/Debug+Asserts/bin/clang++
CFLAGS  := -O0 -g -w -fsanitize=thread -fPIE -fsanitize-blacklist=blacklist
CXXFLAGS:= -O0 -g -w -fsanitize=thread -fPIE -fsanitize-blacklist=blacklist
LDFLAGS := -fsanitize=thread -pie

C_SRCS  := $(wildcard *.c)
CXX_SRCS:= $(wildcard *.cc)

OBJS    := $(patsubst %.c, %, $(C_SRCS)) $(patsubst %.cc, %, $(CXX_SRCS))

fft : fft.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm
	-/usr/bin/time -p ./$@ -m20 -p4
	rm -f cur.exe
	ln -s $@ cur.exe

% : %.c 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	-/usr/bin/time -p ./$@
	rm -f cur.exe
	ln -s $@ cur.exe
#	objdump -S cur.exe > $@.asm

% : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
	-/usr/bin/time -p ./$@
	rm -f cur.exe
	ln -s $@ cur.exe
#	objdump -S cur.exe > $@.asm

clean:
	rm -f $(OBJS) cur.exe *.asm


TSAN2:
#	make -C ../../tsan1/drt/tsan clean
	@echo "-------------------------------"
	@echo "--------Building TSAN----------"
	@echo "-------------------------------"
	make -j17 -s -C ../tsan2/$(DIR)/build
	@echo "-------------------------------"
	@echo "------Building Complete--------"
	@echo "-------------------------------"

XTERN: 
	@echo "-----------------------------------------------------"
	@echo "-------------------BUILDING PARROT-------------------"
	@echo "-----------------------------------------------------"
#	make -s -C $(XTERN_ROOT)/obj ENABLE_OPTIMIZED=0 clean
	make -s -C $(XTERN_ROOT)/obj ENABLE_OPTIMIZED=0 -j17
	make -s -C $(XTERN_ROOT)/obj ENABLE_OPTIMIZED=0 install -j17
	@echo "-----------------------------------------------------"
	@echo "---------------FINISHED BUILDING PARROT--------------"
	@echo "-----------------------------------------------------"

.PHONY: clean TSAN1
