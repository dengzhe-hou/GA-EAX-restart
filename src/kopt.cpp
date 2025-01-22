#ifndef __KOPT__
#include "kopt.h"
#endif

// K优化类构造函数
TKopt::TKopt( int N )
{
    fN = N;  // 城市数量
    
    // 分配并初始化链接数组（存储每个城市的相邻城市）
    fLink = new int* [ fN ];
    for( int i = 0; i < fN; ++i ) fLink[ i ] = new int [ 2 ];

    // 分配各种辅助数组的内存
    fOrdCity = new int [ fN ];     // 城市在段中的顺序
    fOrdSeg = new int [ fN ];      // 段的顺序
    fSegCity = new int [ fN ];     // 每个城市所属的段
    fOrient = new int [ fN ];      // 段的方向
    
    // 段之间的链接关系
    fLinkSeg = new int* [ fN ];
    for( int i = 0; i < fN; ++i ) fLinkSeg[ i ] = new int [ 2 ];

    // 段的大小和城市信息
    fSizeSeg = new int [ fN ];     // 每个段的大小
    fCitySeg = new int* [ fN ];    // 段的起始和结束城市
    for( int i = 0; i < fN; ++i ) fCitySeg[ i ] = new int [ 2 ];

    // 其他辅助数组
    fT = new int [ 5 ];            // 临时数组
    fActiveV = new int [ fN ];     // 活跃顶点标记
    // 反向近邻列表
    fInvNearList = new int* [ fN ];
    for( int i = 0; i < fN; ++i ) fInvNearList[ i ] = new int [ 500 ];

    fNumOfINL = new int [ fN ];    // 每个城市的反向近邻数量
    fArray = new int [ fN+2 ];     // 临时数组（带额外空间）
    fCheckN = new int [ fN ];      // 检查数组
    fB = new int [ fN ];           // 临时数组
    fGene = new int [ fN ];        // 基因数组
}

// 析构函数：释放所有动态分配的内存
TKopt::~TKopt()
{
    // 释放二维数组
    for( int i = 0; i < fN; ++i ) delete [] fLink[ i ];
    delete [] fLink;

    for( int i = 0; i < fN; ++i ) delete [] fLinkSeg[ i ];
    delete [] fLinkSeg;

    for( int i = 0; i < fN; ++i ) delete [] fCitySeg[ i ];
    delete [] fCitySeg;

    for( int i = 0; i < fN; ++i ) delete [] fInvNearList[ i ];
    delete [] fInvNearList;

    // 释放一维数组
    delete [] fOrdCity;
    delete [] fOrdSeg;
    delete [] fSegCity;
    delete [] fOrient;
    delete [] fSizeSeg;
    delete [] fT;
    delete [] fActiveV;
    delete [] fNumOfINL;
    delete [] fArray;
    delete [] fCheckN;
    delete [] fB;
    delete [] fGene;
}

// 设置反向近邻列表
void TKopt::setInvNearList()
{
    // 初始化每个城市的反向近邻数量
    for( int i = 0; i < fN; ++i ) fNumOfINL[ i ] = 0;
    int c;
    // 遍历所有城市及其近邻
    for( int i = 0; i < fN; ++i ){
        for( int k = 0; k < 50; ++k ){
            c = eval->fNearCity[i][k];
            // 将城市i添加到城市c的反向近邻列表中
            if( fNumOfINL[c] < 500 ) fInvNearList[ c ][ fNumOfINL[c]++ ] = i;
            else{
                printf( "Check fNumOfINL[c] < 500 ) in kopt.cpp \n" );
                fflush( stdout );
            }
        }
    }
}

