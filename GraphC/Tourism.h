#pragma once
#include "Graph.h"

// 全局图对象，用于存储景区景点图
extern CGraph m_Graph;

// 创建景区景点图：读取Vex.txt和Edge.txt，初始化图并插入顶点/边
void CreateGraph(void);

// 查询景点信息：显示景点列表，输入编号查询详情及邻接景点
void GetSpotInfo(void);

// 旅游景点导航，输入起始编号，输出所有无回路游览路线
void TravelPath(void);

// 搜索两个景点之间的最短路径
void FindShortPath(void);

// 铺设电路规划（Prim最小生成树）
void DesignCircuitPath(void);