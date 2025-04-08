#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

void main(void)
{
  uint8_t src1[2] = {0x04, 0x73};
  size_t vl;
  vl = __riscv_vsetvlmax_e16m2();

  vbool8_t mask1 = __riscv_vlm_v_b8(src1, vl); // vbool的load操作
  unsigned long index = __riscv_vfirst_m_b8 (mask1, vl);
  printf("index = %ld\r\n", index);
}