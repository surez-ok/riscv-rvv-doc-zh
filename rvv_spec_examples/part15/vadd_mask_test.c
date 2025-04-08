#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 32
int main(void)
{
  int32_t vec1[DATALEN];
  int32_t res[DATALEN] = {0};

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = i;
  }

  const int32_t *pSrcA = vec1;

  int32_t *pDes = res;
  uint8_t src1[4] = {0xFF, 0xFA, 0xFF, 0xFF};

  size_t avl = DATALEN;
  size_t vl;
  vint32m8_t op1, rd;

  vl = __riscv_vsetvl_e32m8(avl);
  // load数据
  op1 = __riscv_vle32_v_i32m8(pSrcA, vl);
  pSrcA += vl;

  vbool4_t mask = __riscv_vlm_v_b4(src1, vl);

  rd = __riscv_vadd_vx_i32m8_m (mask, op1, 100, vl);

  // store数据
  __riscv_vse32_v_i32m8 (pDes, rd, vl);
  pDes += vl;


  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}