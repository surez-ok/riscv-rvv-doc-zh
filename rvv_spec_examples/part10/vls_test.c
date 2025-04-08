#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int32_t vec1[DATALEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, };
  int32_t res[DATALEN] = {0};

  vint32m4_t vx;
  size_t avl = DATALEN;
  // 以字节为单位，对于EEW=32bit，bstride = 16 表示跨过4个元素
  ptrdiff_t bstride = 16;
  size_t vl = __riscv_vsetvl_e32m4(avl);

  vx = __riscv_vlse32_v_i32m4(vec1, bstride, vl);

  __riscv_vse32_v_i32m4 (res, vx, vl);

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}
