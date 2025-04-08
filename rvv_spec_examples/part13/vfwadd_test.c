#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <riscv_vector.h>

typedef float float32_t;
typedef _Float16 float16_t;

#define DATALEN 16
int main(void)
{
  float16_t vec1[DATALEN];
  float16_t vec2[DATALEN];

  for (int i = 0; i < DATALEN; i++) {
    vec1[i] = 32767.0f;
    vec2[i] = 100.0f;
  }

  float32_t res[DATALEN] = {0};

  const float16_t *pSrcA = vec1;
  const float16_t *pSrcB = vec2;
  float32_t *pDes = res;

  size_t avl = DATALEN;
  size_t vl;
  vfloat16m2_t op1, op2;
  vfloat32m4_t rd;

  for (; (vl = __riscv_vsetvl_e16m2(avl)) > 0; avl -= vl) {
    // load数据
    op1 = __riscv_vle16_v_f16m2(pSrcA, vl);
    op2 = __riscv_vle16_v_f16m2(pSrcB, vl);
    pSrcA += vl;
    pSrcB += vl;

    // 向量扩宽加法
    rd = __riscv_vfwadd_vv_f32m4(op1, op2, vl);

    // store数据
    __riscv_vse32_v_f32m4 (pDes, rd, vl);
    pDes += vl;
  }

  // 数据打印
  printf("vfwadd:\r\n");
  for (int i = 0; i < DATALEN; i++) {
    printf("%f, ", res[i]);
  }
  printf("\r\n");

  return 0;
}