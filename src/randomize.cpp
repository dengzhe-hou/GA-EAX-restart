#ifndef __RAND__
#include "randomize.h"
#endif

// 全局随机数生成器指针
TRandom* tRand = NULL;

// 初始化随机数生成器（使用默认种子）
void InitURandom(){
    int seed;
    unsigned short seed16v[3];
    seed = 1111;                 // 设置默认种子值
    seed16v[0] = 100;
    seed16v[1] = 200;
    seed16v[2] = seed;
    tRand = new TRandom();      // 创建随机数生成器实例
    srand(seed);                // 初始化C标准库随机数生成器
}

// 初始化随机数生成器（使用指定种子）
void InitURandom(int dd){
    int seed;
    unsigned short seed16v[3];
    seed = dd;                  // 使用传入的种子值
    seed16v[0] = 100;
    seed16v[1] = 200;
    seed16v[2] = seed;
    tRand = new TRandom();      // 创建随机数生成器实例
    srand(seed);                // 初始化C标准库随机数生成器
}

// 随机数生成器类的构造和析构函数
TRandom::TRandom(){}
TRandom::~TRandom(){}

// 生成指定范围内的随机整数
int TRandom::Integer(int minNumber, int maxNumber){
    return minNumber + (rand() % (maxNumber - minNumber + 1));
}

// 生成指定范围内的随机浮点数
double TRandom::Double(double minNumber, double maxNumber){
    return minNumber + rand() % (int)(maxNumber - minNumber);
}

// 生成随机排列
// array: 用于存储结果的数组
// numOfElement: 总元素数量
// numOfSample: 需要采样的数量
void TRandom::permutation(int *array, int numOfElement, int numOfSample){
    if(numOfElement <= 0) return;
    
    int i, j, k, r;
    int *b = new int[numOfElement];    // 创建标记数组
    for(j=0; j<numOfElement; j++) b[j]=0;  // 初始化标记数组
    
    // 生成随机排列
    for(i=0; i<numOfSample; i++){
        r = rand() % (numOfElement-i);  // 生成随机位置
        k = 0;
        // 查找未使用的位置
        for(j=0; j<=r; j++){
            while(b[k]==1) ++k;
            k++;
        }
        array[i] = k-1;         // 记录选中的位置
        b[k-1] = 1;            // 标记已使用
    }
    delete [] b;                // 释放临时数组
}

// 生成正态分布随机数
// mu: 均值
// sigma: 标准差
double TRandom::normalDistribution(double mu, double sigma){
    double U1, U2, X;
    double PI = 3.1415926;
    
    // Box-Muller变换生成正态分布随机数
    while(1){
        U1 = this->Double(0.0, 1.0);
        if(U1 != 0.0) break;
    }
    U2 = this->Double(0.0, 1.0);
    X = sqrt(-2.0*log(U1)) * cos(2*PI*U2);
    return(mu + sigma*X);
}

// 随机打乱数组元素顺序
void TRandom::shuffle(int *array, int numOfElement){
    int *a = new int[numOfElement];     // 临时数组存储原始数据
    int *b = new int[numOfElement];     // 存储随机位置
    
    // 生成随机排列
    this->permutation(b, numOfElement, numOfElement);
    
    // 复制原始数组
    for(int i = 0; i < numOfElement; ++i) 
        a[i] = array[i];
    
    // 根据随机排列重新排序
    for(int i = 0; i < numOfElement; ++i) 
        array[i] = a[b[i]];
        
    // 释放临时数组
    delete [] a;
    delete [] b;
}