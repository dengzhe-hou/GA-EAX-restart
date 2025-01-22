#ifndef __ENVIRONMENT__
#include "environment.h"
#endif

#include <math.h> 

// 全局变量声明
extern int gBestValue;      // 全局最优值
extern TIndi gBest;         // 全局最优个体
extern int duration;        // 运行时间

// 函数声明
void MakeRandSol(TEvaluator* eval, TIndi& indi);    // 生成随机解
void Make2optSol(TEvaluator* eval, TIndi& indi);    // 生成2-opt优化解

// 环境类构造函数
TEnvironment::TEnvironment()
{
    fEvaluator = new TEvaluator();  // 创建评估器
}

// 环境类析构函数
TEnvironment::~TEnvironment()
{
    // 释放所有动态分配的内存
    delete [] fIndexForMating;
    delete [] tCurPop;
    delete fEvaluator;
    delete tCross;

    // 释放边频率矩阵
    int N = fEvaluator->Ncity;
    for( int i = 0; i < N; ++i ) 
        delete [] fEdgeFreq[ i ];
    delete [] fEdgeFreq;
}

// 定义环境参数和初始化数据结构
void TEnvironment::define()
{
    // 设置问题实例
    fEvaluator->setInstance( fFileNameTSP );
    int N = fEvaluator->Ncity;

    // 初始化配对索引和当前种群
    fIndexForMating = new int [ Npop + 1 ];  
    tCurPop = new TIndi [ Npop ];
    for ( int i = 0; i < Npop; ++i ) 
        tCurPop[i].define( N );

    // 初始化全局最优解
    gBestValue = -1;
    gBest.define(N);

    // 初始化最优解
    tBest.define( N );

    // 初始化交叉操作器
    tCross = new TCross( N );
    tCross->eval = fEvaluator;                 
    tCross->Npop = Npop;             

    // 初始化K-opt操作器
    tKopt = new TKopt( N );
    tKopt->eval = fEvaluator;
    tKopt->setInvNearList();

    // 初始化边频率矩阵
    fEdgeFreq = new int* [ N ]; 
    for( int i = 0; i < N; ++i ) 
        fEdgeFreq[ i ] = new int [ N ];

    // 记录开始时间
    this->fTimeStart = clock();
}

// 主要执行函数
void TEnvironment::doIt()
{
    // 初始化
    this->initPop();         // 初始化种群
    this->init();            // 初始化参数
    this->getEdgeFreq();     // 获取边频率
    
    // 计算当前运行时间
    this->fTimeEnd = clock();
    duration = (int)((double)(this->fTimeEnd - this->fTimeStart) / (double)CLOCKS_PER_SEC);

    // 主循环
    while (duration < tmax)
    {
        // 计算平均值和最优值
        this->setAverageBest();

        // 更新全局最优解
        if (gBestValue == -1 || fBestValue < gBestValue)
        {
            gBestValue = fBestValue;
            gBest = tBest;
            // 如果达到最优值则终止
            if (gBestValue <= this->optimum)
            {
                printf("Find optimal solution %d, exit\n", gBestValue);
                this->terminate = true;
                break;
            }
        }

        // 每50代输出一次状态
        if (fCurNumOfGen % 50 == 0)
        {
            printf("%d:\t%d\t%lf\n", fCurNumOfGen, fBestValue, fAverageValue);
            // 检查运行时间
            this->fTimeEnd = clock();
            duration = (int)((double)(this->fTimeEnd - this->fTimeStart) / (double)CLOCKS_PER_SEC);
            if (duration >= tmax)
                break;
        }

        // 检查终止条件
        if (this->terminationCondition())
            break;

        // 生成新一代
        this->selectForMating();
        for (int s = 0; s < Npop; ++s)
            this->generateKids(s);

        ++fCurNumOfGen;
    }

    // 超时终止
    if (duration >= tmax)
        this->terminate = true;
}

// 初始化算法参数
void TEnvironment::init()
{
    fAccumurateNumCh = 0;    // 累积子代数量
    fCurNumOfGen = 0;        // 当前代数
    fStagBest = 0;           // 最优解停滞代数
    fMaxStagBest = 0;        // 最大允许停滞代数
    fStage = 1;              // 阶段1
    fFlagC[0] = 4;           // 多样性保持策略：4表示使用熵
    fFlagC[1] = 1;           // E集合类型：1表示单AB循环
} 

// 检查终止条件
bool TEnvironment::terminationCondition()
{
    // 如果平均值接近最优值，则终止
    if ( fAverageValue - fBestValue < 0.001 )  
        return true;

    if (fStage == 1) /* 阶段1 */
    {
        // 设置最大停滞代数
        if( fStagBest == int(1500/Nch) && fMaxStagBest == 0)
        {
            fMaxStagBest = int(fCurNumOfGen / 10);
        }
        // 从阶段1转到阶段2
        else if( fMaxStagBest != 0 && fMaxStagBest <= fStagBest ){ 
            fStagBest = 0;
            fMaxStagBest = 0;
            fCurNumOfGen1 = fCurNumOfGen;
            fFlagC[ 1 ] = 2;   // 切换到Block2模式
            fStage = 2;
        }
        return false;
    }

    if (fStage == 2) /* 阶段2 */
    {
        // 设置最大停滞代数
        if( fStagBest == int(1500/Nch) && fMaxStagBest == 0 )
        {
            fMaxStagBest = int( (fCurNumOfGen - fCurNumOfGen1) / 10 );
        }
        // 达到阶段2的终止条件，算法结束
        else if( fMaxStagBest != 0 && fMaxStagBest <= fStagBest )
        {
            return true;
        }
        return false;
    }

    return true;
}

