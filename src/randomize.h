#ifndef __RAND__
#define __RAND__

#include <stdio.h>  // 标准输入输出库
#include <stdlib.h> // 标准库函数
#include <math.h>   // 数学函数库
#include <time.h>   // 时间相关函数库

// 初始化随机数生成器的两个重载函数声明
// 第一个允许传入种子，第二个使用默认种子
extern void InitURandom( int );  // 使用指定种子初始化
extern void InitURandom( void ); // 使用系统时间作为种子初始化

// 随机数生成器类 TRandom
class TRandom {
public:
    TRandom();    // 构造函数
    ~TRandom();   // 析构函数

    // 生成指定范围内的随机整数
    // minNumber: 最小值，maxNumber: 最大值
    int Integer( int minNumber, int maxNumber );

    // 生成指定范围内的随机浮点数
    // minNumber: 最小值，maxNumber: 最大值
    double Double( double minNumber, double maxNumber );

    // 生成服从正态分布的随机数
    // mu: 均值，sigma: 标准差
    double normalDistribution( double mu, double sigma );

    // 从数组中生成随机排列（部分随机抽样）
    // array: 输入数组，numOfelement: 数组总元素数，numOfSample: 抽样数量
    void permutation( int *array, int numOfelement, int numOfSample );

    // 随机打乱数组元素
    // array: 输入数组，numOfElement: 数组元素数量
    void shuffle( int *array, int numOfElement );
};

// 全局随机数生成器指针
extern TRandom* tRand;

#endif