#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>


void main(void)
{
  size_t vl;
  uint16_t res[16] = {0};

  vl = __riscv_vsetvlmax_e16m2();

  vuint16m2_t valm2 = __riscv_vid_v_u16m2 (vl);
  __riscv_vse16_v_u16m2(res, valm2, vl);

  for (int i = 0; i < 16; i++) {
    printf("0x%02x, ", res[i]);
  }
  printf("\r\n");
}