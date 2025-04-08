#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

typedef float float32_t;

#define DATALEN 8
int main(void)
{
  const float32_t vec1[DATALEN] = { 16.0, 16.40, 16.5000, 16.5001, 16.9, -16.1000, -16.5000, -16.9 };
  int32_t res[DATALEN] = {0.0};

  const float32_t *pSrcA = vec1;
  int32_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vfloat32m2_t op1;
  vint32m2_t rd;

  for (; (vl = __riscv_vsetvl_e32m2(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_f32m2(pSrcA, vl);
    pSrcA += vl;

    rd = __riscv_vfcvt_rtz_x_f_v_i32m2(op1, vl);

    // store数据
    __riscv_vse32_v_i32m2 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}