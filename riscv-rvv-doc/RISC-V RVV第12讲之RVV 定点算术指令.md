# RISC-V RVV第12 讲之RVV定点算术指令

[TOC]

V扩展提供定点的算术运算指令，Q类型数是实现定点数的一种方式，参考：[Q (number format) - Wikipedia](https://en.wikipedia.org/wiki/Q_(number_format))

Q数据类型完整格式为： Qm.n ，共需1（符号位）+ m （整数位） + n （小数位）bit位来表示数据。

当整数位m = 0时，除符号位以外都是小数位，表示区间为[-1, 1)，可简写为Qn。如Q15表示16bit，其中1bit为符号位，其余15bit为小数位。

RVV中提供一些Q类型(Q7, Q15, Q31, Q63)运算的指令，恰当的运用这些指令可以提高运算效率。


### 1 单宽度向量饱和加减

~~~asm
# 无符号定点数饱和加
vsaddu.vv vd, vs2, vs1, vm # Vector-vector
vsaddu.vx vd, vs2, rs1, vm # vector-scalar
vsaddu.vi vd, vs2, imm, vm # vector-immediate

# 有符号定点数饱和加
vsadd.vv vd, vs2, vs1, vm # Vector-vector
vsadd.vx vd, vs2, rs1, vm # vector-scalar
vsadd.vi vd, vs2, imm, vm # vector-immediate

# 无符号定点数饱和减
vssubu.vv vd, vs2, vs1, vm # Vector-vector
vssubu.vx vd, vs2, rs1, vm # vector-scalar

# 有符号定点数饱和减
vssub.vv vd, vs2, vs1, vm # Vector-vector
vssub.vx vd, vs2, rs1, vm # vector-scalar
~~~

以Q15 类型运算举例：

**Qn的加法：**

带饱和的Q15 定点数加法：

~~~c
q15_t q_add_sat(q15_t a, q15_t b)
{
    q15_t result;
    int32_t tmp;

    tmp = (int32_t)a + (int32_t)b;
    if (tmp > 0x7FFF)
        tmp = 0x7FFF;
    if (tmp < -1 * 0x8000)
        tmp = -1 * 0x8000;
    result = (q15_t)tmp;

    return result;
}
~~~

若直接使用RVV指令来实现上述C代码，可以采用如下三条指令

~~~c
1. vwadd      扩宽add
2. vmin/vmax  饱和
3. vnsra      截断
~~~

不过RVV提供了定点算术指令：vsadd，实现上述几条指令的效果

示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };
  const int16_t vec2[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const int16_t *pSrcB = vec2;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    op2 = __riscv_vle16_v_i16m1(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 定点数向量加法
    rd = __riscv_vsadd_vv_i16m1(op1, op2, vl);

    // store数据
    __riscv_vse16_v_i16m1 (pDes, rd, vl);
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

打印结果为：

~~~log
res[8] = {3724, 3724, 3724, 3724, 32767, 32767, 32767, 32767}
~~~



**Qn的减法：**

同饱和加一样，带饱和的减法可使用vssub，一条指令实现如下C版本的效果。

~~~c
q15_t q_sub_sat(q15_t a, q15_t b)
{
    q15_t result;
    int32_t tmp;

    tmp = (int32_t)a - (int32_t)b;
    if (tmp > 0x7FFF)
        tmp = 0x7FFF;
    if (tmp < -1 * 0x8000)
        tmp = -1 * 0x8000;
    result = (q15_t)tmp;

    return result;
}
~~~

示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 32767, 32767, 32767, 32767, -32767, -32767, -32767, -32767, };
  const int16_t vec2[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const int16_t *pSrcB = vec2;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    op2 = __riscv_vle16_v_i16m1(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 定点数向量减法
    rd = __riscv_vssub_vv_i16m1(op1, op2, vl);

    // store数据
    __riscv_vse16_v_i16m1 (pDes, rd, vl);
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

打印结果为：

~~~log
res[8] = {30905, 30905, 30905, 30905, -32768, -32768, -32768, -32768}
~~~



### 2 单宽度向量平均加减

~~~asm
# Averaging add
# Averaging adds of unsigned integers.
vaaddu.vv vd, vs2, vs1, vm # roundoff_unsigned(vs2[i] + vs1[i], 1)
vaaddu.vx vd, vs2, rs1, vm # roundoff_unsigned(vs2[i] + x[rs1], 1)
# Averaging adds of signed integers.
vaadd.vv vd, vs2, vs1, vm # roundoff_signed(vs2[i] + vs1[i], 1)
vaadd.vx vd, vs2, rs1, vm # roundoff_signed(vs2[i] + x[rs1], 1)
# Averaging subtract
# Averaging subtract of unsigned integers.
vasubu.vv vd, vs2, vs1, vm # roundoff_unsigned(vs2[i] - vs1[i], 1)
vasubu.vx vd, vs2, rs1, vm # roundoff_unsigned(vs2[i] - x[rs1], 1)
# Averaging subtract of signed integers.
vasub.vv vd, vs2, vs1, vm # roundoff_signed(vs2[i] - vs1[i], 1)
vasub.vx vd, vs2, rs1, vm # roundoff_signed(vs2[i] - x[rs1], 1)
~~~



示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };
  const int16_t vec2[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const int16_t *pSrcB = vec2;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    op2 = __riscv_vle16_v_i16m1(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 定点数向量加法
    rd = __riscv_vaadd_vv_i16m1(op1, op2, __RISCV_VXRM_RDN, vl);

    // store数据
    __riscv_vse16_v_i16m1 (pDes, rd, vl);
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

打印结果为：

~~~log
res[8] = {1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767}
~~~



### 3 单宽度向量饱和乘

~~~asm
# Signed saturating and rounding fractional multiply
# See vxrm description for rounding calculation
vsmul.vv vd, vs2, vs1, vm # vd[i] = clip(roundoff_signed(vs2[i]*vs1[i], SEW-1))
vsmul.vx vd, vs2, rs1, vm # vd[i] = clip(roundoff_signed(vs2[i]*x[rs1], SEW-1))
~~~

**Qn的乘法：**

带饱和的乘法V扩展指令：vsmul

如：Q15向量乘法

~~~c
// saturate to range of int16_t
int16_t sat16(int32_t x)
{
	if (x > 0x7FFF) return 0x7FFF;
	else if (x < -0x8000) return -0x8000;
	else return (int16_t)x;
}

int16_t q_mul(int16_t a, int16_t b)
{
    int16_t result;
    int32_t temp;

    temp = (int32_t)a * (int32_t)b; // result type is operand's type
    // Correct by dividing by base and saturate result
    result = sat16(temp >> 15);

    return result;
}
~~~

示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 32767, 32767, 32767, 32767, -32767, -32767, -32767, -32767, };
  const int16_t vec2[DATALEN] = { 1862, 1862, 1862, 1862, 32767, 32767, 32767, 32767, };

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const int16_t *pSrcB = vec2;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    op2 = __riscv_vle16_v_i16m1(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    rd = __riscv_vsmul_vv_i16m1(op1, op2, __RISCV_VXRM_RDN, vl);

    // store数据
    __riscv_vse16_v_i16m1 (pDes, rd, vl);
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

打印结果为：

~~~log
res[4] = {1861, 1861, 1861, 1861, -32767, -32767, -32767, -32767}
~~~



### 4 单宽度向量缩放移位指令

~~~asm
 # Scaling shift right logical
 vssrl.vv vd, vs2, vs1, vm # vd[i] = roundoff_unsigned(vs2[i], vs1[i])
 vssrl.vx vd, vs2, rs1, vm # vd[i] = roundoff_unsigned(vs2[i], x[rs1])
 vssrl.vi vd, vs2, uimm, vm # vd[i] = roundoff_unsigned(vs2[i], uimm)
 # Scaling shift right arithmetic
 vssra.vv vd, vs2, vs1, vm # vd[i] = roundoff_signed(vs2[i],vs1[i])
 vssra.vx vd, vs2, rs1, vm # vd[i] = roundoff_signed(vs2[i], x[rs1])
 vssra.vi vd, vs2, uimm, vm # vd[i] = roundoff_signed(vs2[i], uimm)
~~~

注释中：

~~~c
roundoff_unsigned(v, d) = (unsigned(v) >> d) + r
roundoff_signed(v, d) = (signed(v) >> d) + r
~~~

示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 32767, 32767, 32767, 32767, -32767, -32767, -32767, -32767, };

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1, rd;
  size_t shift = 2;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    pSrcA += vl;

    rd = __riscv_vssra_vx_i16m1(op1, shift, __RISCV_VXRM_RDN, vl);

    // store数据
    __riscv_vse16_v_i16m1 (pDes, rd, vl);
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

打印结果为：

~~~c
res[8] = {8191, 8191, 8191, 8191, -8192, -8192, -8192, -8192}
~~~



### 5 缩减向量定点数剪切指令

~~~shell
# Narrowing unsigned clip
# SEW 2*SEW SEW
 vnclipu.wv vd, vs2, vs1, vm # vd[i] = clip(roundoff_unsigned(vs2[i], vs1[i]))
 vnclipu.wx vd, vs2, rs1, vm # vd[i] = clip(roundoff_unsigned(vs2[i], x[rs1]))
 vnclipu.wi vd, vs2, uimm, vm # vd[i] = clip(roundoff_unsigned(vs2[i], uimm))
# Narrowing signed clip
 vnclip.wv vd, vs2, vs1, vm # vd[i] = clip(roundoff_signed(vs2[i], vs1[i]))
 vnclip.wx vd, vs2, rs1, vm # vd[i] = clip(roundoff_signed(vs2[i], x[rs1]))
 vnclip.wi vd, vs2, uimm, vm # vd[i] = clip(roundoff_signed(vs2[i], uimm))
~~~

示例如下：

~~~c
#define DATALEN 8
int main(void)
{
  const int16_t vec1[DATALEN] = { 32767, 32767, 32767, 32767, -32767, -32767, -32767, -32767, };

  int8_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  int8_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m1_t op1;
  vint8mf2_t rd;
  size_t shift = 2;

  for (; (vl = __riscv_vsetvl_e16m1(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m1(pSrcA, vl);
    pSrcA += vl;

    rd = __riscv_vnclip_wx_i8mf2(op1, shift, __RISCV_VXRM_RDN, vl);

    // store数据
    __riscv_vse8_v_i8mf2 (pDes, rd, vl);
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

打印结果为：

~~~log
res[8] = {127, 127, 127, 127, -128, -128, -128, -128}
~~~



**参考：**

1. [Q (number format) - Wikipedia](https://en.wikipedia.org/wiki/Q_(number_format))
2. [Qformat Converter](https://chummersone.github.io/qformat.html)