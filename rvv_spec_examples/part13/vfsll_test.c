#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>


#define DATALEN 16
int main(void)
{
  const int16_t vec1[DATALEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  const uint16_t vec2[DATALEN] = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

  int16_t res[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const uint16_t *pSrcB = vec2;
  int16_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint16m2_t op1;
  vuint16m2_t shift;
  vint16m2_t rd;

  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    op1 = __riscv_vle16_v_i16m2(pSrcA, vl);
    shift = __riscv_vle16_v_u16m2(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;
    // 逻辑左移
    rd = __riscv_vsll_vv_i16m2(op1, shift, vl);
    __riscv_vse16_v_i16m2 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}