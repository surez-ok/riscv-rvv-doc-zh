#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16

void main(void)
{
  int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  uint32_t index[DATALEN] = { 5, 5, 5, 5, 5, 5, 5, 5, 11, 11, 11, 11, 15, 15, 15, 15 };
  int32_t res[DATALEN] = {0};

  size_t vl;
  vint32m4_t vx;
  vuint32m4_t vy;
  vl = __riscv_vsetvl_e32m4(DATALEN);
  vx = __riscv_vle32_v_i32m4(vec1, vl);
  vy = __riscv_vle32_v_u32m4(index, vl);

  vx = __riscv_vrgather_vv_i32m4 (vx, vy, vl);

  __riscv_vse32_v_i32m4(res, vx, vl);

  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}
