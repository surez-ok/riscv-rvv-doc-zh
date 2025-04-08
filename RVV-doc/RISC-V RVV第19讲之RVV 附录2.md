## 附录D： 其它指令

这里列举一些spec没有说到的但是存在v扩展指令。

### D.1 vset 与 vget （向量插入/提取指令）

下例同时列出vset与vget指令的用法。

~~~c
void main(void)
{
    int16_t vec1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int16_t vec2[8] = {100, 200, 300, 400, 500, 600, 700, 800};

    int16_t res[16] = {0}, res2[8] = {0};

	uint32_t blkCnt = 16;
	size_t l;
	size_t index = 1;     // 因为mul = 2，所以index只能取[0, 1]
	vint16m2_t v_x, v_zero;
	vint16m1_t valm1, val2m1;
	l = vsetvlmax_e16m2();
	v_x = vle16_v_i16m2(vec1, l);
	valm1 = vle16_v_i16m1(vec2, 8);
    // 插入
	v_x = vset_v_i16m1_i16m2(v_x, index, valm1);
	vse16_v_i16m2(res, v_x, l);
    for (int i = 0; i < 16; i++) {
    	printf("%d \r\n", res[i]);
    }
    // 提取 m=0 的向量
    index = 0;
    val2m1 = vget_v_i16m2_i16m1(v_x, index);
    vse16_v_i16m1(res2, val2m1, l);
    for (int i = 0; i < 8; i++) {
        	printf("%d \r\n", res2[i]);
    }
}
// 结果为：res[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 100, 200, 300, 400, 500, 600, 700, 800}; 相当于将vec2插入到vec1中
//       res2[8] = { 1, 2, 3, 4, 5, 6, 7, 8}
~~~

### D.2 vreinterpret（强制转换指令）

可以进行不同数据类型的转换。

下例表示float32与int32类型的转换。

~~~c

~~~



### 注意

这两个api不是一个东西

v_sum = __riscv_vfmv_v_f_f32m8(0.0f, l);

v_sum = __riscv_vfmv_s_f_f32m8(0.0f, l);

**参考：**

1. [vcompress指令实现电路](https://blog.csdn.net/u013613650)