// 将个体转换为树结构（分段表示）
void TKopt::transIndiToTree( TIndi& indi )
{
    // 构建初始数组表示
    int num, size, orient;
    fArray[1] = 0;
    for( int i = 2; i <= fN; ++i ) fArray[i] = indi.fLink[ fArray[i-1] ][ 1 ];

    // 添加哨兵节点
    fArray[0] = fArray[fN];
    fArray[fN+1] = fArray[1];
    
    // 将路径划分为多个段
    num = 1;
    fNumOfSeg = 0;
    while(1){
        orient = 1;
        size = 0;
        fOrient[ fNumOfSeg ] = orient;
        fOrdSeg[ fNumOfSeg ] = fNumOfSeg;

        // 处理段的第一个城市
        fLink[ fArray[ num ] ][ 0 ] = -1;
        fLink[ fArray[ num ] ][ 1 ] = fArray[ num+1 ];
        fOrdCity[ fArray[ num ] ] = size;
        fSegCity[ fArray[ num ] ] = fNumOfSeg;
        fCitySeg[ fNumOfSeg ][ this->turn(orient) ] = fArray[ num ];
        ++num;
        ++size;

        // 处理段的中间城市
        for( int i = 0; i < (int)sqrt( fN*1.0 )-1; ++i ){
            if( num == fN ) break;
            fLink[ fArray[ num ] ][ 0 ] = fArray[ num-1 ];
            fLink[ fArray[ num ] ][ 1 ] = fArray[ num+1 ];
            fOrdCity[ fArray[ num ] ] = size;
            fSegCity[ fArray[ num ] ] = fNumOfSeg;
            ++num;
            ++size;
        }

        // 处理段的最后城市
        if( num == fN-1 ){
            fLink[ fArray[ num ] ][ 0 ] = fArray[ num-1 ];
            fLink[ fArray[ num ] ][ 1 ] = fArray[ num+1 ];
            fOrdCity[ fArray[ num ] ] = size;
            fSegCity[ fArray[ num ] ] = fNumOfSeg;
            ++num;
            ++size;
        }
        fLink[ fArray[ num ] ][ 0 ] = fArray[ num-1 ];
        fLink[ fArray[ num ] ][ 1 ] = -1;
        fOrdCity[ fArray[ num ] ] = size;
        fSegCity[ fArray[ num ] ] = fNumOfSeg;
        fCitySeg[ fNumOfSeg ][ orient ] = fArray[ num ];
        ++num;
        ++size;
        
        // 更新段的信息
        fSizeSeg[ fNumOfSeg ] = size;
        ++fNumOfSeg;
        if( num == fN+1 ) break;
    }

    // 建立段之间的链接关系
    for( int s = 1; s < fNumOfSeg-1; ++s ){
        fLinkSeg[ s ][ 0 ] = s-1;
        fLinkSeg[ s ][ 1 ] = s+1;
    }
    fLinkSeg[ 0 ][ 0 ] = fNumOfSeg-1;
    fLinkSeg[ 0 ][ 1 ] = 1;
    fLinkSeg[ fNumOfSeg-1 ][ 0 ] = fNumOfSeg-2;
    fLinkSeg[ fNumOfSeg-1 ][ 1 ] = 0;
    
    fTourLength = indi.fEvaluationValue;
    fFixNumOfSeg = fNumOfSeg;
}

// 将树结构转换回个体表示
void TKopt::transTreeToIndi( TIndi& indi ){
    int t_p, t_n;
    // 重建每个城市的链接关系
    for( int t = 0; t < fN; ++t ){
        t_p = this->getPrev( t );   // 获取前一个城市
        t_n = this->getNext( t );   // 获取后一个城市
        indi.fLink[ t ][ 0 ] = t_p;
        indi.fLink[ t ][ 1 ] = t_n;
    }
    eval->doIt( indi );  // 计算新解的评估值
}

// 执行K优化操作
void TKopt::doIt( TIndi& tIndi ){
    this->transIndiToTree( tIndi );  // 转换为树结构
    this->sub();                     // 执行优化
    this->transTreeToIndi( tIndi );  // 转换回个体表示
}

