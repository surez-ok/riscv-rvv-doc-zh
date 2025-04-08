#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int16_t vec1[DATALEN] = {0};
  int32_t res[DATALEN] = {0};

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = 32767;
  }

  int16_t *pSrcA = vec1;
  int32_t *pDes = res;

  size_t avl = DATALEN;

  vint16m2_t op1;
  vint32m4_t rd;

  size_t vl;

  for (; (vl = __riscv_vsetvl_e16m2(avl)) > 0; avl -= vl) {
    op1 = __riscv_vle16_v_i16m2(pSrcA, vl);
    pSrcA += vl;

    // rd = __riscv_vwadd_vx_i32m4 (op1, 0, vl); // 方法1： 加0扩宽
    rd = __riscv_vwcvt_x_x_v_i32m4(op1, vl);     // 方法2： 使用vwcvt指令
    // rd = __riscv_vsext_vf2_i32m4(op1, vl);    // 方法3： 使用vsext指令，这几种方法是等价的

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
