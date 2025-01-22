#ifndef __Cross__
#include "cross.h"
#endif

// TCross类的构造函数，用于初始化交叉操作所需的数据结构
// 参数N为问题规模（如城市数量）
TCross::TCross( int N ){
    fMaxNumOfABcycle = 2000; /* 设置AB循环的最大数量（2000通常足够） */
    fN = N;  // 存储问题规模
    tBestTmp.define( fN );  // 定义临时最优解存储空间
    
    // 初始化邻近数据数组，每个节点存储5个最近邻居
    nearData = new int* [ fN ];
    for ( int j = 0; j < fN; ++j )
        nearData[j] = new int [ 5 ];

    // 初始化AB循环数组，用于存储可能的交叉路径
    fABcycle = new int* [ fMaxNumOfABcycle ];
    for ( int j = 0; j < fMaxNumOfABcycle; ++j ) fABcycle[j] = new int [ 2*fN + 4 ];

    // 初始化各种辅助数组
    koritsu = new int [ fN ];     // 存储孤立节点
    bunki = new int [ fN ];       // 存储分支节点
    koriInv = new int [ fN ];     // 孤立节点的反向映射
    bunInv = new int [ fN ];      // 分支节点的反向映射
    checkKoritsu = new int [ fN ]; // 检查孤立节点的标记数组
    fRoute = new int [ 2*fN + 1 ]; // 存储路径
    fPermu = new int [ fMaxNumOfABcycle ]; // 存储排列

    // 初始化其他必要的数组
    fC = new int [ 2*fN+4 ];      // 用于计算的临时数组
    fJun = new int[ fN+ 1 ];      // 用于存储顺序
    fOrd1 = new int [ fN ];       // 第一个序列的顺序
    fOrd2 = new int [ fN ];       // 第二个序列的顺序

    // 性能优化相关的数据结构初始化
    fOrder = new int [ fN ];      // 存储顺序
    fInv = new int [ fN ];        // 存储逆序
    fSegment = new int* [ fN ];   // 存储线段信息
    for ( int j = 0; j < fN; ++j ) fSegment[ j ] = new int [ 2 ];

    // 初始化段单元相关的数组
    fSegUnit = new int [ fN ];    // 段单元数组
    fSegPosiList = new int[ fN ]; // 段位置列表
    LinkAPosi = new int [ fN ];   // A链接位置
    LinkBPosi = new int* [ fN ];  // B链接位置
    for ( int j = 0; j < fN; ++j ) LinkBPosi[ j ] = new int [ 2 ];

    // 初始化位置和单元相关的数组
    fPosiSeg = new int [ fN ];    // 位置段映射
    fNumOfElementInUnit = new int [ fN ]; // 单元中元素数量
    fCenterUnit = new int [ fN ]; // 中心单元
    for ( int j = 0; j < fN; ++j ) fCenterUnit[ j ] = 0;

    // 初始化中心单元和边修改相关的数组
    fListOfCenterUnit = new int [ fN+2 ]; // 中心单元列表
    fSegForCenter = new int [ fN ];       // 中心段
    fGainAB = new int [ fN ];            // AB增益
    fModiEdge = new int* [ fN ];         // 边修改信息
    for ( int j = 0; j < fN; ++j ) fModiEdge[ j ] = new int [ 4 ];

    // 初始化最优边修改相关的数组
    fBestModiEdge = new int* [ fN ];
    for ( int j = 0; j < fN; ++j ) fBestModiEdge[ j ] = new int [ 4 ];

    // 初始化应用循环相关的数组
    fAppliedCylce = new int [ fN ];
    fBestAppliedCylce = new int [ fN ];

    // Block2算法相关的数据结构初始化
    fNumOfElementINAB = new int [ fMaxNumOfABcycle ]; // AB循环中元素数量
    fInEffectNode = new int* [ fN ];                  // 受影响的节点
    for( int i = 0; i < fN; ++i ) fInEffectNode[ i ] = new int [ 2 ];

    // 初始化权重相关的数组
    fWeightRR = new int* [ fMaxNumOfABcycle ];
    for( int i = 0; i < fMaxNumOfABcycle; ++i ) fWeightRR[ i ] = new int [ fMaxNumOfABcycle ];

    fWeightSR = new int [ fMaxNumOfABcycle ];  // SR权重
    fWeightC = new int [ fMaxNumOfABcycle ];   // 中心权重
    fUsedAB = new int [ fN ];                  // 已使用的AB循环
    fMovedAB = new int [ fN ];                 // 已移动的AB循环
    fABcycleInEset = new int [ fMaxNumOfABcycle ]; // E集合中的AB循环
}

// 析构函数，负责释放所有动态分配的内存
TCross::~TCross()
{
    // 释放基础数组内存
    delete [] koritsu;
    delete [] bunki;
    delete [] koriInv;
    delete [] bunInv;
    delete [] checkKoritsu;
    delete [] fRoute;
    delete [] fPermu;
    
    // 释放二维数组内存
    for ( int j = 0; j < fN; ++j ) delete[] nearData[ j ];
    delete[] nearData;
    
    for ( int j = 0; j < fMaxNumOfABcycle; ++j ) delete[] fABcycle[ j ];
    delete[] fABcycle;
    
    // 释放其他数组内存
    delete [] fC;
    delete [] fJun;
    delete [] fOrd1;
    delete [] fOrd2;

    // 释放性能优化相关的内存
    delete [] fOrder;
    delete [] fInv;
    for ( int j = 0; j < fN; ++j ) delete[] fSegment[ j ];
    delete[] fSegment;
    delete[] fSegUnit;
    delete [] fSegPosiList;
    delete [] LinkAPosi;
    
    for ( int j = 0; j < fN; ++j ) delete[] LinkBPosi[ j ];
    delete [] LinkBPosi;
    
    // 释放位置和单元相关的内存
    delete [] fPosiSeg;
    delete [] fNumOfElementInUnit;
    delete [] fCenterUnit;
    delete [] fListOfCenterUnit;
    delete [] fSegForCenter;
    delete [] fGainAB;

    // 释放边修改相关的内存
    for ( int j = 0; j < fN; ++j ) delete[] fModiEdge[ j ];
    delete [] fModiEdge;
    for ( int j = 0; j < fN; ++j ) delete[] fBestModiEdge[ j ];
    delete [] fBestModiEdge;

    // 释放循环相关的内存
    delete [] fAppliedCylce;
    delete [] fBestAppliedCylce;

    // 释放Block2算法相关的内存
    delete [] fNumOfElementINAB;
    for ( int j = 0; j < fN; ++j ) delete [] fInEffectNode[ j ];
    delete [] fInEffectNode;
    
    // 释放权重相关的内存
    for( int i = 0; i < fMaxNumOfABcycle; ++i ) delete [] fWeightRR[ i ];
    delete [] fWeightRR;
    delete [] fWeightSR;
    delete [] fWeightC;
    delete [] fUsedAB;
    delete [] fMovedAB;
    delete [] fABcycleInEset;
}

// 设置父代个体并进行交叉操作的准备
// tPa1, tPa2: 两个父代个体
// flagC: 控制标志数组
// numOfKids: 子代数量
void TCross::setParents( const TIndi& tPa1, const TIndi& tPa2, int flagC[ 10 ], int numOfKids )
{
    // 设置AB循环
    this->setABcycle( tPa1, tPa2, flagC, numOfKids );
    fDisAB = 0;
    
    // 计算两个父代解之间的差异
    int curr, next, st, pre;
    st = 0;      // 起始节点
    curr = -1;   // 当前节点
    next = st;   // 下一个节点
    
    // 遍历所有节点
    for( int i = 0; i < fN; ++i ){
        pre = curr;   // 更新前一个节点
        curr = next;  // 更新当前节点
        
        // 确定下一个节点
        if( tPa1.fLink[curr][0] != pre ) next = tPa1.fLink[ curr ][ 0 ];
        else next = tPa1.fLink[curr][1];

        // 如果两个父代在当前位置的连接不同，增加差异计数
        if( tPa2.fLink[ curr ][ 0 ] != next && tPa2.fLink[ curr ][ 1 ] != next ) ++fDisAB;
        
        // 记录节点顺序和位置
        fOrder[ i ] = curr;
        fInv[ curr ] = i;
    }
    
    // Block2算法特殊处理
    if (flagC[1] == 2) /* Block2 */
    {
        fTmax = 10;           // 设置最大温度
        fMaxStag = 20;        // 设置最大停滞次数 (1:贪婪局部搜索, 20:禁忌搜索)
        this->setWeight( tPa1, tPa2 ); // 设置权重
    }
}