// 执行2-opt局部搜索的核心函数
void TKopt::sub(){
    int t1_st;
    int dis1, dis2;
    // 初始化所有顶点为活跃状态
    for( int t = 0; t < fN; ++t ) fActiveV[ t ] = 1;
BEGIN:
    // 随机选择起始城市
    t1_st = rand()%fN;
    fT[1] = t1_st;
    
    // 对当前城市t1进行遍历
    while(1){
        fT[1] = this->getNext( fT[1] );  // 获取下一个城市
        if( fActiveV[ fT[1] ] == 0 ) goto RETURN;  // 如果该城市已被处理，跳转
        fFlagRev = 0;
        fT[2] = this->getPrev( fT[1] );  // 获取t1的前一个城市

        // 正向搜索：检查是否可以通过2-opt操作改善解
        for( int num1 = 1; num1 < 50; ++num1 ){
            fT[4] = eval->fNearCity[ fT[1] ][ num1 ];  // 获取t1的第num1个近邻
            fT[3] = this->getPrev( fT[4] );           // 获取该近邻的前一个城市
            // 计算路径改善值
            dis1 = eval->fEdgeDis[fT[1]][fT[2]] - eval->fEdgeDis[fT[1]][fT[4]];
            if( dis1 > 0 ){  // 如果可以改善
                dis2 = dis1 + eval->fEdgeDis[fT[3]][fT[4]] - eval->fEdgeDis[fT[3]][fT[2]];
                if( dis2 > 0 ){  // 如果总体改善值为正
                    this->incrementImp( fFlagRev );  // 执行改进
                    // 更新相关城市的近邻城市的活跃状态
                    for( int a = 1; a <= 4; ++a )
                        for( int k = 0; k < fNumOfINL[fT[a]]; ++k )
                            fActiveV[ this->fInvNearList[fT[a]][k] ] = 1;
                    goto BEGIN;  // 重新开始搜索
                }
            }
            else break;  // 如果无法改善则停止当前搜索
        }

        // 反向搜索：类似于正向搜索，但方向相反
        fFlagRev = 1;
        fT[2] = this->getNext( fT[1] );
        for( int num1 = 1; num1 < 50; ++num1 ){
            fT[4] = eval->fNearCity[ fT[1] ][ num1 ];
            fT[3] = this->getNext( fT[4] );
            dis1 = eval->fEdgeDis[fT[1]][fT[2]] - eval->fEdgeDis[fT[1]][fT[4]];
            if( dis1 > 0 ){
                dis2 = dis1 + eval->fEdgeDis[fT[3]][fT[4]] - eval->fEdgeDis[fT[3]][fT[2]];
                if( dis2 > 0 ){
                    this->incrementImp( fFlagRev );
                    for( int a = 1; a <= 4; ++a )
                        for( int k = 0; k < fNumOfINL[fT[a]]; ++k )
                            fActiveV[ this->fInvNearList[fT[a]][k] ] = 1;
                    goto BEGIN;
                }
            }
            else break;
        }
        fActiveV[ fT[1] ] = 0;  // 标记当前城市为非活跃
RETURN:
        if( fT[1] == t1_st ) break;  // 如果回到起始城市则结束
    }
}

// 获取给定城市的下一个城市
int TKopt::getNext( int t ){
    int t_n, seg, orient;
    seg = fSegCity[ t ];        // 获取城市所在的段
    orient = fOrient[ seg ];    // 获取段的方向
    t_n = fLink[ t ][ orient ]; // 获取下一个城市
    if( t_n == -1 ){           // 如果到达段的末尾
        seg = fLinkSeg[ seg ][ orient ];  // 获取下一个段
        orient = turn( fOrient[ seg ] );  // 更新方向
        t_n = fCitySeg[ seg ][ orient ];  // 获取新段中的城市
    }
    return t_n;
}

// 获取给定城市的前一个城市
int TKopt::getPrev( int t ){
    int t_p, seg, orient;
    seg = fSegCity[ t ];        // 获取城市所在的段
    orient = fOrient[ seg ];    // 获取段的方向
    t_p = fLink[ t ][ this->turn( orient ) ];  // 获取前一个城市
    if( t_p == -1 ){           // 如果到达段的开头
        seg = fLinkSeg[ seg ][ turn(orient) ];  // 获取前一个段
        orient = fOrient[ seg ];                // 更新方向
        t_p = fCitySeg[ seg ][ orient ];       // 获取新段中的城市
    }
    return t_p;
}

// 交换两个整数的值
void TKopt::swap(int &x, int &y){
    int s=x;
    x=y;
    y=s;
}

// 翻转方向（0->1或1->0）
int TKopt::turn( int &orient ){
    return 1-orient;
}

