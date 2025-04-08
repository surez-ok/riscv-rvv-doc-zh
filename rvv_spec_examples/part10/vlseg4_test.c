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

  size_t vl = __riscv_vsetvl_e32m2(avl);

  vint32m2x4_t vtuple = __riscv_vlseg4e32_v_i32m2x4 (vec1, vl);

  vint32m2_t va = __riscv_vget_v_i32m2x4_i32m2 (vtuple, 0);
  vint32m2_t vb = __riscv_vget_v_i32m2x4_i32m2 (vtuple, 1);

  vint32m2_t vc = __riscv_vget_v_i32m2x4_i32m2 (vtuple, 2);
  vint32m2_t vd = __riscv_vget_v_i32m2x4_i32m2 (vtuple, 3);

  // 数据打印
  __riscv_vse32_v_i32m2 (res, va, vl);
  printf("va is:\r\n");
  for (int i = 0; i < avl; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  __riscv_vse32_v_i32m2 (res, vb, vl);
  printf("vb is:\r\n");
  for (int i = 0; i < avl; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  __riscv_vse32_v_i32m2 (res, vc, vl);
  printf("vc is:\r\n");
  for (int i = 0; i < avl; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  __riscv_vse32_v_i32m2 (res, vd, vl);
  printf("vd is:\r\n");
  for (int i = 0; i < avl; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}