// 执行交叉操作的主要函数
// tKid: 子代个体
// tPa2: 第二个父代个体
// numOfKids: 需要生成的子代数量
// flagP: 控制标志
// flagC: 算法控制数组
// fEdgeFreq: 边的频率统计
void TCross::doIt( TIndi& tKid, TIndi& tPa2, int numOfKids, int flagP, int flagC[ 10 ], int **fEdgeFreq )
{
    int Num;                // 实际要处理的AB循环数量
    int jnum, centerAB;     // AB循环的索引和中心AB循环
    int gain;               // 当前获得的增益
    int BestGain;          // 最佳增益
    double pointMax, point; // 评估分数的最大值和当前值
    double DLoss;          // 损失值

    // 设置评估类型和集合类型
    fEvalType = flagC[ 0 ]; // 1:贪婪模式, 2:未使用, 3:距离保持, 4:熵保持
    fEsetType = flagC[ 1 ]; // 1:单AB循环模式, 2:Block2模式

    // 确定实际处理的AB循环数量
    if ( numOfKids <= fNumOfABcycle ) Num = numOfKids;
    else Num = fNumOfABcycle;

    // 单AB循环模式的处理
    if (fEsetType == 1) /* Single-AB */
    {
        // 对AB循环进行随机排列
        tRand->permutation( fPermu, fNumOfABcycle, fNumOfABcycle );
    }
    // Block2模式的处理
    else if( fEsetType == 2 )
    {   // 按照AB循环中元素数量进行排序
        for( int k = 0; k < fNumOfABcycle; ++k ) 
            fNumOfElementINAB[ k ] = fABcycle[ k ][ 0 ];
        tSort->indexB( fNumOfElementINAB, fNumOfABcycle, fPermu, fNumOfABcycle );
    }

    fNumOfGeneratedCh = 0;  // 已生成的子代数量
    pointMax = 0.0;         // 最大评分初始化
    BestGain = 0;          // 最佳增益初始化
    fFlagImp = 0;          // 改进标志初始化

    // 主循环：处理每个AB循环
    for( int j = 0; j < Num; ++j )
    {
        fNumOfABcycleInEset = 0;  // 重置集合中AB循环的数量

        // 单AB循环模式的处理
        if (fEsetType == 1) /* Single-AB */
        {
            jnum = fPermu[ j ];
            fABcycleInEset[ fNumOfABcycleInEset++ ] = jnum;
        }
        // Block2模式的处理
        else if (fEsetType == 2) /* Block2 */
        {
            jnum = fPermu[ j ];
            centerAB = jnum;
            // 选择相关的AB循环
            for( int s = 0; s < fNumOfABcycle; ++s ){
                if( s == centerAB ) 
                    fABcycleInEset[ fNumOfABcycleInEset++ ] = s;
                else{
                    // 根据权重和元素数量决定是否包含其他AB循环
                    if( fWeightRR[ centerAB ][ s ] > 0 && fABcycle[ s ][ 0 ] < fABcycle[ centerAB ][ 0 ] )
                        if( rand() %2 == 0 ) 
                            fABcycleInEset[ fNumOfABcycleInEset++ ] = s;
                }
            }
            this->searchEset( centerAB );  // 搜索相关的AB循环集合
        }

        // 初始化各种计数器和增益值
        fNumOfSPL = 0;
        gain = 0;
        fNumOfAppliedCycle = 0;
        fNumOfModiEdge = 0;

        // 应用选中的AB循环
        fNumOfAppliedCycle = fNumOfABcycleInEset;
        for( int k = 0; k < fNumOfAppliedCycle; ++k ){
            fAppliedCylce[ k ] = fABcycleInEset[ k ];
            jnum = fAppliedCylce[ k ];
            this->changeSol( tKid, jnum, flagP );  // 修改解
            gain += fGainAB[ jnum ];              // 累加增益
        }

        // 构建完整解
        this->makeUnit();                 // 构建单元
        this->makeCompleteSol( tKid );    // 构建完整解
        gain += fGainModi;                // 添加修改带来的增益

        ++fNumOfGeneratedCh;              // 增加已生成子代计数

        // 根据评估类型计算损失值
        if (fEvalType == 1)               // 贪婪模式
            DLoss = 1.0;
        else if (fEvalType == 3)          // 距离保持模式
            DLoss = this->calAdpLoss( fEdgeFreq );
        else if( fEvalType == 4 )         // 熵保持模式
            DLoss = this->calEntLoss( fEdgeFreq );

        // 确保损失值大于0
        if( DLoss <= 0.0 ) DLoss = 0.00000001;

        // 计算评分并更新解的评估值
        point = (double)gain / DLoss;
        tKid.fEvaluationValue = tKid.fEvaluationValue - gain;

        // 如果获得更好的解，保存相关信息
        if( pointMax < point && (2 * fBestNumE < fDisAB || tKid.fEvaluationValue != tPa2.fEvaluationValue ) )
        {
            pointMax = point;
            BestGain = gain;
            fFlagImp = 1;

            // 保存最佳应用的循环信息
            fNumOfBestAppliedCycle = fNumOfAppliedCycle;
            for( int s = 0; s < fNumOfBestAppliedCycle; ++s ) 
                fBestAppliedCylce[ s ] = fAppliedCylce[ s ];

            // 保存最佳修改边的信息
            fNumOfBestModiEdge = fNumOfModiEdge;
            for( int s = 0; s < fNumOfBestModiEdge; ++s ){
                fBestModiEdge[ s ][ 0 ] = fModiEdge[ s ][ 0 ];
                fBestModiEdge[ s ][ 1 ] = fModiEdge[ s ][ 1 ];
                fBestModiEdge[ s ][ 2 ] = fModiEdge[ s ][ 2 ];
                fBestModiEdge[ s ][ 3 ] = fModiEdge[ s ][ 3 ];
            }
        }

        // 恢复到父代1的状态并更新评估值
        this->backToPa1( tKid );
        tKid.fEvaluationValue = tKid.fEvaluationValue + gain;
    }

    // 如果找到更好的解，应用最佳改进
    if( fFlagImp == 1 ){
        this->goToBest( tKid );           // 应用最佳改进
        tKid.fEvaluationValue = tKid.fEvaluationValue - BestGain;  // 更新评估值
        this->incrementEdgeFreq( fEdgeFreq );  // 更新边的频率
    }
}