// 执行增量改进操作
void TKopt::incrementImp( int flagRev ){
    // 定义路径段端点相关变量
    int t1_s, t1_e, t2_s, t2_e;              // 两个路径段的起点和终点
    int seg_t1_s, seg_t1_e, seg_t2_s, seg_t2_e;  // 段的索引
    int ordSeg_t1_s, ordSeg_t1_e, ordSeg_t2_s, ordSeg_t2_e;  // 段的顺序
    int orient_t1_s, orient_t1_e, orient_t2_s, orient_t2_e;   // 段的方向
    int numOfSeg1, numOfSeg2;                // 两个路径中的段数
    int curr;                                // 当前处理的城市
    int ord;                                 // 当前城市的顺序

    // 定义连接相关的标志变量
    int flag_t2e_t1s;      // t2终点到t1起点的连接标志
    int flag_t2s_t1e;      // t2起点到t1终点的连接标志
    int length_t1s_seg;    // t1起点段的长度
    int length_t1e_seg;    // t1终点段的长度
    int seg;

    // 根据反转标志确定路径段的端点
    if( fFlagRev == 0 ){   // 不需要反转
        t1_s = fT[1];      // 第一段起点
        t1_e = fT[3];      // 第一段终点
        t2_s = fT[4];      // 第二段起点
        t2_e = fT[2];      // 第二段终点
    }
    else if( fFlagRev == 1 ){  // 需要反转
        t1_s = fT[2];
        t1_e = fT[4];
        t2_s = fT[3];
        t2_e = fT[1];
    }

    // 获取各个端点的段信息
    seg_t1_s = fSegCity[ t1_s ];          // 获取t1起点所在的段
    ordSeg_t1_s = fOrdSeg[ seg_t1_s ];    // 获取段的顺序
    orient_t1_s = fOrient[ seg_t1_s ];    // 获取段的方向
    // 获取其他端点的类似信息
    seg_t1_e = fSegCity[ t1_e ];
    ordSeg_t1_e = fOrdSeg[ seg_t1_e ];
    orient_t1_e = fOrient[ seg_t1_e ];
    seg_t2_s = fSegCity[ t2_s ];
    ordSeg_t2_s = fOrdSeg[ seg_t2_s ];
    orient_t2_s = fOrient[ seg_t2_s ];
    seg_t2_e = fSegCity[ t2_e ];
    ordSeg_t2_e = fOrdSeg[ seg_t2_e ];
    orient_t2_e = fOrient[ seg_t2_e ];

    //////////////////// Type1: 所有端点在同一段内的情况 ////////////////////////
    if( ( seg_t1_s == seg_t1_e ) && ( seg_t1_s == seg_t2_s ) && ( seg_t1_s == seg_t2_e ) ){
        // 如果需要调整端点顺序
        if( (fOrient[seg_t1_s] == 1 && (fOrdCity[ t1_s ] > fOrdCity[ t1_e ])) ||
            (fOrient[seg_t1_s] == 0 && (fOrdCity[ t1_s ] < fOrdCity[ t1_e ]))){
            // 交换两个段的所有相关信息
            this->swap( t1_s, t2_s );
            this->swap( t1_e, t2_e );
            this->swap( seg_t1_s, seg_t2_s );
            this->swap( seg_t1_e, seg_t2_e );
            this->swap( ordSeg_t1_s, ordSeg_t2_s );
            this->swap( ordSeg_t1_e, ordSeg_t2_e );
            this->swap( orient_t1_s, orient_t2_s );
            this->swap( orient_t1_e, orient_t2_e );
        }

        // 反转第一段中的城市顺序
        curr = t1_s;
        ord = fOrdCity[ t1_e ];
        while(1){
            // 交换当前城市的前后连接
            this->swap( fLink[curr][0], fLink[curr][1] );
            fOrdCity[ curr ] = ord;
            if( curr == t1_e ) break;
            curr = fLink[curr][turn(orient_t1_s)];
            // 根据方向更新顺序值
            if( orient_t1_s == 0 ) ++ord;
            else --ord;
        }

        // 重新连接端点
        fLink[t2_e][orient_t1_s] = t1_e;
        fLink[t2_s][turn(orient_t1_s)] = t1_s;
        fLink[t1_s][orient_t1_s] = t2_s;
        fLink[t1_e][turn(orient_t1_s)] = t2_e;

        return;
    }

    // 计算两个部分包含的段数
    if( ordSeg_t1_e >= ordSeg_t1_s )	numOfSeg1 = ordSeg_t1_e - ordSeg_t1_s + 1;
    else								numOfSeg1 = ordSeg_t1_e - ordSeg_t1_s + 1 + fNumOfSeg;
    if( ordSeg_t2_e >= ordSeg_t2_s )	numOfSeg2 = ordSeg_t2_e - ordSeg_t2_s + 1;
    else								numOfSeg2 = ordSeg_t2_e - ordSeg_t2_s + 1 + fNumOfSeg;

    // 确保第一部分的段数不大于第二部分
    if( numOfSeg1 > numOfSeg2 ){
        // 交换两个部分的所有相关信息
        this->swap( numOfSeg1, numOfSeg2 );
        this->swap( t1_s, t2_s );
        this->swap( t1_e, t2_e );
        this->swap( seg_t1_s, seg_t2_s );
        this->swap( seg_t1_e, seg_t2_e );
        this->swap( ordSeg_t1_s, ordSeg_t2_s );
        this->swap( ordSeg_t1_e, ordSeg_t2_e );
        this->swap( orient_t1_s, orient_t2_s );
        this->swap( orient_t1_e, orient_t2_e );
    }

    // 检查端点连接状态
    flag_t2e_t1s = (fLink[ t2_e ][ orient_t2_e ] == -1) ? 1 : 0;
    flag_t2s_t1e = (fLink[ t2_s ][ this->turn(orient_t2_s) ] == -1) ? 1 : 0;

    // 计算段的长度
    length_t1s_seg = abs( fOrdCity[ t2_e ] - fOrdCity[ fCitySeg[ seg_t2_e ][ orient_t2_e ] ] );
    length_t1e_seg = abs( fOrdCity[ t2_s ] - fOrdCity[ fCitySeg[ seg_t2_s ][ this->turn(orient_t2_s) ] ] );

    ///////////////////// Type2: t1的起点和终点在同一段内 /////////////////
    if( seg_t1_s == seg_t1_e ){
        // 两个端点都是段的端点
        if( flag_t2e_t1s == 1 && flag_t2s_t1e == 1 ){
            // 更新段的方向和连接关系
            orient_t1_s = turn( fOrient[ seg_t1_s ] );
            fOrient[ seg_t1_s ] = orient_t1_s;
            fCitySeg[ seg_t1_s ][ orient_t1_s ] = t1_s;
            fCitySeg[ seg_t1_s ][ turn(orient_t1_s) ] = t1_e;
            fLinkSeg[ seg_t1_s ][ orient_t1_s ] = seg_t2_s;
            fLinkSeg[ seg_t1_s ][ turn(orient_t1_s) ] = seg_t2_e;
            return;
        }

        // t2终点是段端点，t2起点不是
        if( flag_t2e_t1s == 0 && flag_t2s_t1e == 1 ){
            // 反转并重新连接
            curr = t1_e;
            ord = fOrdCity[ t1_s ];
            while(1){
                this->swap( fLink[curr][0], fLink[curr][1] );
                fOrdCity[ curr ] = ord;
                if( curr == t1_s ) break;

                curr = fLink[curr][orient_t2_e];
                if( orient_t2_e == 0 ) --ord;
                else ++ord;
            }
            fLink[t2_e][orient_t2_e] = t1_e;
            fLink[t1_s][orient_t2_e] = -1;
            fLink[t1_e][turn(orient_t2_e)] = t2_e;
            fCitySeg[seg_t2_e][orient_t2_e] = t1_s;
            return;
        }

        // t2起点是段端点，t2终点不是
        if( flag_t2e_t1s == 1 && flag_t2s_t1e == 0 ){
            // 反转并重新连接
            curr = t1_s;
            ord = fOrdCity[ t1_e ];
            while(1){
                this->swap( fLink[curr][0], fLink[curr][1] );
                fOrdCity[ curr ] = ord;
                if( curr == t1_e ) break;

                curr = fLink[curr][turn(orient_t2_s)];
                if( orient_t2_s == 0 ) ++ord;
                else --ord;
            }
            fLink[t2_s][turn(orient_t2_s)] = t1_s;
            fLink[t1_e][turn(orient_t2_s)] = -1;
            fLink[t1_s][orient_t2_s] = t2_s;
            fCitySeg[seg_t2_s][turn(orient_t2_s)] = t1_e;
            return;
        }
    }

    ///////////////////// Type3: 处理跨多段的情况 /////////////////
    // 处理t1起点和t2终点的连接
    if( flag_t2e_t1s == 1 ) {
        // 如果t2终点是段端点，直接连接
        fLinkSeg[seg_t1_s][turn(orient_t1_s)] = seg_t2_s;
    } else {
        // 需要创建新段
        seg_t1_s = fNumOfSeg++;        // 分配新段的索引
        orient_t1_s = orient_t2_e;     // 设置新段的方向
        // 设置端点的连接
        fLink[ t1_s ][turn(orient_t1_s)] = -1;
        fLink[ fCitySeg[seg_t2_e][orient_t2_e]][orient_t1_s] = -1;
        // 初始化新段的属性
        fOrient[seg_t1_s] = orient_t1_s;
        fSizeSeg[seg_t1_s] = length_t1s_seg;
        fCitySeg[seg_t1_s][turn(orient_t1_s)] = t1_s;
        fCitySeg[seg_t1_s][orient_t1_s] = fCitySeg[seg_t2_e][orient_t2_e];
        // 设置段间连接
        fLinkSeg[seg_t1_s][turn(orient_t1_s)] = seg_t2_s;
        fLinkSeg[seg_t1_s][orient_t1_s] = fLinkSeg[seg_t2_e][orient_t2_e];
        seg = fLinkSeg[seg_t2_e][orient_t2_e];
        fLinkSeg[seg][turn(fOrient[seg])] = seg_t1_s;
    }

    // 处理t1终点和t2起点的连接（类似于上面的处理）
    if( flag_t2s_t1e == 1 ) {
        fLinkSeg[seg_t1_e][orient_t1_e] = seg_t2_e;
    } else {
        // 创建并初始化新段
        seg_t1_e = fNumOfSeg++;
        orient_t1_e = orient_t2_s;
        fLink[ t1_e ][orient_t1_e] = -1;
        fLink[ fCitySeg[seg_t2_s][turn(orient_t2_s)] ][turn(orient_t1_e)] = -1;
        fOrient[seg_t1_e] = orient_t1_e;
        fSizeSeg[seg_t1_e] = length_t1e_seg;
        fCitySeg[seg_t1_e][orient_t1_e] = t1_e;
        fCitySeg[seg_t1_e][turn(orient_t1_e)] = fCitySeg[seg_t2_s][turn(orient_t2_s)];
        fLinkSeg[seg_t1_e][orient_t1_e] = seg_t2_e;
        fLinkSeg[seg_t1_e][turn(orient_t1_e)] = fLinkSeg[seg_t2_s][turn(orient_t2_s)];
        seg = fLinkSeg[seg_t2_s][turn(orient_t2_s)];
        fLinkSeg[seg][fOrient[seg]] = seg_t1_e;
    }

    // 更新t2段的信息
    // 更新t2终点段
    fLink[t2_e][orient_t2_e] = -1;
    fSizeSeg[seg_t2_e] -= length_t1s_seg;
    fCitySeg[seg_t2_e][orient_t2_e] = t2_e;
    fLinkSeg[seg_t2_e][orient_t2_e] = seg_t1_e;
    // 更新t2起点段
    fLink[t2_s][turn(orient_t2_s)] = -1;
    fSizeSeg[seg_t2_s] -= length_t1e_seg;
    fCitySeg[seg_t2_s][turn(orient_t2_s)] = t2_s;
    fLinkSeg[seg_t2_s][turn(orient_t2_s)] = seg_t1_s;

    // 反转从seg_t1_s到seg_t1_e之间所有段的方向
    seg = seg_t1_e;
    while(1){
        fOrient[seg] = turn(fOrient[seg]);
        if( seg == seg_t1_s ) break;
        seg = fLinkSeg[seg][fOrient[seg]];
    }

    // 如果t2终点段比新段小，交换它们的信息
    if( fSizeSeg[seg_t2_e] < length_t1s_seg ){
        // 更新段间连接
        seg = fLinkSeg[seg_t2_e][turn(fOrient[seg_t2_e])];
        fLinkSeg[seg][fOrient[seg]] = seg_t1_s;
        seg = fLinkSeg[seg_t2_e][fOrient[seg_t2_e]];
        fLinkSeg[seg][turn(fOrient[seg])] = seg_t1_s;
        seg = fLinkSeg[seg_t1_s][turn(fOrient[seg_t1_s])];
        fLinkSeg[seg][fOrient[seg]] = seg_t2_e;
        seg = fLinkSeg[seg_t1_s][fOrient[seg_t1_s]];
        fLinkSeg[seg][turn(fOrient[seg])] = seg_t2_e;

        // 交换两个段的所有信息
        this->swap( fOrient[seg_t2_e], fOrient[seg_t1_s] );
        this->swap( fSizeSeg[seg_t2_e], fSizeSeg[seg_t1_s] );
        this->swap( fCitySeg[seg_t2_e][0], fCitySeg[seg_t1_s][0] );
        this->swap( fCitySeg[seg_t2_e][1], fCitySeg[seg_t1_s][1] );
        this->swap( fLinkSeg[seg_t2_e][0], fLinkSeg[seg_t1_s][0] );
        this->swap( fLinkSeg[seg_t2_e][1], fLinkSeg[seg_t1_s][1] );
        this->swap( seg_t2_e, seg_t1_s );
    }

    // 如果t2起点段比新段小，交换它们的信息（类似于上面的处理）
    if( fSizeSeg[seg_t2_s] < length_t1e_seg ){
        // 更新段间连接和交换段信息
        seg = fLinkSeg[seg_t2_s][turn(fOrient[seg_t2_s])];
        fLinkSeg[seg][fOrient[seg]] = seg_t1_e;
        seg = fLinkSeg[seg_t2_s][fOrient[seg_t2_s]];
        fLinkSeg[seg][turn(fOrient[seg])] = seg_t1_e;
        seg = fLinkSeg[seg_t1_e][turn(fOrient[seg_t1_e])];
        fLinkSeg[seg][fOrient[seg]] = seg_t2_s;
        seg = fLinkSeg[seg_t1_e][fOrient[seg_t1_e]];
        fLinkSeg[seg][turn(fOrient[seg])] = seg_t2_s;

        this->swap( fOrient[seg_t2_s], fOrient[seg_t1_e] );
        this->swap( fSizeSeg[seg_t2_s], fSizeSeg[seg_t1_e] );
        this->swap( fCitySeg[seg_t2_s][0], fCitySeg[seg_t1_e][0] );
        this->swap( fCitySeg[seg_t2_s][1], fCitySeg[seg_t1_e][1] );
        this->swap( fLinkSeg[seg_t2_s][0], fLinkSeg[seg_t1_e][0] );
        this->swap( fLinkSeg[seg_t2_s][1], fLinkSeg[seg_t1_e][1] );
        this->swap( seg_t2_s, seg_t1_e );
    }

    // 合并多余的段，保持段数不超过初始值
    while( fNumOfSeg > fFixNumOfSeg ){
        // 选择较小的相邻段进行合并
        if( fSizeSeg[ fLinkSeg[fNumOfSeg-1][0] ] <
            fSizeSeg[ fLinkSeg[fNumOfSeg-1][1] ] )
            this->combineSeg( fLinkSeg[fNumOfSeg-1][0], fNumOfSeg-1 );
        else
            this->combineSeg( fLinkSeg[fNumOfSeg-1][1], fNumOfSeg-1 );
    }

    // 重新计算段的顺序
    int ordSeg = 0;
    seg = 0;
    while(1){
        fOrdSeg[seg] = ordSeg;
        ++ordSeg;
        seg = fLinkSeg[seg][ fOrient[seg] ];
        if( seg == 0 ) break;
    }
    return;
}

