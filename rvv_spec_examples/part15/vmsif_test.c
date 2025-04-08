#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>


void main(void)
{
  uint8_t src1[2] = {0xE0, 0x73};
	size_t vl;
	uint8_t res[2] = {0};

	vl = __riscv_vsetvlmax_e16m2();

	vbool8_t mask1 = __riscv_vlm_v_b8(src1, vl); // vbool的load操作
	vbool8_t mask = __riscv_vmsif_m_b8 (mask1, vl);
	__riscv_vsm_v_b8 (res, mask, vl);            // vbool的store操作

	for (int i = 0; i < 2; i++) {
		printf("0x%02x \r\n", res[i]);
	}
}