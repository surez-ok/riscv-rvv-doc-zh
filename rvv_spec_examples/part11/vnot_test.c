#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int16_t vec1[DATALEN] = {0};
  int16_t res[DATALEN] = {0};

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = 1653;
  }

  int16_t *pSrcA = vec1;
  int16_t *pDes = res;

  size_t avl = DATALEN;

  vint16m2_t op1;
  vint16m2_t rd;

  size_t vl;

  for (; (vl = __riscv_vsetvl_e16m2(avl)) > 0; avl -= vl) {
    op1 = __riscv_vle16_v_i16m2(pSrcA, vl);
    pSrcA += vl;

    rd = __riscv_vxor_vx_i16m2(op1, -1, vl);       // vxor指令
    // rd = __riscv_vnot_v_i16m2(op1, vl);         // vnot指令; 这两种写法等价

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