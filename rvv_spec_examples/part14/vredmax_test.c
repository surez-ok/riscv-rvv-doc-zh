#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>


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