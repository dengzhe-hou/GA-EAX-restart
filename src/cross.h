#ifndef __Cross__
#define __Cross__

// 包含必要的头文件
#ifndef __RAND__
#include "randomize.h"    // 随机数生成
#endif

#ifndef __Sort__
#include "sort.h"         // 排序功能
#endif

#ifndef __INDI__
#include "indi.h"         // 个体类定义
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"    // 评估器
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// TCross类 - 实现EAX（边组装交叉）算法
class TCross
{
public:
    // 构造和析构函数
    TCross( int N );                    // N为问题规模
    ~TCross();

    // 主要的EAX操作函数
    void doIt(TIndi &tKid, TIndi &tPa2, int numOfKids, int flagP, int flagC[10], int **fEdgeFreq);  // EAX的主要过程
    void setParents(const TIndi &tPa1, const TIndi &tPa2, int flagC[10], int numOfKids);            // 设置父代信息
    void setABcycle(const TIndi &parent1, const TIndi &parent2, int flagC[10], int numOfKids);      // EAX步骤2：生成AB循环

    // 辅助函数
    void swap(int &x, int &y);          // 交换两个整数
    void formABcycle();                 // 存储发现的AB循环
    void changeSol(TIndi &tKid, int ABnum, int type);  // 将AB循环应用到中间解

    // 解的完整化和优化
    void makeCompleteSol(TIndi &tKid);  // EAX步骤5：完整化解
    void makeUnit();                    // EAX步骤5-1：构建单元
    void backToPa1(TIndi &tKid);        // 恢复到父代1
    void goToBest(TIndi &tKid);         // 修改到最优子代解

    // 评估和统计相关函数
    void incrementEdgeFreq(int **fEdgeFreq);  // 增加边的使用频率
    int calAdpLoss(int **fEdgeFreq);          // 计算平均距离的差异
    double calEntLoss(int **fEdgeFreq);       // 计算边熵的差异

    // Block2算法相关函数
    void setWeight(const TIndi &parent1, const TIndi &parent2);  // 设置权重
    int calCNaive();                           // 计算简单C值
    void searchEset( int num );                // 搜索E集合
    void addAB( int num );                     // 添加AB循环
    void deleteAB( int num );                  // 删除AB循环

    // 公共成员变量
    int fNumOfGeneratedCh;  // 生成的子代数量
    TEvaluator* eval;       // 评估器指针
    int Npop;              // 种群大小

private:
    // 临时存储和标志
    TIndi tBestTmp;        // 临时最优解
    int fFlagImp;          // 改进标志
    int fN;                // 问题规模
    int r;                 // 随机数
    int exam;              // 检查变量
    int examFlag;          // 检查标志
    int flagSt;            // 起始标志
    int flagCycle;         // 循环标志
    int prType;            // 处理类型
    int chDis;             // 距离变化
    int koritsuMany;       // 孤立点数量
    int bunkiMany;         // 分支点数量
    int st;                // 起始点
    int ci;                // 当前点
    int pr;                // 前一点
    int stock;             // 临时存储
    int stAppear;          // 起始点出现次数
    int fEvalType;         // 评估类型
    int fEsetType;         // E集合类型
    int fNumOfABcycleInESet;  // E集合中AB循环数量
    int fNumOfABcycle;     // AB循环总数
    int fPosiCurr;         // 当前位置
    int fMaxNumOfABcycle;  // 最大AB循环数量

    // 基本数据数组
    int *koritsu;         // 孤立点数组
    int *bunki;           // 分支点数组
    int *koriInv;         // 孤立点反向映射
    int *bunInv;          // 分支点反向映射
    int *checkKoritsu;    // 孤立点检查数组
    int *fRoute;          // 路径数组
    int *fPermu;          // 排列数组
    int *fC;              // 临时计算数组
    int *fJun;            // 顺序数组
    int *fOrd1;           // 顺序数组1
    int *fOrd2;           // 顺序数组2

    int **nearData;       // 近邻数据
    int **fABcycle;       // AB循环数据

    // 性能优化相关变量
    int fNumOfUnit;        // 单元数量
    int fNumOfSeg;         // 分段数量
    int fNumOfSPL;         // 分段位置列表数量
    int fNumOfElementInCU; // 中心单元中元素数量
    int fNumOfSegForCenter; // 中心分段数量
    int fGainModi;         // 修改增益
    int fNumOfModiEdge;    // 修改边数量
    int fNumOfBestModiEdge; // 最佳修改边数量
    int fNumOfAppliedCycle; // 应用的循环数量
    int fNumOfBestAppliedCycle; // 最佳应用循环数量

    // 位置和序列相关数组
    int *fOrder;          // 顺序数组
    int *fInv;            // 反向映射
    int *fSegUnit;        // 分段单元
    int *fSegPosiList;    // 分段位置列表
    int *LinkAPosi;       // A链接位置
    int *fPosiSeg;        // 位置分段映射
    int *fNumOfElementInUnit; // 单元中元素数量
    int *fCenterUnit;     // 中心单元
    int *fListOfCenterUnit; // 中心单元列表
    int *fSegForCenter;   // 中心分段
    int *fGainAB;         // AB增益
    int *fAppliedCylce;   // 应用的循环
    int *fBestAppliedCylce; // 最佳应用循环

    // 二维数组
    int **fSegment;       // 分段信息
    int **LinkBPosi;      // B链接位置
    int **fModiEdge;      // 修改边
    int **fBestModiEdge;  // 最佳修改边

    // Block2算法相关变量
    int fNumOfUsedAB;     // 使用的AB循环数量
    int fNumC;            // C节点数量
    int fNumE;            // 边数量
    int fTmax;            // 最大温度
    int fMaxStag;         // 最大停滞次数
    int fNumOfABcycleInEset; // E集合中AB循环数量
    int fDisAB;           // AB距离
    int fBestNumC;        // 最佳C节点数量
    int fBestNumE;        // 最佳边数量

    // Block2算法相关数组
    int *fNumOfElementINAB;  // AB循环中元素数量
    int *fWeightSR;         // SR权重
    int *fWeightC;          // C权重
    int *fUsedAB;           // 使用的AB循环标记
    int *fMovedAB;          // 移动的AB循环
    int *fABcycleInEset;    // E集合中的AB循环

    int **fInEffectNode;    // 受影响的节点
    int **fWeightRR;        // RR权重
};

#endif