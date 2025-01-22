#ifndef __Sort__
#include "sort.h"
#endif

// 全局排序器指针，初始化为空
TSort* tSort = NULL;

// 初始化排序器
void InitSort(){
    tSort = new TSort();
}

// 交换两个整数的值
void swap(int &x, int &y){
    int s=x;
    x=y;
    y=s;
}

// 选择排序算法
// 从左到右找出最小元素并交换到正确位置
void selectionSort(int* Arg, int l, int r){
    int id;
    for(int i=l;i<r;++i){
        id=i;
        // 在未排序部分找出最小元素
        for(int j=i+1;j<=r;++j)
            if(Arg[j]<Arg[id]) id=j;
        // 将最小元素交换到当前位置
        swap(Arg[i], Arg[id]);
    }
}

// 快速排序的分区函数
// 随机选择一个基准元素，并将小于基准的元素移动到左侧
int partition(int* Arg, int l, int r){
    // 随机选择基准元素
    int id=l+rand()%(r-l+1);
    swap(Arg[l], Arg[id]);
    id=l;
    // 将小于基准的元素移动到左侧
    for(int i=l+1;i<=r;++i)
        if(Arg[i]<Arg[l]) swap(Arg[++id], Arg[i]);
    // 将基准元素放到正确位置
    swap(Arg[l], Arg[id]);
    return id;
}

// 快速排序算法
// 对于小规模数组，使用选择排序
// 对于大规模数组，使用分治策略
void quickSort(int* Arg, int l, int r){
    if(l<r){
        // 对于小数组，使用选择排序提高效率
        if(r-l<20)
        {
            selectionSort(Arg, l, r);
            return ;
        }
        // 分区并递归排序
        int mid=partition(Arg, l, r);
        quickSort(Arg, l, mid-1);
        quickSort(Arg, mid+1, r);
    }
}

// TSort类的构造函数和析构函数（默认实现）
TSort::TSort(){}
TSort::~TSort(){}

// 对浮点数数组按升序找出前 numOfOrd 个元素的索引
void TSort::index( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd ){
    int indexBest = 0;
    double valueBest;
    // 使用 checked 数组标记已选择的元素
    int *checked = new int [ numOfArg ];
    for( int i = 0 ; i < numOfArg ; ++i ) checked[ i ] = 0;
    
    for( int i = 0; i < numOfOrd; ++i ){
        // 初始化为一个很大的值
        valueBest = 99999999999.9;
        // 找出未选择的最小元素
        for( int j = 0; j < numOfArg; ++j ){
            if( ( Arg[j] < valueBest ) && checked[j]==0){
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        // 记录最小元素的索引并标记
        indexOrderd[ i ]=indexBest;
        checked[ indexBest ]=1;
    }
    delete [] checked;
}

// 对浮点数数组按降序找出前 numOfOrd 个元素的索引
void TSort::indexB( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd )
{
    int indexBest = 0;
    double valueBest;
    int *checked = new int [ numOfArg ];
    for( int i = 0 ; i < numOfArg ; ++i ) checked[ i ] = 0;
    
    for( int i = 0; i < numOfOrd; ++i ){
        // 初始化为一个很小的值
        valueBest = -99999999999.9;
        // 找出未选择的最大元素
        for( int j = 0; j < numOfArg; ++j ){
            if( ( Arg[j] > valueBest ) && checked[j]==0){
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        // 记录最大元素的索引并标记
        indexOrderd[ i ]=indexBest;
        checked[ indexBest ]=1;
    }
    delete [] checked;
}

// 对整数数组按升序找出前 numOfOrd 个元素的索引
void TSort::index( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd ){
    int indexBest = 0;
    int valueBest;
    int *checked = new int [ numOfArg ];
    for( int i = 0 ; i < numOfArg ; ++i ) checked[ i ] = 0;
    
    for( int i = 0; i < numOfOrd; ++i ){
        // 初始化为一个很大的值
        valueBest = 99999999;
        // 找出未选择的最小元素
        for( int j = 0; j < numOfArg; ++j ){
            if( ( Arg[j] < valueBest ) && checked[j]==0){
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        // 记录最小元素的索引并标记
        indexOrderd[ i ]=indexBest;
        checked[ indexBest ]=1;
    }
    delete [] checked;
}

// 对整数数组按降序找出前 numOfOrd 个元素的索引
void TSort::indexB( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd ){
    int indexBest = 0;
    int valueBest;
    int *checked = new int [ numOfArg ];
    for( int i = 0 ; i < numOfArg ; ++i ) checked[ i ] = 0;
    
    for( int i = 0; i < numOfOrd; ++i ){
        // 初始化为一个很小的值
        valueBest = -999999999;
        // 找出未选择的最大元素
        for( int j = 0; j < numOfArg; ++j ){
            if( ( Arg[j] > valueBest ) && checked[j]==0){
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        // 记录最大元素的索引并标记
        indexOrderd[ i ]=indexBest;
        checked[ indexBest ]=1;
    }
    delete [] checked;
}

// 对整数数组进行排序（默认使用快速排序）
void TSort::sort( int* Arg, int numOfArg ){
    //selectionSort(Arg, 0, numOfArg-1);  // 注释掉的选择排序
    quickSort(Arg, 0, numOfArg-1);  // 使用快速排序
}