// 合并两个段（segL是较大的段，segS是较小的段）
void TKopt::combineSeg( int segL, int segS ){
   int seg;
   int t_s, t_e, direction;    // 小段的起点、终点和遍历方向
   t_s = 0; t_e = 0; direction = 0;
   int ord = 0;               // 城市的序号
   int increment = 0;         // 序号的增量（1或-1）
   int curr, next;           // 当前和下一个城市

   // 情况1：小段在大段的正向连接处
   if( fLinkSeg[segL][fOrient[segL]] == segS ){
       // 连接两段的端点
       fLink[fCitySeg[segL][fOrient[segL]]][fOrient[segL]] = fCitySeg[segS][turn(fOrient[segS])];
       fLink[fCitySeg[segS][turn(fOrient[segS])]][turn(fOrient[segS])] = fCitySeg[segL][fOrient[segL]];
       ord = fOrdCity[fCitySeg[segL][fOrient[segL]]];

       // 更新大段的信息
       fCitySeg[segL][fOrient[segL]] = fCitySeg[segS][fOrient[segS]];
       fLinkSeg[segL][fOrient[segL]] = fLinkSeg[segS][fOrient[segS]];
       seg = fLinkSeg[segS][fOrient[segS]];
       fLinkSeg[seg][turn(fOrient[seg])] = segL;

       // 设置遍历参数
       t_s = fCitySeg[segS][turn(fOrient[segS])];
       t_e = fCitySeg[segS][fOrient[segS]];
       direction = fOrient[segS];

       // 确定序号增量
       if( fOrient[segL] == 1 ) increment = 1;
       else increment = -1;
   }
   // 情况2：小段在大段的反向连接处
   else if( fLinkSeg[segL][turn(fOrient[segL])] == segS ){
       // 连接两段的端点
       fLink[fCitySeg[segL][turn(fOrient[segL])]][turn(fOrient[segL])] = fCitySeg[segS][fOrient[segS]];
       fLink[fCitySeg[segS][fOrient[segS]]][fOrient[segS]] = fCitySeg[segL][turn(fOrient[segL])];
       ord = fOrdCity[fCitySeg[segL][turn(fOrient[segL])]];

       // 更新大段的信息
       fCitySeg[segL][turn(fOrient[segL])] = fCitySeg[segS][turn(fOrient[segS])];
       fLinkSeg[segL][turn(fOrient[segL])] = fLinkSeg[segS][turn(fOrient[segS])];
       seg = fLinkSeg[segS][turn(fOrient[segS])];
       fLinkSeg[seg][fOrient[seg]] = segL;

       // 设置遍历参数
       t_s = fCitySeg[segS][fOrient[segS]];
       t_e = fCitySeg[segS][turn(fOrient[segS])];
       direction = turn(fOrient[segS]);

       // 确定序号增量
       if( fOrient[segL] == 1 ) increment = -1;
       else increment = 1;
   }

   // 更新小段中所有城市的信息
   curr = t_s;
   ord = ord + increment;
   while(1){
       fSegCity[curr] = segL;       // 更新城市所属的段
       fOrdCity[curr] = ord;        // 更新城市的序号

       next = fLink[curr][direction];
       // 如果两段方向不同，需要翻转连接
       if( fOrient[segL] != fOrient[segS] ) this->swap( fLink[curr][0], fLink[curr][1] );

       if( curr == t_e ) break;     // 到达终点退出
       curr = next;
       ord += increment;            // 更新序号
   }
   // 更新段的大小和总段数
   fSizeSeg[segL] += fSizeSeg[segS];
   --fNumOfSeg;
}

