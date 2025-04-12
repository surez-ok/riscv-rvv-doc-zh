#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <riscv_vector.h>

// reference https://github.com/riscv/riscv-v-spec/blob/master/example/strcpy.s

void gen_string(char *s, int n) {
  // char value range: -128 ~ 127
  for (int i = 0; i < n - 1; ++i)
    s[i] = (char)(rand() % 127) + 1;
  s[n - 1] = '\0';
}

#if 0
char *strcpy_vec(char *destination, const char *source) {
  unsigned char *dst = (unsigned char*)destination;
  unsigned char *src = (unsigned char*)source;
  size_t vlmax = __riscv_vsetvlmax_e8m8();
  long first_set_bit = -1;
  for (size_t vl; first_set_bit < 0; src += vl, dst += vl) {
    vuint8m8_t vec_src = __riscv_vle8ff_v_u8m8(src, &vl, vlmax);
    printf("vl = %d\r\n", vl);
    vbool1_t string_terminate = __riscv_vmseq_vx_u8m8_b1(vec_src, 0, vl);
    vbool1_t mask = __riscv_vmsif_m_b1(string_terminate, vl);

    __riscv_vse8_v_u8m8_m(mask, dst, vec_src, vl);
    first_set_bit = __riscv_vfirst_m_b1(string_terminate, vl);
  }
  return destination;
}
#else

char *strcpy_vec(char *destination, const char *source) {
  unsigned char *dst = (unsigned char*)destination;
  unsigned char *src = (unsigned char*)source;
  size_t vlmax = __riscv_vsetvlmax_e8m8();
  size_t vl;
  for (;;) {
     vuint8m8_t vec_src = __riscv_vle8ff_v_u8m8(src, &vl, vlmax);
     printf("vl = %d\r\n", vl);
     src += vl;
     __riscv_vse8_v_u8m8(dst, vec_src, vl);
     dst += vl;
     
     if (vl < vlmax)
       break;
     
  }
  return destination;
}
#endif

int main() {
  const int N = 2000;
  const uint32_t seed = 0xdeadbeef;
  srand(seed);

  // data gen
  char s0[N];
  gen_string(s0, N);

  // compute
  char golden[N], actual[N];
  strcpy(golden, s0);
  strcpy_vec(actual, s0);

  // compare
  puts(strcmp(golden, actual) == 0 ? "pass" : "fail");
}