// 设置AB循环的函数
// tPa1, tPa2: 父代个体
// flagC: 控制标志数组
// numOfKids: 子代数量
void TCross::setABcycle( const TIndi& tPa1, const TIndi& tPa2, int flagC[ 10 ], int numOfKids )
{
    bunkiMany = 0;      // 分支点数量初始化
    koritsuMany = 0;    // 孤立点数量初始化

    // 初始化近邻数据结构
    for( int j = 0; j < fN ; ++j ){
        // 存储来自父代1的连接信息
        nearData[j][1] = tPa1.fLink[j][0];  // 第一个连接点
        nearData[j][3] = tPa1.fLink[j][1];  // 第二个连接点
        nearData[j][0] = 2;                 // 设置节点状态为2(可访问)

        // 将所有点初始化为孤立点
        koritsu[koritsuMany] = j;
        koritsuMany++;

        // 存储来自父代2的连接信息
        nearData[j][2] = tPa2.fLink[j][0];  // 第一个连接点
        nearData[j][4] = tPa2.fLink[j][1];  // 第二个连接点
    }

    // 初始化孤立点检查数组和反向映射
    for(int j = 0; j < fN; ++j ){
        checkKoritsu[j] = -1;              // 初始化检查数组
        koriInv[koritsu[j]] = j;           // 设置反向映射
    }

    fNumOfABcycle = 0;  // AB循环数量初始化
    flagSt = 1;         // 起始标志设置为1

    // 主循环：处理孤立点
    while(koritsuMany != 0){
        // 选择新的起始点
        if(flagSt == 1){
            fPosiCurr = 0;                     // 当前位置重置为0
            r = rand() % koritsuMany;          // 随机选择一个孤立点
            st = koritsu[r];                   // 设置起始点
            checkKoritsu[st] = fPosiCurr;      // 标记检查位置
            fRoute[fPosiCurr] = st;            // 记录路径
            ci = st;                           // 设置当前点
            prType = 2;                        // 设置处理类型为2
        }
        else if(flagSt == 0) ci = fRoute[fPosiCurr];  // 继续使用现有路径

        flagCycle = 0;  // 循环标志初始化
        
        // 寻找循环的内层循环
        while(flagCycle == 0){
            fPosiCurr++;           // 移动到下一个位置
            pr = ci;               // 保存前一个点
            
            // 根据不同的处理类型选择下一个点
            switch(prType){
            case 1:  // 简单连接
                ci = nearData[pr][fPosiCurr%2+1];
                break;
            case 2:  // 随机选择连接
                r = rand()%2;
                ci = nearData[pr][fPosiCurr%2+1+2*r];
                if(r == 0) this->swap(nearData[pr][fPosiCurr%2+1],nearData[pr][fPosiCurr%2+3]);
                break;
            case 3:  // 使用第二种连接
                ci = nearData[pr][fPosiCurr%2+3];
            }
            
            fRoute[fPosiCurr] = ci;  // 记录路径

            // 处理当前点的状态
            if(nearData[ci][0] == 2){  // 如果是可访问点
                if(ci == st){  // 如果回到起始点
                    if(checkKoritsu[st] == 0){
                        // 处理偶数长度路径
                        if((fPosiCurr-checkKoritsu[st])%2 == 0){
                            if(nearData[st][fPosiCurr%2+1] == pr) 
                                this->swap(nearData[ci][fPosiCurr%2+1],nearData[ci][fPosiCurr%2+3]);

                            stAppear = 1;
                            this->formABcycle();  // 形成AB循环
                            // 检查是否达到目标数量
                            if( flagC[ 1 ] == 1 && fNumOfABcycle == numOfKids ) goto RETURN;
                            if( fNumOfABcycle == fMaxNumOfABcycle ) goto RETURN;

                            flagSt = 0;
                            flagCycle = 1;
                            prType = 1;
                        }
                        else{  // 处理奇数长度路径
                            this->swap(nearData[ci][fPosiCurr%2+1],nearData[ci][fPosiCurr%2+3]);
                            prType = 2;
                        }
                        checkKoritsu[st] = fPosiCurr;
                    }
                    else{  // 第二次到达起始点
                        stAppear = 2;
                        this->formABcycle();
                        if( flagC[ 1 ] == 1 && fNumOfABcycle == numOfKids ) goto RETURN;
                        if( fNumOfABcycle == fMaxNumOfABcycle ) goto RETURN;

                        flagSt = 1;
                        flagCycle = 1;
                    }
                }
                // 处理新访问的点
                else if(checkKoritsu[ci] == -1) {
                    checkKoritsu[ci] = fPosiCurr;
                    if(nearData[ci][fPosiCurr%2+1] == pr) 
                        this->swap(nearData[ci][fPosiCurr%2+1],nearData[ci][fPosiCurr%2+3]);
                    prType = 2;
                }
                // 处理已访问过的点
                else if(checkKoritsu[ci] > 0){
                    this->swap(nearData[ci][fPosiCurr%2+1],nearData[ci][fPosiCurr%2+3]);
                    if((fPosiCurr-checkKoritsu[ci])%2 == 0){
                        stAppear = 1;
                        this->formABcycle();
                        if( flagC[ 1 ] == 1 && fNumOfABcycle == numOfKids ) goto RETURN;
                        if( fNumOfABcycle == fMaxNumOfABcycle ) goto RETURN;

                        flagSt = 0;
                        flagCycle = 1;
                        prType = 1;
                    }
                    else{
                        this->swap(nearData[ci][(fPosiCurr+1)%2+1],nearData[ci][(fPosiCurr+1)%2+3]);
                        prType = 3;
                    }
                }
            }
            // 处理分支点
            else if(nearData[ci][0] == 1){
                if(ci == st){
                    stAppear = 1;
                    this->formABcycle();
                    if( flagC[ 1 ] == 1 && fNumOfABcycle == numOfKids ) goto RETURN;
                    if( fNumOfABcycle == fMaxNumOfABcycle ) goto RETURN;
                    flagSt = 1;
                    flagCycle = 1;
                }
                else prType = 1;
            }
        }
    }

    // 处理剩余的分支点
    while(bunkiMany != 0){
        fPosiCurr = 0;
        r = rand() % bunkiMany;    // 随机选择一个分支点
        st = bunki[r];
        fRoute[fPosiCurr] = st;
        ci = st;

        flagCycle = 0;
        while(flagCycle == 0){
            pr = ci;
            fPosiCurr++;
            ci = nearData[pr][fPosiCurr%2+1];
            fRoute[fPosiCurr] = ci;
            if(ci == st){  // 如果形成循环
                stAppear = 1;
                this->formABcycle();
                if( flagC[ 1 ] == 1 && fNumOfABcycle == numOfKids ) goto RETURN;
                if( fNumOfABcycle == fMaxNumOfABcycle ) goto RETURN;

                flagCycle = 1;
            }
        }
    }

RETURN:
    // 检查是否超过最大AB循环数量限制
    if( fNumOfABcycle == fMaxNumOfABcycle ){
        printf( "fMaxNumOfABcycle(%d) must be increased\n", fMaxNumOfABcycle );
        exit( 1 );
    }
}

// 形成AB循环的函数
// 根据已找到的路径信息构建一个新的AB循环
void TCross::formABcycle(){
    int j;
    int st_count;        // 起始点出现次数计数
    int edge_type;       // 边的类型(1或2)
    int st, ci, stock;   // st:起始点, ci:当前点, stock:临时存储
    int cem;            // 循环中的边数量
    int diff;           // 路径长度差值

    // 根据当前位置确定边的类型
    if(fPosiCurr % 2 == 0) edge_type = 1;
    else edge_type = 2;

    // 初始化循环的起始点
    st = fRoute[fPosiCurr];
    cem = 0;
    fC[cem] = st;

    // 追踪路径直到回到起始点指定次数
    st_count = 0;
    while(1){
        cem++;
        fPosiCurr--;
        ci = fRoute[fPosiCurr];

        // 处理孤立点：将其转换为分支点
        if(nearData[ci][0] == 2){
            // 从孤立点列表中移除
            koritsu[koriInv[ci]] = koritsu[koritsuMany-1];
            koriInv[koritsu[koritsuMany-1]] = koriInv[ci];
            koritsuMany--;
            // 添加到分支点列表
            bunki[bunkiMany] = ci;
            bunInv[ci] = bunkiMany;
            bunkiMany++;
        }
        // 处理分支点：从分支点列表中移除
        else if(nearData[ci][0] == 1){
            bunki[bunInv[ci]] = bunki[bunkiMany-1];
            bunInv[bunki[bunkiMany-1]] = bunInv[ci];
            bunkiMany--;
        }

        // 更新点的状态
        nearData[ci][0]--;
        
        // 检查是否回到起始点
        if(ci == st) st_count++;
        if(st_count == stAppear) break;  // 达到要求的出现次数时退出
        
        fC[cem] = ci;  // 记录当前点到循环中
    }

    // 如果循环太短（只有2条边）则返回
    if(cem == 2) return;

    // 记录循环的大小
    fABcycle[fNumOfABcycle][0] = cem;

    // 如果是类型2的边，进行循环移位
    if(edge_type == 2){
        stock = fC[0];
        for(int j = 0; j < cem-1; j++) fC[j] = fC[j+1];
        fC[cem-1] = stock;
    }

    // 将循环复制到AB循环数组中
    for(int j = 0; j < cem; j++) 
        fABcycle[fNumOfABcycle][j+2] = fC[j];

    // 设置循环的额外信息（前后节点）
    fABcycle[fNumOfABcycle][1] = fC[cem-1];
    fABcycle[fNumOfABcycle][cem+2] = fC[0];
    fABcycle[fNumOfABcycle][cem+3] = fC[1];

    // 添加首两个节点到数组末尾以便计算
    fC[cem] = fC[0];
    fC[cem+1] = fC[1];

    // 计算路径长度差值（评估增益）
    diff = 0;
    for(j = 0; j < cem/2; ++j) 
        diff = diff + eval->fEdgeDis[fC[2*j]][fC[1+2*j]] - 
               eval->fEdgeDis[fC[1+2*j]][fC[2+2*j]];

    // 记录该AB循环的增益值
    fGainAB[fNumOfABcycle] = diff;
    ++fNumOfABcycle;  // AB循环数量加1
}

// 交换两个整数值的辅助函数
void TCross::swap(int &x, int &y){
    int s = x;
    x = y;
    y = s;
}

