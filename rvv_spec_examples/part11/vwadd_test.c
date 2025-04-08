#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int16_t vec1[DATALEN];
  int16_t vec2[DATALEN];

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = 32767;
    vec2[i] = 100;
  }

  int16_t res16[DATALEN] = {0};
  int32_t res32[DATALEN] = {0};

  const int16_t *pSrcA = vec1;
  const int16_t *pSrcB = vec2;
  int16_t *pDes16 = res16;
  int32_t *pDes32 = res32;

  size_t avl = DATALEN;
  size_t vl;
  vint16m2_t op1, op2, rd16;
  vint32m4_t rd32;

  for (; (vl = __riscv_vsetvl_e16m2(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_i16m2(pSrcA, vl);
    op2 = __riscv_vle16_v_i16m2(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 向量加法
    rd16 = __riscv_vadd_vv_i16m2(op1, op2, vl);

    // store数据
    __riscv_vse16_v_i16m2 (pDes16, rd16, vl);
    pDes16 += vl;

    // 向量扩宽加法
    rd32 = __riscv_vwadd_vv_i32m4(op1, op2, vl);

    // store数据
    __riscv_vse32_v_i32m4 (pDes32, rd32, vl);
    pDes32 += vl;
  }

  // 数据打印
  printf("vadd not widen:\r\n");
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res16[i]);
  }
  printf("\r\n");
  // 数据打印
  printf("vadd with widen(vwadd):\r\n");
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res32[i]);
  }
  printf("\r\n");

  return 0;
}