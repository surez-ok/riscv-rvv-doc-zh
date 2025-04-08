#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

#define DATALEN 16

void main(void)
{
  int32_t vec1[DATALEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  uint8_t compress_t[2] = {0xF3, 0x31};
  int32_t res[DATALEN] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600};

  size_t vl;
  vint32m4_t vx, vy;
  vbool8_t mask;
  vl = __riscv_vsetvl_e32m4(DATALEN);
  vx = __riscv_vle32_v_i32m4(vec1, vl);
  vy = __riscv_vle32_v_i32m4(res, vl);

  mask = __riscv_vlm_v_b8(compress_t, vl);
  vuint32m4_t valm4 = __riscv_viota_m_u32m4 (mask, vl);

  vy = __riscv_vrgather_vv_i32m4_mu (mask, vy, vx, valm4, vl);

  __riscv_vse32_v_i32m4(res, vy, vl);
  for (int i = 0; i < DATALEN; i++) {
    printf("%d, ", res[i]);
  }
  printf("\r\n");
}
