#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16
int main(void)
{
  int vec1[DATALEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  int res[DATALEN] = {0};

  size_t avl = 7;

  size_t vl = __riscv_vsetvl_e32m4(avl);

  vint32m4x2_t vtuple = __riscv_vlseg2e32_v_i32m4x2 (vec1, vl);

  vint32m4_t vx = __riscv_vget_v_i32m4x2_i32m4 (vtuple, 0);
  vint32m4_t vy = __riscv_vget_v_i32m4x2_i32m4 (vtuple, 1);

  // 数据打印
  __riscv_vse32_v_i32m4 (res, vx, vl);
  printf("vx is:\r\n");
  for (int i = 0; i < 8; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  __riscv_vse32_v_i32m4 (res, vy, vl);
  printf("vy is:\r\n");
  for (int i = 0; i < 8; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}