// 检查数据结构的一致性
void TKopt::checkDetail(){
   int seg, seg_p, seg_n;     // 当前段、前一段、后一段
   int ord, ord_p, ord_n;     // 当前序号、前一序号、后一序号
   int orient;                // 段的方向
   int curr;                  // 当前城市

   // 检查每个段的信息
   seg = 0;
   for( int s = 0; s < fNumOfSeg; ++s ){
       seg = s;
       orient = fOrient[ seg ];
       // 获取相邻段
       seg_p = fLinkSeg[ seg ][ this->turn(orient) ];
       seg_n = fLinkSeg[ seg ][ orient ];

       // 检查序号的合法性
       ord = fOrdSeg[ seg ];
       ord_p = ord - 1 ;
       if( ord_p < 0 ) ord_p = fNumOfSeg - 1;

       ord_n = ord + 1;
       if( ord_n >= fNumOfSeg ) ord_n = 0;

       // 检查段内城市的连接
       curr = fCitySeg[ s ][ 0 ];
       int count = 0;

       while(1){
           ++count;
           if( curr == fCitySeg[ s ][1] ) break;
           curr = fLink[curr][1];
       }
   }

   // 检查每个城市的连接信息
   int t, t_n, t_p, t_s, t_e;
   for( t = 0; t < fN; ++t ){
       seg = fSegCity[ t ];
       orient = fOrient[ seg ];
       t_s = fCitySeg[ seg ][ 0 ];
       t_e = fCitySeg[ seg ][ 1 ];

       t_p = fLink[ t ][ 0 ];
       t_n = fLink[ t ][ 1 ];
   }
}

