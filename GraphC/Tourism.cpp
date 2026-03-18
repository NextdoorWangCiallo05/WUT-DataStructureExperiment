#include "Graph.h"
#include "Tourism.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// 定义全局图对象
CGraph m_Graph;

// 创建景区景点图：读取Vex.txt和Edge.txt，构建邻接矩阵图
void CreateGraph(void) {
    // 1. 初始化图
    m_Graph.Init();
    ifstream fin;
    int nVexNum = 0;

    // 2. 读取Vex.txt，插入顶点
    fin.open("Vex.txt");
    if (!fin.is_open()) {
        cout << "Vex.txt文件打开失败，创建图失败！" << endl;
        return;
    }
    // 读取景点总数
    fin >> nVexNum;
    Vex sVex;
    for (int i = 0; i < nVexNum; i++) {
        fin >> sVex.num;
        fin >> sVex.name;
        fin.ignore(); // 忽略换行符，避免getline读取空行
        fin.getline(sVex.desc, sizeof(sVex.desc)); // 读取景点介绍（含空格）
        m_Graph.InsertVex(sVex);
    }
    fin.close();

    // 3. 读取Edge.txt，插入边
    fin.open("Edge.txt");
    if (!fin.is_open()) {
        cout << "Edge.txt文件打开失败，创建图失败！" << endl;
        return;
    }
    Edge sEdge;
    while (fin >> sEdge.vex1 >> sEdge.vex2 >> sEdge.weight) {
        m_Graph.InsertEdge(sEdge);
    }
    fin.close();

    // 4. 输出创建结果，按实验要求格式
    cout << "====================创建景区景点图成功====================" << endl;
    cout << "顶点数目：" << m_Graph.GetVexNum() << endl;
    cout << "顶点-----" << endl;
    for (int i = 0; i < m_Graph.GetVexNum(); i++) {
        Vex v = m_Graph.GetVex(i);
        cout << v.num << "-" << v.name << endl;
    }
    cout << "边-----" << endl;
    for (int i = 0; i < m_Graph.GetVexNum(); i++) {
        for (int j = i + 1; j < m_Graph.GetVexNum(); j++) { // 避免重复输出边
            int weight = m_Graph.GetAdjMatrixVal(i, j);
            if (weight != 0) {
                cout << "<v" << i << ",v" << j << ">" << weight << endl;
            }
        }
    }
    cout << "==========================================================" << endl;
}

// 查询景点信息：显示列表，输入编号查询详情+邻接景点
void GetSpotInfo(void) {
    int nVexNum = m_Graph.GetVexNum();
    if (nVexNum == 0) {
        cout << "请先创建景区景点图！" << endl;
        return;
    }

    // 显示所有景点列表
    cout << "====================景点列表====================" << endl;
    for (int i = 0; i < nVexNum; i++) {
        Vex v = m_Graph.GetVex(i);
        cout << v.num << "-" << v.name << endl;
    }
    cout << "================================================" << endl;

    // 输入要查询的景点编号
    int vNum;
    cout << "请输入想要查询的景点编号：";
    cin >> vNum;
    if (vNum < 0 || vNum >= nVexNum) {
        cout << "景点编号不存在，查询失败！" << endl;
        return;
    }

    // 输出当前景点详情
    Vex curVex = m_Graph.GetVex(vNum);
    cout << "====================景点详情====================" << endl;
    cout << "景点名字：" << curVex.name << endl;
    cout << "景点介绍：" << curVex.desc << endl;

    // 查找并输出邻接景点及距离
    Edge aEdge[MAX_VERTEX_NUM];
    int nEdgeNum = m_Graph.FindEdge(vNum, aEdge);
    cout << "周边景区-----" << endl;
    for (int i = 0; i < nEdgeNum; i++) {
        Vex adjVex = m_Graph.GetVex(aEdge[i].vex2);
        cout << curVex.name << "->" << adjVex.name << " " << aEdge[i].weight << "m" << endl;
    }
    cout << "================================================" << endl;
}