// 修改解的函数
// tKid: 待修改的子代解
// ABnum: 使用的AB循环编号
// type: 修改类型
void TCross::changeSol( TIndi& tKid, int ABnum, int type ){
    int j;
    int cem;                    // 循环中的边数量
    int r1, r2, b1, b2;        // 当前要处理的四个节点
    int po_r1, po_r2, po_b1, po_b2;  // 节点在解中的位置

    // 获取AB循环的大小
    cem = fABcycle[ABnum][0];
    fC[0] = fABcycle[ABnum][0];

    // 根据类型不同，以不同方式复制AB循环
    if(type == 2) 
        // 类型2：反向复制
        for(j = 0; j < cem+3; j++) 
            fC[cem+3-j] = fABcycle[ABnum][j+1];
    else 
        // 类型1：正向复制
        for(j = 1; j <= cem+3; j++) 
            fC[j] = fABcycle[ABnum][j];

    // 处理循环中的每对边
    for(j = 0; j < cem/2; j++){
        // 获取需要改变连接的四个节点
        r1 = fC[2+2*j];    // 当前边的第一个节点
        r2 = fC[3+2*j];    // 当前边的第二个节点
        b1 = fC[1+2*j];    // 新连接的第一个节点
        b2 = fC[4+2*j];    // 新连接的第二个节点

        // 更新第一个节点的连接
        if(tKid.fLink[r1][0] == r2) 
            tKid.fLink[r1][0] = b1;
        else 
            tKid.fLink[r1][1] = b1;
        
        // 更新第二个节点的连接
        if(tKid.fLink[r2][0] == r1) 
            tKid.fLink[r2][0] = b2;
        else 
            tKid.fLink[r2][1] = b2;

        // 获取各节点在解中的位置
        po_r1 = fInv[r1];
        po_r2 = fInv[r2];
        po_b1 = fInv[b1];
        po_b2 = fInv[b2];

        // 处理特殊情况：当节点在解的首尾位置时
        if(po_r1 == 0 && po_r2 == fN-1) 
            fSegPosiList[fNumOfSPL++] = po_r1;
        else if(po_r1 == fN-1 && po_r2 == 0) 
            fSegPosiList[fNumOfSPL++] = po_r2;
        // 记录较大的位置索引
        else if(po_r1 < po_r2) 
            fSegPosiList[fNumOfSPL++] = po_r2;
        else if(po_r2 < po_r1) 
            fSegPosiList[fNumOfSPL++] = po_r1;

        // 更新位置链接信息
        // 保存旧的连接信息
        LinkBPosi[po_r1][1] = LinkBPosi[po_r1][0];
        LinkBPosi[po_r2][1] = LinkBPosi[po_r2][0];
        // 设置新的连接信息
        LinkBPosi[po_r1][0] = po_b1;
        LinkBPosi[po_r2][0] = po_b2;
    }
}

// 构建完整解的函数
// tKid: 需要完整化的子代解
void TCross::makeCompleteSol( TIndi& tKid ){
    int j, j1, j2;
    int st, pre, curr, next;           // 用于遍历的变量
    int a, b, c, d, aa, bb, a1, b1;    // 存储节点
    int min_unit_city;                 // 最小单元中的城市数
    int center_un, select_un;          // 中心单元和选择的单元
    int diff, max_diff;                // 改进值和最大改进值
    int near_num, nearMax;             // 近邻数和最大近邻数

    fGainModi = 0;  // 初始化修改增益
    
    // 当还存在多个单元时继续处理
    while( fNumOfUnit != 1 ){
        // 找出包含最少城市的单元作为中心单元
        min_unit_city = fN + 12345;
        for( int u = 0; u < fNumOfUnit; ++u )
            if( fNumOfElementInUnit[ u ] < min_unit_city ){
                center_un = u;
                min_unit_city = fNumOfElementInUnit[ u ];
            }

        // 找出中心单元的起始节点
        st = -1;
        fNumOfSegForCenter = 0;
        for( int s = 0; s < fNumOfSeg; ++s )
            if( fSegUnit[ s ] == center_un ){
                int posi = fSegment[ s ][ 0 ];
                st = fOrder[ posi ];
                fSegForCenter[ fNumOfSegForCenter++ ] = s;
            }

        // 遍历中心单元，收集其所有节点
        curr = -1;
        next = st;
        fNumOfElementInCU = 0;
        while(1){
            pre = curr;
            curr = next;
            fCenterUnit[ curr ] = 1;  // 标记为中心单元的节点
            fListOfCenterUnit[ fNumOfElementInCU ] = curr;
            ++fNumOfElementInCU;
            // 确定下一个节点
            if( tKid.fLink[ curr ][ 0 ] != pre ) 
                next = tKid.fLink[ curr ][ 0 ];
            else 
                next = tKid.fLink[ curr ][ 1 ];
            if( next == st ) break;  // 回到起点时结束
        }
        // 添加首尾节点用于循环处理
        fListOfCenterUnit[ fNumOfElementInCU ] = fListOfCenterUnit[ 0 ];
        fListOfCenterUnit[ fNumOfElementInCU+1 ] = fListOfCenterUnit[ 1 ];

        // 初始化最大改进值和相关节点
        max_diff = -999999999;
        a1 = -1; b1 = -1;
        nearMax = 10;  // 设置初始近邻搜索范围

    RESTART:
        // 寻找最佳改进方案
        for( int s = 1; s <= fNumOfElementInCU; ++s ){
            a = fListOfCenterUnit[ s ];

            // 遍历近邻节点
            for( near_num = 1; near_num <= nearMax; ++near_num ){
                c = eval->fNearCity[ a ][ near_num ];
                // 如果不是中心单元的节点
                if( fCenterUnit[ c ] == 0 ){
                    // 尝试不同的连接方式
                    for( j1 = 0; j1 < 2; ++j1 ){
                        b = fListOfCenterUnit[ s-1+2*j1 ];
                        for( j2 = 0; j2 < 2; ++j2 ){
                            d = tKid.fLink[ c ][ j2 ];
                            // 计算第一种连接方式的改进值
                            diff = eval->fEdgeDis[a][b] + eval->fEdgeDis[c][d] - 
                                  eval->fEdgeDis[a][c] - eval->fEdgeDis[b][d];
                            if( diff > max_diff ){
                                aa = a; bb = b; a1 = c; b1 = d;
                                max_diff = diff;
                            }
                            // 计算第二种连接方式的改进值
                            diff = eval->fEdgeDis[a][b] + eval->fEdgeDis[d][c] -
                                  eval->fEdgeDis[a][d] - eval->fEdgeDis[b][c];
                            if( diff > max_diff ){
                                aa = a; bb = b; a1 = d; b1 = c;
                                max_diff = diff;
                            }
                        }
                    }
                }
            }
        }

        // 如果没找到改进方案，扩大搜索范围
        if (a1 == -1 && nearMax == 10) {
            nearMax = 50;
            goto RESTART;
        }
        // 如果扩大搜索范围后仍未找到，随机选择一个改进方案
        else if( a1 == -1 && nearMax == 50  ){
            int r = rand() % ( fNumOfElementInCU - 1 );
            a = fListOfCenterUnit[ r ];
            b = fListOfCenterUnit[ r+1 ];
            for( j = 0; j < fN; ++j ){
                if( fCenterUnit[ j ] == 0 ){
                    aa = a; bb = b;
                    a1 = j;
                    b1 = tKid.fLink[ j ][ 0 ];
                    break;
                }
            }
            max_diff = eval->fEdgeDis[aa][bb] + eval->fEdgeDis[a1][b1] - 
                      eval->fEdgeDis[a][a1] - eval->fEdgeDis[b][b1];
        }

        // 更新连接关系
        if( tKid.fLink[aa][0] == bb ) tKid.fLink[aa][0] = a1;
        else tKid.fLink[aa][1] = a1;
        if( tKid.fLink[bb][0] == aa ) tKid.fLink[bb][0] = b1;
        else tKid.fLink[bb][1] = b1;
        if( tKid.fLink[a1][0] == b1 ) tKid.fLink[a1][0] = aa;
        else tKid.fLink[a1][1] = aa;
        if( tKid.fLink[b1][0] == a1 ) tKid.fLink[b1][0] = bb;
        else tKid.fLink[b1][1] = bb;

        // 记录修改的边
        fModiEdge[ fNumOfModiEdge ][ 0 ] = aa;
        fModiEdge[ fNumOfModiEdge ][ 1 ] = bb;
        fModiEdge[ fNumOfModiEdge ][ 2 ] = a1;
        fModiEdge[ fNumOfModiEdge ][ 3 ] = b1;
        ++fNumOfModiEdge;

        fGainModi += max_diff;  // 累加改进值

        // 更新单元信息
        int posi_a1 = fInv[ a1 ];
        select_un = -1;
        // 找出a1所在的单元
        for( int s = 0; s < fNumOfSeg; ++s )
            if( fSegment[ s ][ 0 ] <= posi_a1 && posi_a1 <=  fSegment[ s ][ 1 ] ){
                select_un = fSegUnit[ s ];
                break;
            }

        // 合并单元
        for( int s = 0; s < fNumOfSeg; ++s )
            if( fSegUnit[ s ] == select_un ) 
                fSegUnit[ s ] = center_un;

        // 更新单元中的元素数量
        fNumOfElementInUnit[ center_un ] += fNumOfElementInUnit[ select_un ];

        // 处理最后一个单元
        for( int s = 0; s < fNumOfSeg; ++s )
            if( fSegUnit[ s ] == fNumOfUnit - 1 ) 
                fSegUnit[ s ] = select_un;

        fNumOfElementInUnit[ select_un ] = fNumOfElementInUnit[ fNumOfUnit - 1 ];
        --fNumOfUnit;

        // 清除中心单元标记
        for( int s = 0; s < fNumOfElementInCU; ++s ){
            c = fListOfCenterUnit[ s ];
            fCenterUnit[ c ] = 0;
        }
    }
}

