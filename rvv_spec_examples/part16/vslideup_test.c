#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int32_t vec1[DATALEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  int32_t res[DATALEN] = {0};

  size_t vl;
  vint32m4_t vx;
  vl = __riscv_vsetvl_e32m4(DATALEN);
  vx = __riscv_vle32_v_i32m4(vec1, vl);
  
  vx = __riscv_vslideup_vx_i32m4 (vx, vx, 3, vl);
  __riscv_vse32_v_i32m4(res, vx, vl);

  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}