// 设置种群的平均值和最佳值
void TEnvironment::setAverageBest()
{
    int stockBest = tBest.fEvaluationValue;  // 保存当前最佳解的评估值
    fAverageValue = 0.0;                     // 初始化平均值
    fBestIndex = 0;                          // 初始化最佳解的索引
    fBestValue = tCurPop[0].fEvaluationValue;// 初始化最佳值为第一个个体的评估值
    
    // 遍历整个种群
    for(int i = 0; i < Npop; ++i )
    {
        fAverageValue += tCurPop[i].fEvaluationValue;  // 累加评估值
        // 如果找到更好的解，更新最佳索引和最佳值
        if( tCurPop[i].fEvaluationValue < fBestValue )
        {
            fBestIndex = i;
            fBestValue = tCurPop[i].fEvaluationValue;
        }
    }
    tBest = tCurPop[ fBestIndex ];           // 更新最佳解
    fAverageValue /= (double)Npop;           // 计算平均值
    
    // 如果找到了更好的解
    if( tBest.fEvaluationValue < stockBest )
    {
        fStagBest = 0;                       // 重置停滞计数器
        fBestNumOfGen = fCurNumOfGen;        // 记录当前代数
        fBestAccumeratedNumCh = fAccumurateNumCh;  // 记录累计变化次数
    }
    else ++fStagBest;                        // 否则增加停滞计数
}

// 初始化种群
void TEnvironment::initPop(){
    for ( int i = 0; i < Npop; ++i )
    {
        tKopt->makeRandSol(tCurPop[i]);      // 生成随机解
        tKopt->doIt(tCurPop[i]);             // 使用2-opt邻域搜索进行局部优化
    }
}

// 选择配对个体
void TEnvironment::selectForMating()
{
    // 生成0到Npop-1的随机排列作为配对索引
    tRand->permutation( fIndexForMating, Npop, Npop ); 
    fIndexForMating[ Npop ] = fIndexForMating[ 0 ];  // 首尾相接，便于循环配对
}

// 生成子代
void TEnvironment::generateKids( int s )
{
    /* 注意：tCurPop[fIndexForMating[s]]将被最佳子代解替换
       同时在此过程中更新fEegeFreq[][] */
    // 设置父代
    tCross->setParents( tCurPop[fIndexForMating[s]], tCurPop[fIndexForMating[s+1]], fFlagC, Nch );  
    // 执行交叉操作
    tCross->doIt( tCurPop[fIndexForMating[s]], tCurPop[fIndexForMating[s+1]], Nch, 1, fFlagC, fEdgeFreq );
    fAccumurateNumCh += tCross->fNumOfGeneratedCh;  // 累加生成的子代数量
}

// 计算边的频率
void TEnvironment::getEdgeFreq()
{
    int  k0, k1, N = fEvaluator->Ncity;
    // 初始化频率矩阵
    for( int j1 = 0; j1 < N; ++j1 )
        for( int j2 = 0; j2 < N; ++j2 )
            fEdgeFreq[ j1 ][ j2 ] = 0;

    // 统计每条边在种群中出现的频率
    for( int i = 0; i < Npop; ++i )
        for(int j = 0; j < N; ++j ){
            k0 = tCurPop[ i ].fLink[ j ][ 0 ];
            k1 = tCurPop[ i ].fLink[ j ][ 1 ];
            ++fEdgeFreq[ j ][ k0 ];
            ++fEdgeFreq[ j ][ k1 ];
        }
}

// 输出结果
void TEnvironment::printOn()
{
    printf("Total time: %d\n", duration);     // 输出总运行时间
    printf("bestval = %d, optimum = %d \n", gBestValue, this->optimum);  // 输出最佳值和最优值
    fEvaluator->writeToStdout(gBest);        // 输出最佳解
    if (gBestValue != -1 && gBestValue <= this->optimum)
        printf("Successful\n");               // 如果达到最优解，输出成功
    else
        printf("Unsuccessful\n");             // 否则输出失败
    fflush(stdout);
}

// 将最佳解写入文件
void TEnvironment::writeBest()
{
    FILE *fp;
    char filename[ 80 ];

    sprintf( filename, "bestSolution.txt" );  // 设置输出文件名
    fp = fopen( filename, "a");              // 以追加模式打开文件
    fEvaluator->writeTo( fp, gBest );        // 写入最佳解
    fclose( fp );                            // 关闭文件
}