// 构建单元的函数
void TCross::makeUnit(){
    // 检查是否需要添加首位置到分段列表
    int flag = 1;
    for( int s = 0; s < fNumOfSPL; ++s ){
        if( fSegPosiList[ s ] == 0 ){  // 如果0已经在列表中
            flag = 0;
            break;
        }
    }
    // 如果0不在列表中，添加它并更新相关连接
    if( flag == 1 ){
        fSegPosiList[ fNumOfSPL++ ] = 0;  // 添加0到列表末尾
        // 更新首尾位置的连接关系
        LinkBPosi[ fN-1 ][ 1 ] = LinkBPosi[ fN-1 ][ 0 ];
        LinkBPosi[ 0 ][ 1 ] = LinkBPosi[ 0 ][ 0 ];
        LinkBPosi[ fN-1 ][ 0 ] = 0;
        LinkBPosi[ 0 ][ 0 ] = fN-1;
    }

    // 对分段位置列表进行排序
    tSort->sort( fSegPosiList, fNumOfSPL );
    fNumOfSeg = fNumOfSPL;
    
    // 根据排序后的位置构建分段信息
    for( int s = 0; s < fNumOfSeg-1; ++s ){
        fSegment[ s ][ 0 ] = fSegPosiList[ s ];     // 分段起始位置
        fSegment[ s ][ 1 ] = fSegPosiList[ s+1 ]-1; // 分段结束位置
    }
    // 处理最后一个分段
    fSegment[ fNumOfSeg-1 ][ 0 ] = fSegPosiList[ fNumOfSeg-1 ];
    fSegment[ fNumOfSeg-1 ][ 1 ] = fN - 1;

    // 为每个分段建立连接关系和位置映射
    for( int s = 0; s < fNumOfSeg; ++s ){
        // 分段首尾互相连接
        LinkAPosi[ fSegment[ s ][ 0 ] ] = fSegment[ s ][ 1 ];
        LinkAPosi[ fSegment[ s ][ 1 ] ] = fSegment[ s ][ 0 ];
        // 记录位置所属的分段
        fPosiSeg[ fSegment[ s ][ 0 ] ] = s;
        fPosiSeg[ fSegment[ s ][ 1 ] ] = s;
    }

    // 初始化分段单元信息
    for( int s = 0; s < fNumOfSeg; ++s ) fSegUnit[ s ] = -1;
    fNumOfUnit = 0;  // 重置单元数量

    // 变量声明
    int p_st, p1, p2, p_next, p_pre;  // 用于遍历的位置指针
    int segNum;                        // 分段编号

    // 构建单元
    while(1){
        // 寻找未分配单元的分段
        flag = 0;
        for( int s = 0; s < fNumOfSeg; ++s ){
            if( fSegUnit[ s ] == -1 ){  // 找到未分配的分段
                p_st = fSegment[ s ][ 0 ];  // 起始位置
                p_pre = -1;                 // 前一个位置
                p1 = p_st;                  // 当前位置
                flag = 1;
                break;
            }
        }
        if( flag == 0 ) break;  // 所有分段都已分配完毕

        // 跟踪连接构建单元
        while(1){
            segNum = fPosiSeg[ p1 ];           // 获取当前位置的分段编号
            fSegUnit[ segNum ] = fNumOfUnit;   // 将分段分配给当前单元

            p2 = LinkAPosi[ p1 ];              // 获取连接的另一端
            p_next = LinkBPosi[ p2 ][ 0 ];     // 获取下一个位置
            // 特殊情况处理：当前位置与连接位置相同时
            if( p1 == p2 )
                if( p_next == p_pre ) p_next = LinkBPosi[ p2 ][ 1 ];

            // 如果回到起点，完成一个单元
            if( p_next == p_st ){
                ++fNumOfUnit;
                break;
            }

            // 移动到下一个位置
            p_pre = p2;
            p1 = p_next;
        }
    }

    // 初始化每个单元中的元素数量
    for( int s = 0; s < fNumOfUnit; ++s ) fNumOfElementInUnit[ s ] = 0;

    // 整理分段信息，合并相邻的相同单元的分段
    int unitNum = -1;
    int tmpNumOfSeg = -1;
    for( int s = 0; s < fNumOfSeg; ++s ){
        if( fSegUnit[ s ] != unitNum ){  // 新单元的开始
            ++tmpNumOfSeg;
            // 记录新分段的信息
            fSegment[ tmpNumOfSeg ][ 0 ] = fSegment[ s ][ 0 ];
            fSegment[ tmpNumOfSeg ][ 1 ] = fSegment[ s ][ 1 ];
            unitNum = fSegUnit[ s ];
            fSegUnit[ tmpNumOfSeg ] = unitNum;
            // 累加元素数量
            fNumOfElementInUnit[ unitNum ] +=
                fSegment[ s ][ 1 ] - fSegment[ s ][ 0 ] + 1;
        }
        else{  // 同一单元的连续分段
            // 扩展当前分段的结束位置
            fSegment[ tmpNumOfSeg ][ 1 ] = fSegment[ s ][ 1 ];
            // 累加元素数量
            fNumOfElementInUnit[ unitNum ] +=
                fSegment[ s ][ 1 ] - fSegment[ s ][ 0 ] + 1;
        }
    }
    // 更新最终的分段数量
    fNumOfSeg = tmpNumOfSeg + 1;
}

// 将解恢复到父代1的状态
void TCross::backToPa1( TIndi& tKid ){
    int aa, bb, a1, b1;  // 存储需要修改连接的节点
    int jnum;            // AB循环编号

    // 逆序撤销之前的边修改
    for( int s = fNumOfModiEdge - 1; s >= 0; --s ){
        aa = fModiEdge[ s ][ 0 ];
        a1 = fModiEdge[ s ][ 1 ];
        bb = fModiEdge[ s ][ 2 ];
        b1 = fModiEdge[ s ][ 3 ];

        // 恢复原始连接关系
        if( tKid.fLink[aa][0] == bb ) tKid.fLink[aa][0] = a1;
        else tKid.fLink[aa][1] = a1;
        if( tKid.fLink[b1][0] == a1 ) tKid.fLink[b1][0] = bb;
        else tKid.fLink[b1][1] = bb;
        if( tKid.fLink[bb][0] == aa ) tKid.fLink[bb][0] = b1;
        else tKid.fLink[bb][1] = b1;
        if( tKid.fLink[a1][0] == b1 ) tKid.fLink[a1][0] = aa;
        else tKid.fLink[a1][1] = aa;
    }

    // 反向应用所有AB循环的修改
    for( int s = 0; s < fNumOfAppliedCycle; ++s ){
        jnum = fAppliedCylce[ s ];
        this->changeSol( tKid, jnum, 2 );  // 类型2表示反向应用
    }
}

// 将解更新到找到的最优状态
void TCross::goToBest( TIndi& tKid ){
    int aa, bb, a1, b1;  // 存储需要修改连接的节点
    int jnum;            // AB循环编号

    // 应用最优的AB循环修改
    for( int s = 0; s < fNumOfBestAppliedCycle; ++s ){
        jnum = fBestAppliedCylce[ s ];
        this->changeSol( tKid, jnum, 1 );  // 类型1表示正向应用
    }

    // 应用最优的边修改
    for( int s = 0; s < fNumOfBestModiEdge; ++s ){
        aa = fBestModiEdge[ s ][ 0 ];
        bb = fBestModiEdge[ s ][ 1 ];
        a1 = fBestModiEdge[ s ][ 2 ];
        b1 = fBestModiEdge[ s ][ 3 ];

        // 更新连接关系
        if( tKid.fLink[aa][0] == bb ) tKid.fLink[aa][0] = a1;
        else tKid.fLink[aa][1] = a1;
        if( tKid.fLink[bb][0] == aa ) tKid.fLink[bb][0] = b1;
        else tKid.fLink[bb][1] = b1;
        if( tKid.fLink[a1][0] == b1 ) tKid.fLink[a1][0] = aa;
        else tKid.fLink[a1][1] = aa;
        if( tKid.fLink[b1][0] == a1 ) tKid.fLink[b1][0] = bb;
        else tKid.fLink[b1][1] = bb;
    }
}

