#include "AVX2_implement.h"

int signal = 0;
extern int signal;

/*
speedup:

[sqrt serial]:          [650.757] ms
sqrt_implement_by_AVX2
[sqrt ispc]:            [120.863] ms
[sqrt task ispc]:       [34.532] ms
                                (5.38x speedup from ISPC)
                                (18.85x speedup from task ISPC)
*/



/*小结：
_mm256_movemask_ps：输出32位整数。对于浮点向量中的每个通道，取其最高位(符号位)作为整数的对应位。
                    显然其最大值为0xFF。 ps意即packed single。

_mm256_set1_ps：设置一个向量，其元素都为输入值。

_mm256_setzero_ps：设置一个向量，其元素全为0。

_mm256_cmp_ps：比较浮点数，返回一个向量。根据比较操作符(GT/LT/EQ)比较ab向量，若满足条件则对应位为1.
               注意OQ和OS的区别。OQ为非 signaling NaN，而OS为 signaling NaN。O代表Oderned，S为Signaling， Q代表 quiet

_mm256_blendv_ps：根据掩码，若该位为1,选择b向量的通道，否则选择a向量的通道。

_mm256_loadu_ps：加载一个向量，其元素为地址指向的8个浮点数。u代表内存未32位对齐。

_mm256_storeu_ps：将向量存储到地址，其元素为向量的8个浮点数。u代表内存未32位对齐。

_mm256_srli_epi32：右移操作符。srli：s代表shift,r代表right,l代表logical逻辑移位，i代表immediate 立即数。
立即数（Immediate Value）是指直接嵌入到指令中的常量值。
立即数通常用于指定操作数或控制操作的行为，而不是从寄存器或内存中读取数据。

_mm256_andnot_ps: 每个元素都是第一个向量中相应元素的补码与第二个向量中相应元素的按位与的结果。
（命名简直神经，我还以为是按位与非）

*/
void sqrt_implement_by_AVX2(int N,
                float initialGuess,
                float values[],
                float output[])
{
    if (signal == 0) {
        cout << "sqrt_implement_by_AVX2" << endl;
        signal++;
    }
    
    static const float kThreshold = 0.00001f;
    __m256 kThreshold_vec = _mm256_set1_ps(kThreshold);
    __m256 abs_vec = _mm256_set1_ps(-1.0f);
    __m256 zero_vec = _mm256_set1_ps(0.f);
    __m256 one_vec = _mm256_set1_ps(1.0f);
    __m256 half_vec = _mm256_set1_ps(0.5f);
    __m256 three_vec = _mm256_set1_ps(3.0f);
    float guess = initialGuess;
    // cout<<"sqrt_implement_by_AVX2"<<endl;
    for (int i = 0; i < N-8; i += 8)
    {

        __m256 x = _mm256_loadu_ps(values + i);
        __m256 guess_vec = _mm256_set1_ps(guess);
        __m256 guess_2_vec = _mm256_set1_ps(guess * guess);
        __m256 guess_3_vec = _mm256_set1_ps(guess * guess * guess);
        __m256 error = _mm256_sub_ps(_mm256_mul_ps(x, guess_2_vec), one_vec);
        
        // cout<<"sqrt_implement_by_AVX2"<<endl;
        // 实现1，通过对符号位的操作。
        // // 先强转成整型，然后右移31位取出符号位，然后转回浮点型便于后续计算
        // __m256 abs_mask = _mm256_castsi256_ps(_mm256_srli_epi32(_mm256_castps_si256(error), 31));
        // // 为1代表负数，符号位取反。
        // 不对，按这个方法，大于0的通道保持不变，小于0的通道赋值为0.并不是绝对值。
        // __m256 error_abs = _mm256_andnot_ps(abs_mask, error);
        // 实现2，通过掩码与乘法。
        __m256 abs_mask = _mm256_cmp_ps(error, zero_vec, _CMP_LT_OQ);
        __m256 error_obj = _mm256_mul_ps(error, abs_vec);
        __m256 error_abs = _mm256_blendv_ps(error, error_obj, abs_mask);
        // if (i == 8) {
        //     cout << *(float*)&x << endl;
        //     cout << *(float*)&error << endl;
        //     cout << *(float*)&error_abs << endl;
        //     sleep(10000);
        // }
        // if(i==8)
        // cout << "error:"<<_mm256_movemask_ps(_mm256_cmp_ps(_mm256_sub_ps(error_abs, kThreshold_vec), zero_vec, _CMP_GT_OQ)) != 0xFF << endl;
        while (_mm256_movemask_ps(_mm256_cmp_ps(_mm256_sub_ps(error_abs, kThreshold_vec), zero_vec, _CMP_GT_OQ)) != 0x0)
        {
            // cout<<"sqrt_implement_by_AVX2"<<endl;
            guess_vec = _mm256_mul_ps(_mm256_sub_ps(_mm256_mul_ps(guess_vec, three_vec), _mm256_mul_ps(x, guess_3_vec)), half_vec);
            guess_2_vec = _mm256_mul_ps(guess_vec, guess_vec);
            guess_3_vec = _mm256_mul_ps(guess_2_vec, guess_vec);
            error = _mm256_sub_ps(_mm256_mul_ps(x, guess_2_vec), one_vec);
            abs_mask = _mm256_cmp_ps(error, zero_vec, _CMP_LT_OQ);
            error_obj = _mm256_mul_ps(error, abs_vec);
            error_abs = _mm256_blendv_ps(error, error_obj, abs_mask);
            // if (i == 8) {
            //     cout << *(float*)&x << endl;
            //     cout << *(float*)&guess_vec << endl;
            //     cout << *(float*)&error << endl;
            //     cout << *(float*)&error_abs << endl;
            //     sleep(10000);
            // }
        }
        _mm256_storeu_ps(output + i, _mm256_mul_ps(guess_vec, x));
    }
}
