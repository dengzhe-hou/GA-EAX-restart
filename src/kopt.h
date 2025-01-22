#ifndef __KOPT__
#define __KOPT__

// 包含必要的头文件
#ifndef __RAND__
#include "randomize.h"    // 随机数生成
#endif

#ifndef __Sort__
#include "sort.h"         // 排序功能
#endif

#ifndef __INDI__
#include "indi.h"         // 个体（解）的定义
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"    // 评估器
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// K-opt局部搜索类定义
class TKopt{
public:
    // 构造和析构函数
    TKopt( int N );       // 构造函数，N为城市数量
    ~TKopt();             // 析构函数

    // 公共成员函数
    void setInvNearList();                    // 设置反向近邻列表
    void transIndiToTree( TIndi& indi );      // 将个体转换为树结构
    void transTreeToIndi( TIndi& indi );      // 将树结构转换回个体
    void doIt(TIndi &tIndi);                  // 执行2-opt邻域局部搜索

    // 路径操作相关函数
    int getNext( int t );                     // 获取下一个城市
    int getPrev( int t );                     // 获取前一个城市
    int turn( int &orient );                  // 转换方向

    // 核心优化函数
    void sub();                               // 子过程（局部搜索的主要逻辑）
    void incrementImp( int flagRev );         // 增量改进
    void combineSeg( int segL, int segS );    // 合并段

    // 辅助函数
    void checkDetail();                       // 检查数据结构细节
    void checkValid();                        // 检查解的有效性
    void swap(int &x, int &y);                // 交换两个整数
    void makeRandSol(TIndi &indi);            // 生成随机解

    TEvaluator* eval;                         // 评估器指针

private:
    // 基本参数
    int fN;              // 城市数量
    int fFixNumOfSeg;    // 固定的段数
    int fNumOfSeg;       // 当前段数
    int fFlagRev;        // 反转标志
    int fTourLength;     // 路径长度

    // 二维数组（动态分配）
    int **fLink;         // 城市间的连接关系
    int **fLinkSeg;      // 段间的连接关系
    int **fCitySeg;      // 段的起点和终点城市
    int **fInvNearList;  // 反向近邻列表

    // 一维数组（动态分配）
    int *fT;            // 临时数组（用于局部搜索）
    int *fB;            // 辅助数组
    int *fSegCity;      // 记录每个城市所属的段
    int *fOrdCity;      // 城市在段内的顺序
    int *fOrdSeg;       // 段的顺序
    int *fOrient;       // 段的方向
    int *fSizeSeg;      // 段的大小
    int *fActiveV;      // 活跃顶点标记
    int *fNumOfINL;     // 每个城市的反向近邻数量
    int *fArray;        // 通用数组
    int *fCheckN;       // 检查数组
    int *fGene;         // 基因（城市序列）数组
};

#endif