// 更新边的使用频率
void TCross::incrementEdgeFreq( int **fEdgeFreq ){
    int j, jnum, cem;         // 循环变量和AB循环信息
    int r1, r2, b1, b2;       // 存储需要更新频率的边的端点
    int aa, bb, a1;           // 存储修改边的端点

    // 处理最优AB循环中的边频率更新
    for( int s = 0; s < fNumOfBestAppliedCycle; ++s ){
        jnum = fBestAppliedCylce[ s ];

        // 获取AB循环信息
        cem = fABcycle[ jnum ][ 0 ];
        fC[ 0 ] = fABcycle[ jnum ][ 0 ];

        // 复制AB循环数据
        for( j = 1; j <= cem+3; ++j )
            fC[ j ] = fABcycle[ jnum ][ j ];

        // 更新循环中每对边的频率
        for( j = 0; j < cem/2; ++j ){
            r1 = fC[2+2*j]; r2 = fC[3+2*j];
            b1 = fC[1+2*j]; b2 = fC[4+2*j];

            // 增加新添加的边的频率
            ++fEdgeFreq[ r1 ][ b1 ];
            ++fEdgeFreq[ r2 ][ b2 ];
            
            // 减少删除的边的频率
            --fEdgeFreq[ r1 ][ r2 ];
            --fEdgeFreq[ r2 ][ r1 ];
        }
    }

    // 更新最优修改边的频率
    for( int s = 0; s < fNumOfBestModiEdge; ++s ){
        aa = fBestModiEdge[ s ][ 0 ];
        bb = fBestModiEdge[ s ][ 1 ];
        a1 = fBestModiEdge[ s ][ 2 ];
        b1 = fBestModiEdge[ s ][ 3 ];

        // 减少原始边的频率（双向）
        --fEdgeFreq[ aa ][ bb ];
        --fEdgeFreq[ bb ][ aa ];
        --fEdgeFreq[ a1 ][ b1 ];
        --fEdgeFreq[ b1 ][ a1 ];

        // 增加新边的频率（双向）
        ++fEdgeFreq[ aa ][ a1 ];
        ++fEdgeFreq[ a1 ][ aa ];
        ++fEdgeFreq[ bb ][ b1 ];
        ++fEdgeFreq[ b1 ][ bb ];
    }
}

// 计算自适应损失值的函数
// fEdgeFreq: 边的使用频率矩阵
int TCross::calAdpLoss( int **fEdgeFreq )
{
    int j, jnum, cem;          // 循环变量和AB循环信息
    int r1, r2, b1, b2;        // 存储AB循环中的节点
    int aa, bb, a1;            // 存储修改边的节点
    double DLoss;              // 损失值

    DLoss = 0;  // 初始化损失值

    // 第一阶段：计算AB循环带来的损失
    for( int s = 0; s < fNumOfAppliedCycle; ++s )
    {
        jnum = fAppliedCylce[ s ];  // 获取当前AB循环编号

        // 获取AB循环信息
        cem = fABcycle[ jnum ][ 0 ];
        fC[ 0 ] = fABcycle[ jnum ][ 0 ];

        // 复制AB循环数据
        for( j = 1; j <= cem+3; ++j ) 
            fC[ j ] = fABcycle[ jnum ][ j ];

        // 处理循环中的每对边
        for( j = 0; j < cem/2; ++j ){
            // 获取要处理的四个节点
            r1 = fC[2+2*j]; r2 = fC[3+2*j];
            b1 = fC[1+2*j]; b2 = fC[4+2*j];

            // 计算损失值：
            // 移除边时，减去(频率-1)，因为边被移除后频率会减1
            DLoss -= (fEdgeFreq[ r1 ][ r2 ]-1);  // r1-r2边的损失
            DLoss -= (fEdgeFreq[ r2 ][ r1 ]-1);  // r2-r1边的损失（考虑双向）
            // 添加边时，加上当前频率
            DLoss += fEdgeFreq[ r2 ][ b2 ];      // r2-b2边的贡献
            DLoss += fEdgeFreq[ b2 ][ r2 ];      // b2-r2边的贡献（考虑双向）

            // 更新边的频率
            // 移除原有边
            --fEdgeFreq[ r1 ][ r2 ];
            --fEdgeFreq[ r2 ][ r1 ];
            // 添加新边
            ++fEdgeFreq[ r2 ][ b2 ];
            ++fEdgeFreq[ b2 ][ r2 ];
        }
    }

    // 第二阶段：计算修改边带来的损失
    for( int s = 0; s < fNumOfModiEdge; ++s )
    {
        // 获取要修改的边的节点
        aa = fModiEdge[ s ][ 0 ];
        bb = fModiEdge[ s ][ 1 ];
        a1 = fModiEdge[ s ][ 2 ];
        b1 = fModiEdge[ s ][ 3 ];

        // 计算移除原有边的损失
        DLoss -= (fEdgeFreq[ aa ][ bb ]-1);
        DLoss -= (fEdgeFreq[ bb ][ aa ]-1);
        DLoss -= (fEdgeFreq[ a1 ][ b1 ]-1);
        DLoss -= (fEdgeFreq[ b1 ][ a1 ]-1);

        // 计算添加新边的贡献
        DLoss += fEdgeFreq[ aa ][ a1 ];
        DLoss += fEdgeFreq[ a1 ][ aa ];
        DLoss += fEdgeFreq[ bb ][ b1 ];
        DLoss += fEdgeFreq[ b1 ][ bb ];

        // 更新边的频率
        // 移除原有边
        --fEdgeFreq[ aa ][ bb ];
        --fEdgeFreq[ bb ][ aa ];
        --fEdgeFreq[ a1 ][ b1 ];
        --fEdgeFreq[ b1 ][ a1 ];

        // 添加新边
        ++fEdgeFreq[ aa ][ a1 ];
        ++fEdgeFreq[ a1 ][ aa ];
        ++fEdgeFreq[ bb ][ b1 ];
        ++fEdgeFreq[ b1 ][ bb ];
    }

    // 第三阶段：恢复边的频率状态
    // 恢复AB循环中的边频率
    for( int s = 0; s < fNumOfAppliedCycle; ++s ){
        jnum = fAppliedCylce[ s ];
        cem = fABcycle[ jnum ][ 0 ];
        fC[ 0 ] = fABcycle[ jnum ][ 0 ];
        for( j = 1; j <= cem+3; ++j ) 
            fC[ j ] = fABcycle[ jnum ][ j ];

        for( j = 0; j < cem/2; ++j ){
            r1 = fC[2+2*j]; r2 = fC[3+2*j];
            b1 = fC[1+2*j]; b2 = fC[4+2*j];

            // 恢复原始边的频率
            ++fEdgeFreq[ r1 ][ r2 ];
            ++fEdgeFreq[ r2 ][ r1 ];
            // 撤销新边的频率
            --fEdgeFreq[ r2 ][ b2 ];
            --fEdgeFreq[ b2 ][ r2 ];
        }
    }

    // 恢复修改边的频率
    for( int s = 0; s < fNumOfModiEdge; ++s )
    {
        aa = fModiEdge[ s ][ 0 ];
        bb = fModiEdge[ s ][ 1 ];
        a1 = fModiEdge[ s ][ 2 ];
        b1 = fModiEdge[ s ][ 3 ];

        // 恢复原有边的频率
        ++fEdgeFreq[ aa ][ bb ];
        ++fEdgeFreq[ bb ][ aa ];
        ++fEdgeFreq[ a1 ][ b1 ];
        ++fEdgeFreq[ b1 ][ a1 ];

        // 撤销新边的频率
        --fEdgeFreq[ aa ][ a1 ];
        --fEdgeFreq[ a1 ][ aa ];
        --fEdgeFreq[ bb ][ b1 ];
        --fEdgeFreq[ b1 ][ bb ];
    }

    // 返回最终的损失值（除以2是因为每条边被计算了两次，考虑了双向）
    return int(DLoss / 2);
}

