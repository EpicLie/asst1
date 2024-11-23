#include "AVX2_implement.h"

int signal = 0;
extern int signal;
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
        // if(i==160080)
        // cout<<values[i]<<endl;
        __m256 x = _mm256_loadu_ps(values + i);
        // cout<<"sqrt_implement_by_AVX2"<<endl;
        __m256 guess_vec = _mm256_set1_ps(guess);
        __m256 guess_2_vec = _mm256_set1_ps(guess * guess);
        __m256 guess_3_vec = _mm256_set1_ps(guess * guess * guess);
        __m256 error = _mm256_sub_ps(_mm256_mul_ps(x, guess_2_vec), one_vec);
        // if (i == 0) {
        //     cout << *(float*)&x << endl;
        //     cout << *(float*)&error << endl;
            
        // }
        
        // cout<<"sqrt_implement_by_AVX2"<<endl;
        // 实现1，通过对符号位的操作。
        // // 先强转成整型，然后右移31位取出符号位，然后转回浮点型便于后续计算
        // __m256 abs_mask = _mm256_castsi256_ps(_mm256_srli_epi32(_mm256_castps_si256(error), 31));
        // // 为1代表负数，符号位取反。
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
        // cout << _mm256_movemask_ps(error_abs) << endl;
        // cout<<"sqrt_implement_by_AVX2"<<endl;
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
        // cout << _mm256_movemask_ps(guess_vec) << endl;
        _mm256_storeu_ps(output + i, _mm256_mul_ps(guess_vec, x));
        // cout << output[i] << endl;
        // cout << i << endl;
    }
}
