#ifndef __SORT__  // 防止头文件重复包含的预处理器宏
#define __SORT__

// 引入必要的标准库头文件
#include <stdio.h>   // 标准输入输出
#include <stdlib.h>  // 标准库函数
#include <math.h>    // 数学函数
#include <assert.h>  // 断言宏
#include <algorithm> // 标准算法库
using namespace std;

// 声明初始化排序器的函数
void InitSort();

// 声明交换两个整数值的函数
void swap(int &x, int &y);

// 声明选择排序函数
// Arg: 待排序数组
// l: 排序起始索引
// r: 排序结束索引
void selectionSort(int* Arg, int l, int r);

// 声明快速排序的分区函数
// 返回分区的中心点索引
int partition(int* Arg, int l, int r);

// 声明快速排序函数
void quickSort(int* Arg, int l, int r);

// 排序工具类 TSort
class TSort {
public:
    // 构造函数和析构函数
    TSort();
    ~TSort();

    // 对浮点数数组按升序找出前 numOfOrd 个元素的索引
    // Arg: 输入数组
    // numOfArg: 数组总长度
    // indexOrderd: 输出的索引数组
    // numOfOrd: 需要找出的元素个数
    void index( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd );

    // 对整数数组按升序找出前 numOfOrd 个元素的索引
    void index( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd );

    // 对浮点数数组按降序找出前 numOfOrd 个元素的索引
    void indexB( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd );

    // 对整数数组按降序找出前 numOfOrd 个元素的索引
    void indexB( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd );

    // 对整数数组进行排序
    void sort( int* Arg, int numOfArg );
};

// 全局排序器指针
extern TSort* tSort;

#endif  // __SORT__ 结束预处理器条件编译