// 计算熵损失值的函数
// fEdgeFreq: 边的使用频率矩阵
// 返回值：基于信息熵的损失值
double TCross::calEntLoss( int **fEdgeFreq ){
    int j, jnum, cem;           // 循环变量和AB循环信息
    int r1, r2, b1, b2;         // AB循环中的节点
    int aa, bb, a1;             // 修改边的节点
    double DLoss;               // 总的熵损失值
    double h1, h2;              // 用于计算熵的概率值

    DLoss = 0;  // 初始化损失值

    // 第一阶段：处理AB循环带来的熵变化
    for( int s = 0; s < fNumOfAppliedCycle; ++s ){
        jnum = fAppliedCylce[ s ];
        cem = fABcycle[ jnum ][ 0 ];
        fC[ 0 ] = fABcycle[ jnum ][ 0 ];

        // 复制AB循环数据
        for( j = 1; j <= cem+3; ++j ) 
            fC[ j ] = fABcycle[ jnum ][ j ];

        // 处理每对边的熵变化
        for( j = 0; j < cem/2; ++j )
        {
            r1 = fC[2+2*j]; r2 = fC[3+2*j];
            b1 = fC[1+2*j]; b2 = fC[4+2*j];

            // 计算移除边导致的熵变化
            // h1: 移除后的概率, h2: 移除前的概率
            h1 = (double)(fEdgeFreq[r1][r2] - 1) / (double)Npop;
            h2 = (double)(fEdgeFreq[r1][r2]) / (double)Npop;
            // 仅当频率大于0时计算熵
            if (fEdgeFreq[r1][r2] - 1 != 0)
                DLoss -= h1 * log(h1);  // 移除后的熵
            DLoss += h2 * log(h2);      // 移除前的熵
            // 更新频率
            --fEdgeFreq[r1][r2];
            --fEdgeFreq[r2][r1];

            // 计算添加边导致的熵变化
            // h1: 添加后的概率, h2: 添加前的概率
            h1 = (double)(fEdgeFreq[r2][b2] + 1) / (double)Npop;
            h2 = (double)(fEdgeFreq[r2][b2]) / (double)Npop;
            DLoss -= h1 * log(h1);      // 添加后的熵
            if (fEdgeFreq[r2][b2] != 0)
                DLoss += h2 * log(h2);   // 添加前的熵（如果边已存在）
            // 更新频率
            ++fEdgeFreq[r2][b2];
            ++fEdgeFreq[b2][r2];
        }
    }

    // 第二阶段：处理修改边带来的熵变化
    for( int s = 0; s < fNumOfModiEdge; ++s ){
        aa = fModiEdge[ s ][ 0 ];
        bb = fModiEdge[ s ][ 1 ];
        a1 = fModiEdge[ s ][ 2 ];
        b1 = fModiEdge[ s ][ 3 ];

        // 处理第一条要移除的边
        h1 = (double)( fEdgeFreq[ aa ][ bb ] - 1 )/(double)Npop;
        h2 = (double)( fEdgeFreq[ aa ][ bb ] )/(double)Npop;
        if( fEdgeFreq[ aa ][ bb ] - 1 != 0 )
            DLoss -= h1 * log( h1 );
        DLoss += h2 * log( h2 );
        --fEdgeFreq[ aa ][ bb ];
        --fEdgeFreq[ bb ][ aa ];

        // 处理第二条要移除的边
        h1 = (double)( fEdgeFreq[ a1 ][ b1 ] - 1 )/(double)Npop;
        h2 = (double)( fEdgeFreq[ a1 ][ b1 ] )/(double)Npop;
        if( fEdgeFreq[ a1 ][ b1 ] - 1 != 0 )
            DLoss -= h1 * log( h1 );
        DLoss += h2 * log( h2 );
        --fEdgeFreq[ a1 ][ b1 ];
        --fEdgeFreq[ b1 ][ a1 ];

        // 处理第一条要添加的边
        h1 = (double)( fEdgeFreq[ aa ][ a1 ] + 1 )/(double)Npop;
        h2 = (double)( fEdgeFreq[ aa ][ a1 ])/(double)Npop;
        DLoss -= h1 * log( h1 );
        if( fEdgeFreq[ aa ][ a1 ] != 0 )
            DLoss += h2 * log( h2 );
        ++fEdgeFreq[ aa ][ a1 ];
        ++fEdgeFreq[ a1 ][ aa ];

        // 处理第二条要添加的边
        h1 = (double)( fEdgeFreq[ bb ][ b1 ] + 1 )/(double)Npop;
        h2 = (double)( fEdgeFreq[ bb ][ b1 ])/(double)Npop;
        DLoss -= h1 * log( h1 );
        if( fEdgeFreq[ bb ][ b1 ] != 0 )
            DLoss += h2 * log( h2 );
        ++fEdgeFreq[ bb ][ b1 ];
        ++fEdgeFreq[ b1 ][ bb ];
    }
    DLoss = -DLoss;  // 反转损失值

    // 第三阶段：恢复边的频率状态
    // 恢复AB循环中的边频率
    for( int s = 0; s < fNumOfAppliedCycle; ++s ){
        jnum = fAppliedCylce[ s ];
        cem = fABcycle[ jnum ][ 0 ];
        fC[ 0 ] = fABcycle[ jnum ][ 0 ];

        for( j = 1; j <= cem+3; ++j ) 
            fC[ j ] = fABcycle[ jnum ][ j ];

        for( j = 0; j < cem/2; ++j ){
            r1 = fC[2+2*j]; r2 = fC[3+2*j];
            b1 = fC[1+2*j]; b2 = fC[4+2*j];

            // 恢复边的频率
            ++fEdgeFreq[ r1 ][ r2 ];
            ++fEdgeFreq[ r2 ][ r1 ];
            --fEdgeFreq[ r2 ][ b2 ];
            --fEdgeFreq[ b2 ][ r2 ];
        }
    }

    // 恢复修改边的频率
    for( int s = 0; s < fNumOfModiEdge; ++s ){
        aa = fModiEdge[ s ][ 0 ];
        bb = fModiEdge[ s ][ 1 ];
        a1 = fModiEdge[ s ][ 2 ];
        b1 = fModiEdge[ s ][ 3 ];

        // 恢复原有边的频率
        ++fEdgeFreq[ aa ][ bb ];
        ++fEdgeFreq[ bb ][ aa ];
        ++fEdgeFreq[ a1 ][ b1 ];
        ++fEdgeFreq[ b1 ][ a1 ];

        // 撤销新边的频率
        --fEdgeFreq[ aa ][ a1 ];
        --fEdgeFreq[ a1 ][ aa ];
        --fEdgeFreq[ bb ][ b1 ];
        --fEdgeFreq[ b1 ][ bb ];
    }
    return DLoss;  // 返回最终的熵损失值
}

// 设置AB循环和节点之间的权重关系
// tPa1, tPa2: 父代个体
void TCross::setWeight( const TIndi& tPa1, const TIndi& tPa2 ){
    int cem;                // AB循环中的边数
    int r1, r2, v1, v2, v_p;  // 用于存储节点编号
    int AB_num;            // AB循环编号

    // 初始化节点的影响关系数组
    for( int i = 0; i < fN; ++i ){
        fInEffectNode[ i ][ 0 ] = -1;  // 第一个影响的AB循环
        fInEffectNode[ i ][ 1 ] = -1;  // 第二个影响的AB循环
    }

    // 步骤1：标记红边所连接的节点与AB循环的关系
    for( int s = 0; s < fNumOfABcycle; ++s ){
        cem = fABcycle[ s ][ 0 ];  // 获取当前AB循环的边数
        // 处理每一对红边
        for( int j = 0; j < cem/2; ++j ){
            r1 = fABcycle[ s ][ 2*j+2 ];  // 红边的第一个节点
            r2 = fABcycle[ s ][ 2*j+3 ];  // 红边的第二个节点

            // 为r1节点添加AB循环关系
            if( fInEffectNode[ r1 ][ 0 ] == -1 ) 
                fInEffectNode[ r1 ][ 0 ] = s;
            else if ( fInEffectNode[ r1 ][ 1 ] == -1 ) 
                fInEffectNode[ r1 ][ 1 ] = s;

            // 为r2节点添加AB循环关系
            if( fInEffectNode[ r2 ][ 0 ] == -1 ) 
                fInEffectNode[ r2 ][ 0 ] = s;
            else if ( fInEffectNode[ r2 ][ 1 ] == -1 ) 
                fInEffectNode[ r2 ][ 1 ] = s;
        }
    }

    // 步骤2：处理只与一个AB循环相关的节点
    for( int i = 0; i < fN; ++i ){
        if( fInEffectNode[ i ][ 0 ] != -1 && fInEffectNode[ i ][ 1 ] == -1 ){
            AB_num = fInEffectNode[ i ][ 0 ];  // 获取相关的AB循环编号
            v1 = i;

            // 找到父代1中与父代2不同的连接点
            if( tPa1.fLink[ v1 ][ 0 ] != tPa2.fLink[ v1 ][ 0 ] && 
                tPa1.fLink[ v1 ][ 0 ] != tPa2.fLink[ v1 ][ 1 ] )
                v_p = tPa1.fLink[ v1 ][ 0 ];
            else if( tPa1.fLink[ v1 ][ 1 ] != tPa2.fLink[ v1 ][ 0 ] && 
                     tPa1.fLink[ v1 ][ 1 ] != tPa2.fLink[ v1 ][ 1 ] )
                v_p = tPa1.fLink[ v1 ][ 1 ];

            // 沿着路径继续寻找相关节点
            while( 1 ){
                fInEffectNode[ v1 ][ 1 ] = AB_num;  // 添加第二个AB循环关系

                // 找到下一个要处理的节点
                if( tPa1.fLink[ v1 ][ 0 ] != v_p ) 
                    v2 = tPa1.fLink[ v1 ][ 0 ];
                else if( tPa1.fLink[ v1 ][ 1 ] != v_p ) 
                    v2 = tPa1.fLink[ v1 ][ 1 ];

                // 为新节点添加AB循环关系
                if( fInEffectNode[ v2 ][ 0 ] == -1 ) 
                    fInEffectNode[ v2 ][ 0 ] = AB_num;
                else if( fInEffectNode[ v2 ][ 1 ] == -1 ) 
                    fInEffectNode[ v2 ][ 1 ] = AB_num;

                // 如果新节点已经有两个AB循环关系，则停止
                if( fInEffectNode[ v2 ][ 1 ] != -1 ) break;
                v_p = v1;
                v1 = v2;
            }
        }
    }

    // 步骤3：计算AB循环之间的权重关系
    // 初始化权重数组
    for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ){
        fWeightC[ s1 ] = 0;  // 清零单个AB循环的权重
        for( int s2 = 0; s2 < fNumOfABcycle; ++s2 ) 
            fWeightRR[ s1 ][ s2 ] = 0;  // 清零AB循环对之间的权重
    }

    // 计算权重
    for( int i = 0; i < fN; ++i ){
        // 如果节点与两个AB循环相关
        if( fInEffectNode[ i ][ 0 ] != -1 && fInEffectNode[ i ][ 1 ] != -1 ){
            // 增加AB循环对之间的权重
            ++fWeightRR[ fInEffectNode[ i ][ 0 ] ][ fInEffectNode[ i ][ 1 ] ];
            ++fWeightRR[ fInEffectNode[ i ][ 1 ] ][ fInEffectNode[ i ][ 0 ] ];
        }
        // 如果节点连接的两个AB循环不同
        if( fInEffectNode[ i ][ 0 ] != fInEffectNode[ i ][ 1 ] ){
            // 增加单个AB循环的权重
            ++fWeightC[ fInEffectNode[ i ][ 0 ] ];
            ++fWeightC[ fInEffectNode[ i ][ 1 ] ];
        }
    }
    // 清除AB循环与自身的权重关系
    for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ) 
        fWeightRR[ s1 ][ s1 ] = 0;
}

