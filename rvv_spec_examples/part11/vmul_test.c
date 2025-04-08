#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  const int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  const int32_t vec2[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

  int32_t res[DATALEN] = {0};

  const int32_t *pSrcA = vec1;
  const int32_t *pSrcB = vec2;
  int32_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vint32m4_t op1, op2, rd;

  for (; (vl = __riscv_vsetvl_e32m4(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle32_v_i32m4(pSrcA, vl);
    op2 = __riscv_vle32_v_i32m4(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 向量乘法
    rd = __riscv_vmul_vv_i32m4(op1, op2, vl);

    // store数据
    __riscv_vse32_v_i32m4 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}