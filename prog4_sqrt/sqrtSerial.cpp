#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// 牛顿法做了一点变形 推起来还有点麻烦
// 牛顿迭代法可以做的事不只开方，对于很多非线性的函数都可以得到解析解。
// 必须要先有一个猜测值
// 在这里实际上求的是某个数平方根的倒数。我不明白为什么要这样绕一圈。
// 对于大于3的数算法不收敛。原因是初值的选取完全偏离了真实值。
// 一部分原因是牛顿法的本质是泰勒展开的应用，其作用范围应该是x的邻域。
// 且由于在对牛顿法变形的过程中默认guess是大于0的，又因为初始guess为1，所以经计算x不能大于或等于3
void sqrtSerial(int N,
                float initialGuess,
                float values[],
                float output[])
{

    static const float kThreshold = 0.00001f;

    for (int i=0; i<N; i++) {

        float x = values[i];
        float guess = initialGuess;

        float error = fabs(guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            error = fabs(guess * guess * x - 1.f);
        }

        output[i] = x * guess;
    }
}

