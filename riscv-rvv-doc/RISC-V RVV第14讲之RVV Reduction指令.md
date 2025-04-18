# RISC-V RVV第14 讲之RVV Reduction指令

[TOC]

向量reduction操作接收一个向量寄存器组中的元素和一个向量寄存器第0个元素作为入参，结果存放到目标向量寄存器的第0个元素中。

目标向量寄存器可以与源操作数重叠，包括掩码寄存器。

**注意：**向量reduction操作读取并写入向量寄存器的第0个元素，而不是直接使用标量寄存器，目的是使VPU与标量CPU解耦。

### 1 单宽度整数reduction指令

单宽整数reduction指令所有操作数与结果具有相同的SEW宽度，vredsum 算术和运算可能存在溢出。

~~~shell
 # Simple reductions, where [*] denotes all active elements:
 vredsum.vs vd, vs2, vs1, vm # vd[0] = sum( vs1[0] , vs2[*] )
 vredmaxu.vs vd, vs2, vs1, vm # vd[0] = maxu( vs1[0] , vs2[*] )
 vredmax.vs vd, vs2, vs1, vm # vd[0] = max( vs1[0] , vs2[*] )
 vredminu.vs vd, vs2, vs1, vm # vd[0] = minu( vs1[0] , vs2[*] )
 vredmin.vs vd, vs2, vs1, vm # vd[0] = min( vs1[0] , vs2[*] )
 vredand.vs vd, vs2, vs1, vm # vd[0] = and( vs1[0] , vs2[*] )
 vredor.vs vd, vs2, vs1, vm # vd[0] = or( vs1[0] , vs2[*] )
 vredxor.vs vd, vs2, vs1, vm # vd[0] = xor( vs1[0] , vs2[*] )
~~~

示例如下：

~~~c
// 求和，vredsum
#define DATALEN 16
int main(void)
{
  const int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  int32_t res;

  const int32_t *pSrcA = vec1;

  size_t avl = DATALEN;
  size_t vl;
  vint32m4_t op1;
  vint32m1_t v_res;

  v_res = __riscv_vmv_v_x_i32m1(0, 1);
  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_i32m4(pSrcA, vl);
    pSrcA += vl;

    v_res = __riscv_vredsum_vs_i32m4_i32m1 (op1, v_res, vl);
  }
  res = __riscv_vmv_x_s_i32m1_i32(v_res);
  // 数据打印
  printf("sum = %d\r\n", res);

  return 0;
}
~~~
打印结果为：
~~~log
sum = 136, 也即 sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16) = 136
~~~

vredmax 示例如下：
~~~c
// 求最大值，vredmax
#define DATALEN 16
int main(void)
{
  const int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  int32_t res;

  const int32_t *pSrcA = vec1;

  size_t avl = DATALEN;
  size_t vl;
  vint32m4_t op1;
  vint32m1_t v_res;

  v_res = __riscv_vmv_v_x_i32m1(vec1[0], 1);
  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_i32m4(pSrcA, vl);
    pSrcA += vl;

    v_res = __riscv_vredmax_vs_i32m4_i32m1 (op1, v_res, vl);
  }
  res = __riscv_vmv_x_s_i32m1_i32(v_res);
  // 数据打印
  printf("res = %d\r\n", res);

  return 0;
}
~~~

打印结果为：
~~~log
res = 16， 即数组vec1的最大值为16
~~~
vredand 所有元素按位相与，vredor 所有元素按位或， vredxor所有元素按位异或，这几条指令不再举例。


### 2 扩宽整数reduction指令

对于vwredsumu.vs与vwredsum.vs两条指令，在求和之前，先扩宽为2SEW宽度，这样避免溢出。

~~~shell
 # Unsigned sum reduction into double-width accumulator
 vwredsumu.vs vd, vs2, vs1, vm # 2*SEW = 2*SEW + sum(zero-extend(SEW))
 # Signed sum reduction into double-width accumulator
 vwredsum.vs vd, vs2, vs1, vm # 2*SEW = 2*SEW + sum(sign-extend(SEW))
~~~

### 3 单宽度向量浮点reduction指令

浮点也有reduction指令。

~~~shell
 # Simple reductions.
 vfredosum.vs vd, vs2, vs1, vm # Ordered sum
 vfredusum.vs vd, vs2, vs1, vm # Unordered sum
 vfredmax.vs vd, vs2, vs1, vm  # Maximum value
 vfredmin.vs vd, vs2, vs1, vm  # Minimum value
~~~

**vfredosum与vfredusum的区别：**

vfredosum是顺序的，性能较差，vfredosum指令必须按元素顺序对浮点值求和，从vs1[0]中的标量开始，即它执行计算：

~~~c
vd[0] = `(((vs1[0] + vs2[0]) + vs2[1]) + ...) + vs2[vl-1]`
~~~

其中，每个加操作在引发异常标志和生成或传播特殊值方面与标量浮点指令相同。

而vfredusum是乱序的，性能比vfredosum好，但其结果可能与标量运算结果有些小的差别，这是因为浮点运算的计算顺序可能会影响结果。

归纳来说：

|        | vfredosum（浮点顺序求和） | vfredusum（浮点乱序求和） |
| ------ | ------------------------- | ------------------------- |
| 性能   | 低                        | 高                        |
| 准确度 | 与标量一致                | 可能有些小差别            |

示例如下：

~~~c
// vfredusum
typedef float float32_t;

#define DATALEN 16
int main(void)
{
  const float32_t vec1[DATALEN] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
  float32_t res;

  const float32_t *pSrcA = vec1;

  size_t avl = DATALEN;
  size_t vl;
  vfloat32m4_t op1;
  vfloat32m1_t v_res;

  v_res = __riscv_vfmv_v_f_f32m1(0.0, 1);
  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_f32m4 (pSrcA, vl);
    pSrcA += vl;

    v_res = __riscv_vfredusum_vs_f32m4_f32m1 (op1, v_res, vl);
  }
  res = __riscv_vfmv_f_s_f32m1_f32(v_res);
  // 数据打印
  printf("sum = %f\r\n", res);

  return 0;
}
~~~

打印结果为：

~~~log
sum = 136.000000
~~~

示例如下：

~~~c
typedef float float32_t;

#define DATALEN 16
int main(void)
{
  const float32_t vec1[DATALEN] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
  float32_t res;

  const float32_t *pSrcA = vec1;

  size_t avl = DATALEN;
  size_t vl;
  vfloat32m4_t op1;
  vfloat32m1_t v_res;

  v_res = __riscv_vfmv_v_f_f32m1(vec1[0], 1);
  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_f32m4 (pSrcA, vl);
    pSrcA += vl;

    v_res = __riscv_vfredmax_vs_f32m4_f32m1 (op1, v_res, vl);
  }
  res = __riscv_vfmv_f_s_f32m1_f32(v_res);
  // 数据打印
  printf("res = %f\r\n", res);

  return 0;
}
~~~

打印结果为：

~~~log
res = 16.000000
~~~



### 4 扩宽向量浮点reduction指令

~~~shell
 # Simple reductions.
 vfwredosum.vs vd, vs2, vs1, vm # Ordered sum
 vfwredusum.vs vd, vs2, vs1, vm # Unordered sum
~~~



