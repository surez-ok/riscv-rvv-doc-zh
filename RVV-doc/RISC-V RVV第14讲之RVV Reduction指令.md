# RISC-V RVV第14 讲之RVV Reduction指令



### 1 单宽度整数reduction指令

单宽整数reduction指令所有操作数与结果具有相同的SEW宽度，算术和可能存在溢出。

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

举例：

~~~c
// vredsum
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

vredmax 用例如下：
~~~c
// vredmax
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

举例如下：

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

用例：

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

打印日志如下：

~~~log
res = 16.000000
~~~



### 4 扩宽向量浮点reduction指令

~~~shell
 # Simple reductions.
 vfwredosum.vs vd, vs2, vs1, vm # Ordered sum
 vfwredusum.vs vd, vs2, vs1, vm # Unordered sum
~~~