// 检查解的有效性
void TKopt::checkValid(){
   int t_st, t_c, t_n;        // 起始城市、当前城市、下一个城市
   int count;                 // 访问城市计数
   int seg, orient;           // 当前段和方向
   int Invalid = 0;           // 无效解标记

   // 初始化检查数组
   for( int i = 0; i < fN; ++i ) fCheckN[ i ] = 0;

   // 随机选择起始城市
   t_st = rand() % fN;
   t_n = t_st;
   count = 0;

   // 沿着路径遍历所有城市
   while(1){
       t_c = t_n;
       fCheckN[ t_c ] = 1;    // 标记已访问
       ++count;

       // 获取当前城市所在段的信息
       seg = fSegCity[ t_c ];
       orient = fOrient[ seg ];
       t_n = this->getNext( t_c );  // 获取下一个城市

       // 如果回到起点，完成一个回路
       if( t_n == t_st ) break;

       // 如果访问城市数超过总数，说明路径无效
       if( count == fN+1 ){
           Invalid = 1;
           break;
       }
   }

   // 检查是否所有城市都被访问
   for( int i = 0; i < fN; ++i )
       if( fCheckN[ i ] == 0 ) Invalid = 1;

   // 输出无效解的警告
   if( Invalid == 1 ) printf( "Invalid \n" ); fflush( stdout );
}

