#pragma once
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

// 最大顶点数
#define MAX_VERTEX_NUM 20

// 顶点结构体（景点信息）
struct Vex {
    int num;        // 景点编号
    char name[20];  // 景点名字
    char desc[1024];// 景点介绍
};

// 边结构体（道路信息）
struct Edge {
    int vex1;       // 边的第一个顶点编号
    int vex2;       // 边的第二个顶点编号
    int weight;     // 边的权值（道路距离，单位m）
};

// 路线节点结构体，用于存储一条路线的所有景点编号
typedef struct PathNode {
    int vexs[MAX_VERTEX_NUM]; // 存储一条路线的所有景点编号
    struct PathNode* next;    // 指向下一条路线的指针
} PathNode, * PathList;

// 图的类（邻接矩阵实现）
class CGraph {
private:
    int m_aAdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; // 邻接矩阵
    Vex m_aVexs[MAX_VERTEX_NUM];                      // 顶点数组
    int m_nVexNum;                                    // 当前顶点数目
public:
    // 构造函数
    CGraph() { m_nVexNum = 0; memset(m_aAdjMatrix, 0, sizeof(m_aAdjMatrix)); }
    // 初始化图
    void Init(void);
    // 插入顶点
    bool InsertVex(Vex sVex);
    // 插入边（无向图，双向赋值）
    bool InsertEdge(Edge sEdge);
    // 根据编号获取顶点信息
    Vex GetVex(int v);
    // 查找指定顶点的所有邻边，返回邻边数量
    int FindEdge(int v, Edge aEdge[]);
    // 获取当前顶点数目
    int GetVexNum() { return m_nVexNum; }
    // 获取邻接矩阵的值
    int GetAdjMatrixVal(int v1, int v2) { return m_aAdjMatrix[v1][v2]; }
    // DFS核心函数，递归遍历生成路线，回溯实现多路线
    void DFS(int nVex, bool bVisited[], int aPath[], int nIndex, PathList& pList);
    // DFS遍历入口函数，初始化参数并调用DFS
    void DFSTraverse(int nVex, PathList& pList);
    // Dijkstra最短路径 - 求源点到终点的最短路径，返回最短距离，path数组存储路径（从终点到源点）
    int Dijkstra(int nStart, int nEnd, int path[]);
    // Prim最小生成树 - 构建最小生成树，treeEdges数组存储生成树的边，返回总权值
    int Prim(Edge treeEdges[]);
};
// 路线链表相关函数
PathList CreatePathList();                // 创建空的路径链表
void AddPath(PathList& pList, int aPath[], int nLen); // 向链表添加一条路线
void FreePathList(PathList& pList);       // 释放路径链表内存
