## RISC-V RVV第9讲之搭建RVV开发环境

## 1 工具链

为了方便后续的测试，我们需要安装RISC-V GCC编译器和qemu运行环境。

**riscv gcc：**

为了方便，riscv gcc这里我们下载已预编译好的`riscv64-unknown-linux-gnu-gcc`与`riscv64-unknown-linux-gnu-gdb`

~~~html
https://github.com/riscv-collab/riscv-gnu-toolchain
~~~

下载：riscv64-glibc-ubuntu-22.04-gcc-nightly-2025.01.20-nightly.tar.xz

解压并配置工具链路径：

~~~sh
# 解压
$ tar -zxvf riscv64-glibc-ubuntu-20.04-gcc-nightly-2024.09.03-nightly.tar.gz

# 配置工具链路径
$ export PATH=/path/to/riscv/bin/:$PATH

# 测试
$ riscv64-unknown-linux-gnu-gcc -v
...
gcc version 14.2.0

$ riscv64-unknown-linux-gnu-gdb -v
GNU gdb (GDB) 15.1
~~~

**riscv qemu：**

qemu没有预编译好的版本，需要自己手动编译，为了方便，这里我们选择qemu-user模式。

~~~sh
$ sudo apt-get install -y build-essential pkg-config libglib2.0-dev zlib1g-dev libpixman-1-dev autoconf automake libtool bison flex texinfo gcc g++ git

$ git clone -b stable-9.0 https://github.com/qemu/qemu.git
$ cd qemu

$ mkdir build && cd build

$ ../configure --prefix=/path/to/qemu/build/linux_qemu --disable-pa --disable-linux-aio --disable-rbd --disable-vnc --disable-sdl --disable-sdl-image --target-list=riscv64-linux-user

$ make -j

# 在 qemu/build 路径下生成 qemu-riscv64  
# 配置工具链路径
$ export PATH=/path/to/qemu/build/:$PATH

# 测试
$ ./qemu-riscv64 --version
qemu-riscv64 version 9.0.4 (v9.0.4)
Copyright (c) 2003-2024 Fabrice Bellard and the QEMU Project developers
~~~



## 2 编译调试RVV intrinsic 的一个实例

编译一个简单的RVV intrinsic 代码，并在qemu环境中运行和调试。

~~~c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  const int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  const int32_t vec2[DATALEN] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

  int32_t res[DATALEN] = {0};

  const int32_t *pSrcA = vec1;
  const int32_t *pSrcB = vec2;
  int32_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint32m4_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_i32m4(pSrcA, vl);
    op2 = __riscv_vle32_v_i32m4(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 向量加法
    rd = __riscv_vadd_vv_i32m4(op1, op2, vl);

    // store数据
    __riscv_vse32_v_i32m4 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}
~~~

**编译：**

使用如下命令，或者进入part9文件夹，执行make即可

~~~sh
$ riscv64-unknown-linux-gnu-gcc -march=rv64imafdcv -mabi=lp64d *.c -o vadd_example.elf -lm -static -O2 -g
~~~

其中，`-march=rv64imafdcv` 中的v表示支持RVV

**在qemu中运行：**

~~~sh
qemu-riscv64 -cpu rv64,g=true,c=true,v=true,vlen=128,elen=64,vext_spec=v1.0 ./vadd_example.elf
~~~

打印日志如下，可见成功的实现了向量加法：

~~~sh
$ qemu-riscv64 -cpu rv64,g=true,c=true,v=true,vlen=128,elen=64,vext_spec=v1.0 ./vadd_example.elf
2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
~~~

**单步调试RVV程序：**

要学习RVV指令，一个单步调试指令的环境是不可或缺的，这里使用qemu以及`riscv64-unknown-linux-gnu-gdb`来单步调试RVV程序。

注意：在使用`riscv64-unknown-linux-gnu-gcc`编译代码时带上-g

step1：qemu启动gdb服务端

使用`qemu-user`启动你的程序，并通过`-g`参数指定GDB服务器监听的端口号，这里监听的端口号为1234。

~~~sh
$ qemu-riscv64 -cpu rv64,g=true,c=true,v=true,vlen=128,elen=64,vext_spec=v1.0 -g 1234 ./vadd_example.elf
~~~