// 旅游景点导航功能实现
void TravelPath(void) {
    int nVexNum = m_Graph.GetVexNum();
    if (nVexNum == 0) {
        cout << "请先创建景区景点图！" << endl;
        return;
    }

    // 显示所有景点列表，供用户选择起始点
    cout << "====================旅游景点导航====================" << endl;
    cout << "景区景点列表：" << endl;
    for (int i = 0; i < nVexNum; i++) {
        Vex v = m_Graph.GetVex(i);
        cout << i << "-" << v.name << endl;
    }

    // 输入起始景点编号
    int nStartVex;
    cout << "请输入起始景点编号：";
    cin >> nStartVex;
    if (nStartVex < 0 || nStartVex >= nVexNum) {
        cout << "起始景点编号不存在，导航失败！" << endl;
        cout << "====================================================" << endl;
        return;
    }

    // 创建路径链表，调用DFS遍历生成所有路线
    PathList pPathList = CreatePathList();
    m_Graph.DFSTraverse(nStartVex, pPathList);

    // 输出所有导航路线
    cout << "导游路线为：" << endl;
    PathNode* pCur = pPathList->next;
    int nRouteNum = 0;
    while (pCur != NULL) {
        nRouteNum++;
        cout << "路线" << nRouteNum << "：";
        // 遍历当前路线的所有顶点，输出景点名称
        for (int i = 0; pCur->vexs[i] != -1; i++) {
            Vex v = m_Graph.GetVex(pCur->vexs[i]);
            if (i > 0) {
                cout << "->";
            }
            cout << v.name;
        }
        cout << endl;
        pCur = pCur->next;
    }

    // 无有效路线的情况处理
    if (nRouteNum == 0) {
        cout << "未找到有效游览路线！" << endl;
    }

    // 释放路径链表内存
    FreePathList(pPathList);
    cout << "====================================================" << endl;
}

// 搜索最短路径 - 界面交互+调用Dijkstra算法+输出结果
void FindShortPath(void) {
    int nVexNum = m_Graph.GetVexNum();
    if (nVexNum == 0) {
        cout << "请先创建景区景点图！" << endl;
        return;
    }

    // 显示景点列表
    cout << "====================搜索最短路径====================" << endl;
    cout << "景区景点列表：" << endl;
    for (int i = 0; i < nVexNum; i++) {
        Vex v = m_Graph.GetVex(i);
        cout << i << "-" << v.name << endl;
    }

    // 输入起点和终点编号
    int nStart, nEnd;
    cout << "请输入起点的编号：";
    cin >> nStart;
    cout << "请输入终点的编号：";
    cin >> nEnd;

    // 合法性校验
    if (nStart < 0 || nStart >= nVexNum || nEnd < 0 || nEnd >= nVexNum) {
        cout << "景点编号无效，查询失败！" << endl;
        cout << "====================================================" << endl;
        return;
    }

    // 调用Dijkstra算法求最短路径
    int path[MAX_VERTEX_NUM];
    int minDist = m_Graph.Dijkstra(nStart, nEnd, path);
    if (minDist == -1) {
        cout << "两点之间无有效路径！" << endl;
        cout << "====================================================" << endl;
        return;
    }

    // 输出最短路径（path是终点->起点，需逆序输出）
    cout << "最短路线为：";
    int i = 0;
    while (path[i] != -1) i++; // 找到路径长度
    for (int j = i - 1; j >= 0; j--) {
        Vex v = m_Graph.GetVex(path[j]);
        cout << v.name;
        if (j > 0) cout << "->";
    }
    // 输出最短距离
    cout << endl << "最短距离为：" << minDist << "m" << endl;
    cout << "====================================================" << endl;
}

// 铺设电路规划 - 界面交互+调用Prim算法+输出最小生成树结果
void DesignCircuitPath(void) {
    int nVexNum = m_Graph.GetVexNum();
    if (nVexNum == 0) {
        cout << "请先创建景区景点图！" << endl;
        return;
    }

    cout << "====================铺设电路规划====================" << endl;
    cout << "在以下两个景点之间铺设电路：" << endl;

    // 调用Prim算法构建最小生成树
    Edge treeEdges[MAX_VERTEX_NUM]; // 存储生成树的边（n-1条）
    int totalWeight = m_Graph.Prim(treeEdges);

    // 输出每条铺设的道路及长度
    for (int i = 0; i < nVexNum - 1; i++) {
        Vex v1 = m_Graph.GetVex(treeEdges[i].vex1);
        Vex v2 = m_Graph.GetVex(treeEdges[i].vex2);
        cout << v1.name << "->" << v2.name << "  " << treeEdges[i].weight << "m" << endl;
    }

    // 输出总铺设长度
    cout << "铺设电路的总长度为：" << totalWeight << "m" << endl;
    cout << "====================================================" << endl;
}