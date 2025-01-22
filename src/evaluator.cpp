#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

// 评估器类构造函数
TEvaluator::TEvaluator()
{
    fEdgeDis = NULL;     // 边距离矩阵
    fNearCity = NULL;    // 邻近城市矩阵
    Ncity = 0;          // 城市数量
    fNearNumMax = 50;   // 最大邻近城市数量
}

// 析构函数：释放动态分配的内存
TEvaluator::~TEvaluator(){
    // 释放边距离矩阵
    for ( int i = 0; i < Ncity; ++i ) delete[] fEdgeDis[ i ];
    delete[] fEdgeDis;

    // 释放邻近城市矩阵
    for ( int i = 0; i < Ncity; ++i ) delete[] fNearCity[ i ];
    delete[] fNearCity;

    // 释放坐标数组
    delete [] x;
    delete [] y;
}

// 从文件中读取并设置问题实例
void TEvaluator::setInstance( char filename[] )
{
    FILE* fp;
    int n;
    char word[ 80 ], type[ 80 ];
    fp = fopen( filename, "r" );

    /* 读取实例信息 */
    while( 1 ){
        if( fscanf( fp, "%s", word ) == EOF ) break;
        // 读取城市数量
        if( strcmp( word, "DIMENSION" ) == 0 ){
            fscanf( fp, "%s", word );
            fscanf( fp, "%d", &Ncity );
        }
        // 读取边权重类型
        if( strcmp( word, "EDGE_WEIGHT_TYPE" ) == 0 ){
            fscanf( fp, "%s", word );
            fscanf( fp, "%s", type );
        }
        // 查找坐标数据段开始标记
        if( strcmp( word, "NODE_COORD_SECTION" ) == 0 ) break;
    }

    // 检查文件格式
    if( strcmp( word, "NODE_COORD_SECTION" ) != 0 ){
        printf( "Error in reading the instance\n" );
        exit(0);
    }

    // 分配内存并读取坐标数据
    x = new double [ Ncity ];
    y = new double [ Ncity ];
    int *checkedN = new int[Ncity];

    // 读取每个城市的坐标
    for( int i = 0; i < Ncity; ++i ){
        fscanf( fp, "%d", &n );
        fscanf( fp, "%s", word );
        x[ i ] = atof( word );
        fscanf( fp, "%s", word );
        y[ i ] = atof( word );
    }
    fclose(fp);

    // 分配边距离矩阵和邻近城市矩阵的内存
    fEdgeDis = new int* [ Ncity ];
    for( int i = 0; i < Ncity; ++i ) fEdgeDis[ i ] = new int [ Ncity ];
    fNearCity = new int* [ Ncity ];
    for( int i = 0; i < Ncity; ++i ) fNearCity[ i ] = new int [ fNearNumMax+1 ];

    // 根据不同的距离计算类型计算边距离
    if( strcmp( type, "EUC_2D" ) == 0  ) {
        // 欧几里得距离
        for( int i = 0; i < Ncity ; ++i )
            for( int j = 0; j < Ncity ; ++j )
                fEdgeDis[ i ][ j ]=(int)(sqrt((x[i]-x[j])*(x[i]-x[j])+(y[i]-y[j])*(y[i]-y[j]))+0.5);
    }
    else if( strcmp( type, "ATT" ) == 0  ) {
        // ATT距离计算方式
        for( int i = 0; i < Ncity; ++i ){
            for( int j = 0; j < Ncity; ++j ) {
                double r = (sqrt(((x[i]-x[j])*(x[i]-x[j])+(y[i]-y[j])*(y[i]-y[j]))/10.0));
                int t = (int)r;
                if( (double)t < r ) fEdgeDis[ i ][ j ] = t+1;
                else fEdgeDis[ i ][ j ] = t;
            }
        }
    }
    else if( strcmp( type, "CEIL_2D" ) == 0  ){
        // 向上取整的欧几里得距离
        for( int i = 0; i < Ncity ; ++i )
            for( int j = 0; j < Ncity ; ++j )
                fEdgeDis[ i ][ j ]=(int)ceil(sqrt((x[i]-x[j])*(x[i]-x[j])+(y[i]-y[j])*(y[i]-y[j])));
    }
    else{
        printf( "EDGE_WEIGHT_TYPE is not supported\n" );
        exit( 1 );
    }

    // 计算每个城市的邻近城市列表
    int ci, j1, j2, j3;
    int cityNum = 0;
    int minDis;
    for( ci = 0; ci < Ncity; ++ci ){
        for( j3 = 0; j3 < Ncity; ++j3 ) checkedN[ j3 ] = 0;
        checkedN[ ci ] = 1;
        fNearCity[ ci ][ 0 ] = ci;
        // 找出距离最近的fNearNumMax个城市
        for( j1 = 1; j1 <= fNearNumMax; ++j1 ) {
            minDis = 100000000;
            for( j2 = 0; j2 < Ncity; ++j2 ){
                if( fEdgeDis[ ci ][ j2 ] <= minDis && checkedN[ j2 ] == 0 ){
                    cityNum = j2;
                    minDis = fEdgeDis[ ci ][ j2 ];
                }
            }
            fNearCity[ ci ][ j1 ] = cityNum;
            checkedN[ cityNum ] = 1;
        }
    }
}

