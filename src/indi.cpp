#ifndef __INDI__
#include "indi.h"
#endif

// 个体类构造函数
TIndi::TIndi()
{
    fN = 0;                    // 城市数量初始化为0
    fLink = NULL;              // 链接数组指针初始化为空
    fEvaluationValue = 0;      // 评估值（路径长度）初始化为0
}

// 析构函数：释放动态分配的内存
TIndi::~TIndi()
{
    // 释放二维数组内存
    for ( int i = 0; i < fN; ++i ) delete[] fLink[ i ];
    delete[] fLink;
}

// 定义个体：分配内存空间
void TIndi::define( int N )
{
    fN = N;                    // 设置城市数量
    fLink = new int* [ fN ];   // 为链接数组分配内存
    // 为每个城市分配2个整数空间（存储与之相连的两个城市的编号）
    for( int i = 0; i < fN; ++i ) fLink[ i ] = new int [ 2 ];
}

// 赋值运算符重载
TIndi& TIndi::operator = ( const TIndi& src )
{
    fN = src.fN;              // 复制城市数量
    // 复制链接信息
    for ( int i = 0; i < fN; ++i )
        for ( int j = 0; j < 2; ++j ) fLink[i][j] = src.fLink[i][j];
    fEvaluationValue = src.fEvaluationValue;  // 复制评估值
    return *this;
}

// 相等运算符重载：检查两个解是否相同
bool TIndi::operator == ( const TIndi& src ){
    int curr, next, pre, flag_identify;

    // 检查基本属性是否相同
    if( fN != src.fN ) return false;                    // 城市数量不同
    if( fEvaluationValue != src.fEvaluationValue ) return false;  // 评估值不同

    // 检查路径是否相同
    curr = 0;    // 从城市0开始
    pre = -1;    // 前一个城市初始化为-1
    // 遍历所有城市
    for( int i = 0; i < fN; ++i ){
        // 确定下一个城市
        if( fLink[curr][0] == pre ) next = fLink[curr][1];
        else next = fLink[curr][0];

        // 检查在另一个解中，当前城市是否也与下一个城市相连
        if( src.fLink[curr][0] != next && src.fLink[curr][1] != next ) return false;
        
        // 移动到下一个城市
        pre = curr;
        curr = next;
    }
    return true;  // 所有检查都通过，两个解相同
}