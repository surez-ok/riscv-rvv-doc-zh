#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16

void main(void)
{
  int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  int32_t res[DATALEN] = {0};

  size_t vl;
  vint32m4_t vx, vy;

  vl = __riscv_vsetvl_e32m4(DATALEN);
  vx = __riscv_vle32_v_i32m4(vec1, vl);

  // vmv.v.v
  vy = __riscv_vmv_v_v_i32m4 (vx, vl);


  __riscv_vse32_v_i32m4(res, vy, vl);

  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  // vmv.v.x
  vy = __riscv_vmv_v_x_i32m4 (100, vl);

  __riscv_vse32_v_i32m4(res, vy, vl);

  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  // vmv.x.s
  int32_t s1 = __riscv_vmv_x_s_i32m4_i32 (vy);
  printf("s0 = %d\r\n", s1);

   // vmv.v.x
  vy = __riscv_vmv_s_x_i32m4 (200, vl);

  __riscv_vse32_v_i32m4(res, vy, vl);

  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}
