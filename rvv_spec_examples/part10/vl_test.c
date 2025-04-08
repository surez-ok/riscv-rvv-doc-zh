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

  vint32m4_t vx;
  size_t avl = DATALEN;
  // 设置参数
  size_t vl = __riscv_vsetvl_e32m4(avl);
  // load
  vx = __riscv_vle32_v_i32m4(vec1, vl);
  // store数据，可以将vx线性化，顺序装到MEM地址为pDes的地方
  __riscv_vse32_v_i32m4 (res, vx, vl);

  // 数据打印
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");

  return 0;
}