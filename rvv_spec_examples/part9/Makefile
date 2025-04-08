CC := riscv64-unknown-linux-gnu-gcc
QEMU_USER := qemu-riscv64
SRC_DIRS := .
VLEN := 128

# 查找所有要编译的C文件
SRCS := $(sort $(shell find $(SRC_DIRS) -name '*.c'))

# 目标文件为源文件加上.elf后缀
TARGET := $(SRCS:%.c=%.elf)

# 定义运行QEMU的命令模板
define RUN_QEMU
$(QEMU_USER) -cpu rv64,g=true,c=true,v=true,vlen=$(VLEN),elen=64,vext_spec=v1.0 $1
endef

all: build run

build: $(TARGET)

# 编译规则
$(TARGET): %.elf: %.c
	$(CC) -march=rv64imafdcv -mabi=lp64d $< -o $@ -lm -static -O2 -g

# 运行目标
run: $(TARGET)
	@$(foreach file,$(TARGET),echo "run $(file)"; $(call RUN_QEMU,$(file));)

.PHONY: clean
clean:
	-rm *.elf
