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
  const float32_t vec2[DATALEN] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };

  float32_t res[DATALEN] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

  const float32_t *pSrcA = vec1;
  const float32_t *pSrcB = vec2;
  float32_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vfloat32m4_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_f32m4(pSrcA, vl);
    op2 = __riscv_vle32_v_f32m4(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    rd = __riscv_vle32_v_f32m4(pDes, vl);
    // 向量乘加
    rd = __riscv_vfmacc_vv_f32m4(rd, op1, op2, vl);

    // store数据
    __riscv_vse32_v_f32m4 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%f, ", res[i]);
  }
  printf("\r\n");

  return 0;
}