// 计算简单C值（用于评估AB循环的效果）
int TCross::calCNaive(){
    int count_C;   // C值计数
    int tt;        // 临时变量

    count_C = 0;

    // 遍历所有节点
    for( int i = 0; i < fN; ++i ){
        // 如果节点与两个AB循环相关
        if( fInEffectNode[ i ][ 0 ] != -1 && fInEffectNode[ i ][ 1 ] != -1 ){
            tt = 0;
            // 检查相关AB循环是否被使用
            if( fUsedAB[ fInEffectNode[ i ][ 0 ] ] == 1 ) ++tt;
            if( fUsedAB[ fInEffectNode[ i ][ 1 ] ] == 1 ) ++tt;
            // 如果只有一个相关的AB循环被使用，增加C值
            if( tt == 1 ) ++count_C;
        }
    }
    return count_C;  // 返回最终的C值
}

// AB循环集合的搜索函数
// centerAB: 中心AB循环的编号
void TCross::searchEset( int centerAB ){
    int nIter;              // 当前迭代次数
    int stagImp;           // 未改进的迭代次数
    int delta_weight;      // 权重变化量
    int min_delta_weight_nt;  // 非禁忌移动的最小权重变化
    int flag_AddDelete;      // 改进移动的标志（1:添加, -1:删除, 0:无）
    int flag_AddDelete_nt;   // 非禁忌移动的标志
    int selected_AB;         // 选中的改进AB循环
    int selected_AB_nt;      // 选中的非禁忌AB循环
    int jnum;                // 临时变量，存储AB循环编号

    // 初始化计数器
    fNumC = 0;  // E-set中C节点的数量
    fNumE = 0;  // E-set中边的数量

    // 初始化AB循环的使用状态
    fNumOfUsedAB = 0;
    for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ){
        fUsedAB[ s1 ] = 0;      // 标记为未使用
        fWeightSR[ s1 ] = 0;    // 清零SR权重
        fMovedAB[ s1 ] = 0;     // 清零移动时间
    }

    // 添加初始的AB循环集合
    for( int s = 0; s < fNumOfABcycleInEset; ++s ){
        jnum = fABcycleInEset[ s ];
        this->addAB( jnum );
    }
    // 记录初始状态
    fBestNumC = fNumC;
    fBestNumE = fNumE;

    // 搜索主循环
    stagImp = 0;   // 重置停滞计数器
    nIter = 0;     // 重置迭代计数器
    while( 1 ){
        ++nIter;   // 增加迭代次数
        min_delta_weight_nt = 99999999;  // 初始化最小非禁忌权重变化
        flag_AddDelete = 0;      // 重置改进移动标志
        flag_AddDelete_nt = 0;   // 重置非禁忌移动标志

        // 遍历所有AB循环寻找可能的移动
        for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ){
            // 检查添加未使用的AB循环
            if( fUsedAB[ s1 ] == 0 && fWeightSR[ s1 ] > 0 ){
                // 计算添加后的权重变化
                delta_weight = fWeightC[ s1 ] - 2 * fWeightSR[ s1 ];
                // 如果能改进最优解
                if( fNumC + delta_weight < fBestNumC ){
                    selected_AB = s1;
                    flag_AddDelete = 1;
                    fBestNumC = fNumC + delta_weight;
                }
                // 更新非禁忌最优移动
                if( delta_weight < min_delta_weight_nt && nIter > fMovedAB[ s1 ] ){
                    selected_AB_nt = s1;
                    flag_AddDelete_nt = 1;
                    min_delta_weight_nt = delta_weight;
                }
            }
            // 检查删除已使用的AB循环（除中心循环外）
            else if( fUsedAB[ s1 ] == 1 && s1 != centerAB ){
                // 计算删除后的权重变化
                delta_weight = - fWeightC[ s1 ] + 2 * fWeightSR[ s1 ];
                // 如果能改进最优解
                if( fNumC + delta_weight < fBestNumC ){
                    selected_AB = s1;
                    flag_AddDelete = -1;
                    fBestNumC = fNumC + delta_weight;
                }
                // 更新非禁忌最优移动
                if( delta_weight < min_delta_weight_nt && nIter > fMovedAB[ s1 ] ){
                    selected_AB_nt = s1;
                    flag_AddDelete_nt = -1;
                    min_delta_weight_nt = delta_weight;
                }
            }
        }

        // 如果找到改进移动
        if( flag_AddDelete != 0 ){
            // 执行添加或删除操作
            if( flag_AddDelete == 1 ) 
                this->addAB( selected_AB );
            else if( flag_AddDelete == -1 ) 
                this->deleteAB( selected_AB );

            // 更新移动时间（添加随机禁忌期）
            fMovedAB[ selected_AB ] = nIter + tRand->Integer( 1, fTmax );

            fBestNumE = fNumE;

            // 更新E-set中的AB循环
            fNumOfABcycleInEset = 0;
            for( int s1 = 0; s1 < fNumOfABcycle; ++s1 )
                if( fUsedAB[ s1 ] == 1 ) 
                    fABcycleInEset[ fNumOfABcycleInEset++ ] = s1;

            stagImp = 0;  // 重置停滞计数器
        }
        // 如果找到非禁忌移动
        else if( flag_AddDelete_nt != 0 ) {
            // 执行非禁忌的添加或删除操作
            if( flag_AddDelete_nt == 1 ) 
                this->addAB( selected_AB_nt );
            else if( flag_AddDelete_nt == -1 )
                this->deleteAB( selected_AB_nt );
            
            // 更新移动时间
            fMovedAB[ selected_AB_nt ] = nIter + tRand->Integer( 1, fTmax );
        }

        // 更新停滞计数器
        if( flag_AddDelete == 0 ) ++stagImp;
        // 如果达到最大停滞次数则终止搜索
        if( stagImp == fMaxStag ) break;
    }
}

// 添加AB循环到E-set中
// num: 要添加的AB循环编号
void TCross::addAB( int num ){
    // 更新C节点数量
    // fWeightC[num]: AB循环num的总权重
    // fWeightSR[num]: AB循环num与已使用AB循环的关联权重
    // 减去2倍关联权重是因为关联节点会被计算两次
    fNumC += fWeightC[ num ] - 2 * fWeightSR[ num ];
    
    // 更新边的数量
    // fABcycle[num][0]存储了AB循环中的边数
    // 除以2是因为每个AB循环包含的是边对
    fNumE += fABcycle[ num ][ 0 ] / 2;

    // 标记AB循环为已使用
    fUsedAB[ num ] = 1;
    ++fNumOfUsedAB;  // 增加已使用的AB循环计数

    // 更新其他AB循环的SR权重
    // fWeightRR[s1][num]表示AB循环s1和num之间的关联权重
    for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ) 
        fWeightSR[ s1 ] += fWeightRR[ s1 ][ num ];
}

// 从E-set中删除AB循环
// num: 要删除的AB循环编号
void TCross::deleteAB( int num ){
    // 更新C节点数量（与添加操作相反）
    fNumC -= fWeightC[ num ] - 2 * fWeightSR[ num ];
    
    // 更新边的数量（与添加操作相反）
    fNumE -= fABcycle[ num ][ 0 ] / 2;

    // 标记AB循环为未使用
    fUsedAB[ num ] = 0;
    --fNumOfUsedAB;  // 减少已使用的AB循环计数

    // 更新其他AB循环的SR权重（与添加操作相反）
    for( int s1 = 0; s1 < fNumOfABcycle; ++s1 ) 
        fWeightSR[ s1 ] -= fWeightRR[ s1 ][ num ];
}