// 生成随机解
void TKopt::makeRandSol( TIndi& indi ){
   // 初始化数组 fB，用于生成随机排列
   for( int j = 0; j < fN; ++j ) fB[j] = j;
   int r;
   
   // 生成随机排列（Fisher-Yates洗牌算法）
   for( int i = 0; i < fN; ++i ){
       r = rand() % (fN-i);     // 随机选择位置
       fGene[i] = fB[r];        // 保存选中的城市
       fB[r] = fB[fN-i-1];      // 将末尾元素移到空出的位置
   }

   // 建立链接关系
   // 处理中间城市的连接
   for( int j2 = 1 ; j2 < fN-1; ++j2 ){
       indi.fLink[fGene[j2]][0] = fGene[j2-1];  // 连接前一个城市
       indi.fLink[fGene[j2]][1] = fGene[j2+1];  // 连接后一个城市
   }
   // 处理首尾城市的连接，形成回路
   indi.fLink[fGene[0]][0] = fGene[fN-1];    // 首城市与末城市相连
   indi.fLink[fGene[0]][1] = fGene[1];       // 首城市与第二个城市相连
   indi.fLink[fGene[fN-1]][0] = fGene[fN-2]; // 末城市与倒数第二个城市相连
   indi.fLink[fGene[fN-1]][1] = fGene[0];    // 末城市与首城市相连

   // 计算解的评估值
   eval->doIt( indi );
}