step2：使用gdb连接到qemu

新建一个终端，使用如下命令启动gdb

~~~sh
$ riscv64-unknown-linux-gnu-gdb
(gdb) target remote localhost:1234
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x00000000000104d4 in ?? ()
(gdb) file vadd_example.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from vadd_example.elf...
(gdb) b main
Breakpoint 1 at 0x10400: file main.c, line 9.
(gdb) c
Continuing.

Breakpoint 1, main () at main.c:9
9           const int32_t vec1[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

~~~

接下来，单步调试这个RVV函数，并查看RVV的寄存器，可见v24 寄存器的内容符合预期。

~~~sh
(gdb) set disassemble-next-line on
# 经过多次单步
(gdb)
20          op1 = __riscv_vle32_v_i32m4(vec1, vl);
=> 0x000000000001047a <main+122>:       02016c07                vle32.v v24,(sp)
(gdb) info reg vl
vl             0x10     16
(gdb) info reg vtype
vtype          0xd2     210
(gdb) info reg v24
v24            {q = {0x4000000030000000200000001}, l = {0x200000001, 0x400000003}, w = {0x1, 0x2, 0x3, 0x4}, s = {0x1, 0x0,
    0x2, 0x0, 0x3, 0x0, 0x4, 0x0}, b = {0x1, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0}}
~~~



**注意：**后续章节的RVV用例都是使用类似方法查看向量寄存器的。

## 3 rvv-intrinsic-doc 仓库的使用

**从哪里查询RVV intrinsic APIs？**

官方参考：https://github.com/riscv-non-isa/rvv-intrinsic-doc/tree/main

在线查询API：https://dzaima.github.io/intrinsics-viewer/

这里讲述怎么测试rvv-intrinsic-doc仓库中提供的API

**step1：下载rvv-intrinsic-doc 仓库**

~~~sh
$ git clone https://github.com/riscv-non-isa/rvv-intrinsic-doc
~~~

RVV intrinsic API 可查询 rvv-intrinsic-doc/auto-generated/intrinsic_funcs.adoc

**step2：测试 rvv intrinsic api**

~~~sh
$ cd rvv-intrinsic-doc/rvv-intrinsic-generator
$ make run-api-testing COMPILER=riscv64-unknown-linux-gnu-gcc
~~~

测试结果：

~~~log
-----------------------------------------------------------------
Total                                             :  468/ 468
-----------------------------------------------------------------
-----------------------------------------------------------------
API Test Summary:
-----------------------------------------------------------------
Vector Loads and Stores Intrinsics                :   29/  29
Vector Loads and Stores Segment Intrinsics        :  189/ 189
Vector Integer Arithmetic Intrinsics              :   63/  63
Vector Fixed-Point Arithmetic Intrinsics          :   13/  13
Vector Floating-Point Intrinsics                  :   46/  46
Vector Reduction Operations                       :   16/  16
Vector Mask Intrinsics                            :   18/  18
Vector Permutation Intrinsics                     :   10/  10
Miscellaneous Vector Utility Intrinsics           :    2/   2
BFloat16 Vector Loads and Stores Intrinsics       :    9/   9
BFloat16 Vector Loads and Stores Segment Intrinsics:   63/  63
BFloat16 Convert Intrinsics                       :    0/   0
BFloat16 Arithmetic Intrinsics                    :    2/   2
BFloat16 Miscellaneous Vector Utility Intrinsics  :    7/   7
Zvbb - Vector Bit-manipulation used in Cryptography:    1/   1
Zvbc - Vector Carryless Multiplication            :    0/   0
Zvkg - Vector GCM/GMAC                            :    0/   0
Zvkned - NIST Suite: Vector AES Block Cipher      :    0/   0
Zvknh - NIST Suite: Vector SHA-2 Secure Hash      :    0/   0
Zvksed - ShangMi Suite: SM4 Block Cipher          :    0/   0
Zvksh - ShangMi Suite: SM3 Secure Hash            :    0/   0
-----------------------------------------------------------------
Total                                             :  468/ 468
-----------------------------------------------------------------
~~~

表明编译器支持这些API





