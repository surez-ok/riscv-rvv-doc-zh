#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

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