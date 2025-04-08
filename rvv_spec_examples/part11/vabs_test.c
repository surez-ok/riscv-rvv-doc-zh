#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
void main(void)
{
  int16_t vec1[DATALEN] = {-1, -2, -3, -4, 5, 6, 7, 8, 9, 10, 11, 12, -13, -14, -15, 16};
  int16_t res[DATALEN] = {0};

  size_t avl = DATALEN;
  size_t vl;
  vint16m2_t vx, v_zero;
  int16_t *pSrc = vec1;
  int16_t *pDest = res;

  v_zero = __riscv_vmv_v_x_i16m2(0, vl);
  for (; (vl = __riscv_vsetvl_e16m2(avl)) > 0; avl -= vl) {
    vx = __riscv_vle16_v_i16m2(pSrc, vl);
    pSrc += vl;
    // 当向量中的元素<0时，将mask对应的bit置1
    vbool8_t mask = __riscv_vmslt_vx_i16m2_b8(vx, 0, vl);
    // 将maskbit置1的元素求相反数
    vx = __riscv_vssub_vv_i16m2_m(mask, v_zero, vx, vl);

    __riscv_vse16_v_i16m2(pDest, vx, vl);
    pDest += vl;
  }

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}

