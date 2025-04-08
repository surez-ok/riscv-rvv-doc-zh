#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

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
