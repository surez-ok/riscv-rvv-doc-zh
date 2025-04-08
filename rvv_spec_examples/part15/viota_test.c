#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>


void main(void)
{
  uint8_t src1[2] = {0x0F, 0x73};
  size_t vl;
  uint16_t res[16] = {0};

  vl = __riscv_vsetvlmax_e16m2();

  vbool8_t mask1 = __riscv_vlm_v_b8(src1, vl); // vbool的load操作
  vuint16m2_t valm2 = __riscv_viota_m_u16m2 (mask1, vl);
  __riscv_vse16_v_u16m2(res, valm2, vl);

  for (int i = 0; i < 16; i++) {
    printf("0x%02x, ", res[i]);
  }
  printf("\r\n");
}