// 计算个体的评估值（总路径长度）
void TEvaluator::doIt( TIndi& indi ){
    int d = 0;
    // 累加所有边的距离
    for( int i = 0; i < Ncity; ++i ) d += fEdgeDis[ i ][ indi.fLink[i][0] ] + fEdgeDis[ i ][ indi.fLink[i][1] ];
    indi.fEvaluationValue = d/2;  // 由于每条边被计算了两次，所以需要除以2
}

// 将个体的解写入文件
void TEvaluator::writeTo( FILE* fp, TIndi& indi )
{
    Array=new int[Ncity];
    int curr=0, st=0, count=0, pre=-1, next;
    // 将链接表示转换为路径序列
    while( 1 ){
        Array[ count++ ] = curr + 1;
        if( count > Ncity ){
            printf( "Invalid\n" );
            return;
        }
        // 确定下一个城市
        if( indi.fLink[ curr ][ 0 ] == pre ) next = indi.fLink[ curr ][ 1 ];
        else next = indi.fLink[ curr ][ 0 ];

        pre = curr;
        curr = next;
        if( curr == st ) break;  // 回到起点，结束
    }
    
    // 检查解的有效性
    if( this->checkValid( Array, indi.fEvaluationValue ) == false )
        printf( "Individual is invalid \n" );

    // 写入文件
    fprintf( fp, "%d %d\n", indi.fN, indi.fEvaluationValue );
    for( int i = 0; i < indi.fN; ++i )
        fprintf( fp, "%d ", Array[ i ] );
    fprintf( fp, "\n" );
}

// 将个体的解输出到标准输出
void TEvaluator::writeToStdout(TIndi &indi)
{
    Array = new int[Ncity];               // 创建数组存储路径
    int curr = 0, st = 0, count = 0, pre = -1, next;
    // 将链接表示转换为路径序列
    while (1)
    {
        Array[count++] = curr + 1;        // 记录当前城市（从1开始编号）
        if (count > Ncity)                // 检查是否超出城市数量
        {
            printf("Invalid\n");
            return;
        }
        // 确定下一个城市：如果前一个城市是link[0]，则选择link[1]，反之亦然
        if (indi.fLink[curr][0] == pre)
            next = indi.fLink[curr][1];
        else
            next = indi.fLink[curr][0];

        pre = curr;                       // 更新前一个城市
        curr = next;                      // 移动到下一个城市
        if (curr == st)                   // 如果回到起点，结束循环
            break;
    }
    // 检查解的有效性
    if (this->checkValid(Array, indi.fEvaluationValue) == false)
        printf("Individual is invalid \n");

    // 输出解的信息
    printf("%d %d\n", indi.fN, indi.fEvaluationValue);  // 输出城市数量和总距离
    for (int i = 0; i < indi.fN; ++i)                   // 输出访问顺序
        printf("%d ", Array[i]);
    printf("\n");
}

// 检查解的有效性
bool TEvaluator::checkValid(int* array, int value) {
    int *check = new int[Ncity];
    // 初始化检查数组
    for( int i = 0; i < Ncity; ++i ) check[i] = 0;
    
    // 统计每个城市出现的次数
    for( int i = 0; i < Ncity; ++i ) ++check[array[i]-1];
    
    // 检查每个城市是否只出现一次
    for( int i = 0; i < Ncity; ++i )
        if( check[i] != 1 ) return false;
    
    // 计算实际路径长度
    int distance = 0;
    // 计算相邻城市间的距离
    for( int i = 0; i < Ncity-1; ++i )
        distance += fEdgeDis[array[i]-1][array[i+1]-1];
    
    // 添加返回起点的距离
    distance += fEdgeDis[array[Ncity-1]-1][array[0]-1];

    delete [] check;
    
    // 检查计算的距离是否与给定的评估值相符
    if( distance != value ) return false;
    return true;
}