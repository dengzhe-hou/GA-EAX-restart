/*
 * main.cpp
 *   created on: Oct 6, 2020
 *       author: shengcailiu (liusccc@gmail.com)
 */

// 这是GA-EAX-restart算法的主程序,主要功能包括:
// 参数处理和初始化：
// 解析命令行参数(TSP文件、种群大小、子代数等)
// 初始化随机数生成器
// 创建算法环境

// 信号处理：
// 注册SIGTERM和SIGINT信号处理函数
// 在程序被中断时输出当前最优解

// 算法主循环：
// 最多运行100万次迭代
// 每次迭代调用gEnv->doIt()执行进化
// 检查终止条件(达到最优解或时间限制)

// 结果输出：
// 输出运行时间和最优解质量
// 将最优路径转换为城市序列并输出
// 判断是否达到已知的最优解

#ifndef __ENVIRONMENT__
#include "environment.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <iostream>
using namespace std;

// 全局变量定义
int gBestValue = -1;    // 当前找到的最优解值
TIndi gBest;           // 当前找到的最优解
int optimum;           // 最优解值(如果已知)
int duration;          // 运行时间

/**
* 信号处理函数 - 处理程序终止信号(如Ctrl+C)
* 在程序被中断时打印当前最优解信息
*/
void signalHandler(int signum) {
   cout << endl << "Signal (" << signum << ") received.\n";
   cout << endl;

   // 打印运行时间和解的质量
   printf("Total time: %d\n", duration);
   printf("bestval = %d, optimum = %d \n", gBestValue, optimum);

   // 将最优解转换为城市序列并输出
   int Ncity = gBest.fN;
   int *Array = new int[Ncity];
   int curr = 0, st = 0, count = 0, pre = -1, next;
   
   // 通过链表结构重构路径
   while (1) {
       Array[count++] = curr + 1;  // 城市编号从1开始
       if (count > Ncity) {
           printf("Invalid\n");
           return;
       }
       // 确定下一个城市
       if (gBest.fLink[curr][0] == pre)
           next = gBest.fLink[curr][1];
       else
           next = gBest.fLink[curr][0];

       pre = curr;
       curr = next;
       if (curr == st) break;  // 回到起点,路径完成
   }

   // 输出解的信息
   printf("%d %d\n", gBest.fN, gBest.fEvaluationValue);
   for (int i = 0; i < gBest.fN; ++i)
       printf("%d ", Array[i]);
   printf("\n");

   // 判断是否达到最优解
   if (gBestValue != -1 && gBestValue <= optimum)
       printf("Successful\n");
   else
       printf("Unsuccessful\n");
   fflush(stdout);

   exit(signum);
}

/**
* 主函数
* 参数格式: ./GA-EAX-restart tsp_file NPOP NCH optimum tmax seed
* tsp_file: TSP问题实例文件
* NPOP: 种群大小(默认100)
* NCH: 子代数量(默认30)
* optimum: 最优值(未知时设为-1)
* tmax: 最大运行时间(秒)
* seed: 随机数种子
*/
int main(int argc, char* argv[]) {
   // 注册信号处理函数
   signal(SIGTERM, signalHandler);
   signal(SIGINT, signalHandler);

   // 创建并初始化环境
   TEnvironment* gEnv = new TEnvironment();
   gEnv->fFileNameTSP=(char*)malloc(100);

   // 检查命令行参数
   if(argc != 7) {
       cout << "./GA-EAX-restart tsp_file NPOP NCH optimum tmax seed";
       exit(-1);
   }

   // 设置参数
   gEnv->fFileNameTSP = argv[1];   // TSP文件名
   gEnv->Npop = atoi(argv[2]);     // 种群大小
   gEnv->Nch = atoi(argv[3]);      // 子代数量
   optimum = gEnv->optimum = atoi(argv[4]);  // 最优值
   gEnv->tmax = atoi(argv[5]);     // 最大运行时间
   InitURandom(atoi(argv[6]));     // 初始化随机数生成器
   gEnv->terminate = false;        // 终止标志

   // 初始化并运行算法
   cout<<"Initialization ..."<<endl;
   gEnv->define();
   for (int n = 0; n < 1000000; ++n) {  // 最多运行100万次
       printf("Run %d\n", n);
       gEnv->doIt();
       if (gEnv->terminate) break;  // 如果达到终止条件则退出
   }
   
   // 输出最终结果
   gEnv->printOn();

   return 0;
}