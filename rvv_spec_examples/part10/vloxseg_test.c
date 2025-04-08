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

  size_t avl = 16;

  vuint32m4_t bindex;
  // 注意index单位为byte
  uint32_t index_table[4] = {7 * 4, 3 * 4, 10 * 4, 8 * 4};

  size_t vl = __riscv_vsetvl_e32m4(avl);

  bindex = __riscv_vle32_v_u32m4(index_table, vl);

  vint32m4x2_t vtuple = __riscv_vloxseg2ei32_v_i32m4x2 (vec1, bindex, vl);

  vint32m4_t vx = __riscv_vget_v_i32m4x2_i32m4 (vtuple, 0);
  vint32m4_t vy = __riscv_vget_v_i32m4x2_i32m4 (vtuple, 1);

  // 数据打印
  __riscv_vse32_v_i32m4 (res, vx, vl);
  printf("vx is:\r\n");
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  __riscv_vse32_v_i32m4 (res, vy, vl);
  printf("vy is:\r\n");
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}