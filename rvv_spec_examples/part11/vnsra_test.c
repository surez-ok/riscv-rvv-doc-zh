#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int16_t vec1[DATALEN];
  int8_t res[DATALEN] = {0};

  int16_t *pSrcA = vec1;
  int8_t *pDes = res;

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = 1967;
  }
  size_t shift = 4;

  size_t avl = DATALEN;
  size_t vl;
  vint16m2_t op1;

  vint8m1_t rd;

  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    op1 = __riscv_vle16_v_i16m2(pSrcA, vl);
    pSrcA += vl;
    // 缩减右移
    rd = __riscv_vnsra_wx_i8m1(op1, shift, vl);
    __riscv_vse8_v_i8m1 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}