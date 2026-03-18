#include "Graph.h"
#include <cstdlib>
#include <climits>

// 在文件顶部添加INF宏定义，表示无穷大（用于最短路径算法）
#define INF INT_MAX

// 初始化图：邻接矩阵置0，顶点数置0
void CGraph::Init(void) {
    m_nVexNum = 0;
    for (int i = 0; i < MAX_VERTEX_NUM; i++) {
        for (int j = 0; j < MAX_VERTEX_NUM; j++) {
            m_aAdjMatrix[i][j] = 0; // 0表示无边
        }
    }
}

// 插入顶点：校验顶点数是否满，不满则插入
bool CGraph::InsertVex(Vex sVex) {
    if (m_nVexNum >= MAX_VERTEX_NUM) {
        cout << "顶点数目达到上限，插入失败！" << endl;
        return false;
    }
    m_aVexs[m_nVexNum++] = sVex;
    return true;
}

// 插入边：无向图，邻接矩阵双向赋值，做下标越界校验
bool CGraph::InsertEdge(Edge sEdge) {
    if (sEdge.vex1 < 0 || sEdge.vex1 >= m_nVexNum ||
        sEdge.vex2 < 0 || sEdge.vex2 >= m_nVexNum) {
        cout << "顶点编号越界，边插入失败！" << endl;
        return false;
    }
    // 无向图，双向设置权值
    m_aAdjMatrix[sEdge.vex1][sEdge.vex2] = sEdge.weight;
    m_aAdjMatrix[sEdge.vex2][sEdge.vex1] = sEdge.weight;
    return true;
}

// 根据编号获取顶点信息
Vex CGraph::GetVex(int v) {
    return m_aVexs[v];
}

// 查找指定顶点的所有邻边，将邻边存入aEdge数组，返回邻边数量
int CGraph::FindEdge(int v, Edge aEdge[]) {
    if (v < 0 || v >= m_nVexNum) {
        return 0;
    }
    int k = 0;
    for (int i = 0; i < m_nVexNum; i++) {
        if (m_aAdjMatrix[v][i] != 0) { // 邻接矩阵值非0表示有边
            aEdge[k].vex1 = v;
            aEdge[k].vex2 = i;
            aEdge[k].weight = m_aAdjMatrix[v][i];
            k++;
        }
    }
    return k;
}

// 创建空的路径链表，返回头节点
PathList CreatePathList() {
    PathNode* pHead = (PathNode*)malloc(sizeof(PathNode));
    pHead->next = NULL;
    memset(pHead->vexs, -1, sizeof(pHead->vexs)); // 初始化顶点编号为-1（无效值）
    return pHead;
}

// 向路径链表添加一条路线
void AddPath(PathList& pList, int aPath[], int nLen) {
    PathNode* pNewNode = (PathNode*)malloc(sizeof(PathNode));
    // 复制路线顶点编号到新节点
    for (int i = 0; i < nLen; i++) {
        pNewNode->vexs[i] = aPath[i];
    }
    memset(pNewNode->vexs + nLen, -1, sizeof(pNewNode->vexs) - nLen * sizeof(int));
    pNewNode->next = pList->next;
    pList->next = pNewNode;
}

// 释放路径链表的内存，防止内存泄漏
void FreePathList(PathList& pList) {
    PathNode* pCur = pList;
    while (pCur != NULL) {
        PathNode* pTemp = pCur;
        pCur = pCur->next;
        free(pTemp);
    }
    pList = NULL;
}

// DFS核心函数，递归遍历+回溯，生成所有游览路线
void CGraph::DFS(int nVex, bool bVisited[], int aPath[], int nIndex, PathList& pList) {
    bVisited[nVex] = true;          // 标记当前顶点为已访问
    aPath[nIndex++] = nVex;         // 将当前顶点加入路径，遍历深度+1

    // 判断是否遍历完所有顶点：是则保存该路线
    bool bAllVisited = true;
    for (int i = 0; i < m_nVexNum; i++) {
        if (!bVisited[i]) {
            bAllVisited = false;
            break;
        }
    }
    if (bAllVisited) {
        AddPath(pList, aPath, nIndex); // 保存有效路线
    }
    // 未遍历完，递归访问所有未访问的邻接顶点
    else {
        for (int i = 0; i < m_nVexNum; i++) {
            // 邻接矩阵值非0表示有边，且顶点未被访问
            if (m_aAdjMatrix[nVex][i] != 0 && !bVisited[i]) {
                DFS(i, bVisited, aPath, nIndex, pList); // 递归遍历
                bVisited[i] = false;                    // 回溯：标记为未访问
                // nIndex无需手动减1，递归返回后会自动恢复上一层的nIndex值
            }
        }
    }
}

// DFS遍历入口函数，初始化访问状态、路径数组，调用核心DFS
void CGraph::DFSTraverse(int nVex, PathList& pList) {
    if (nVex < 0 || nVex >= m_nVexNum) {
        cout << "起始景点编号无效！" << endl;
        return;
    }
    bool bVisited[MAX_VERTEX_NUM] = { false }; // 初始化所有顶点为未访问
    int aPath[MAX_VERTEX_NUM] = { -1 };        // 初始化路径数组为-1
    int nIndex = 0;                            // 初始化遍历深度为0
    DFS(nVex, bVisited, aPath, nIndex, pList); // 调用DFS核心函数
}

// Dijkstra算法 - 求nStart到nEnd的最短路径
// path数组：存储最短路径（从终点到起点的前驱序列），返回值：最短距离
int CGraph::Dijkstra(int nStart, int nEnd, int path[]) {
    // 合法性校验
    if (nStart < 0 || nStart >= m_nVexNum || nEnd < 0 || nEnd >= m_nVexNum) {
        return -1;
    }
    if (nStart == nEnd) {
        path[0] = nEnd;
        return 0;
    }

    int dist[MAX_VERTEX_NUM];    // 源点到各顶点的最短距离
    bool visited[MAX_VERTEX_NUM];// 顶点是否已加入最短路径集
    int preVex[MAX_VERTEX_NUM];  // 各顶点的前驱顶点编号
    // 初始化
    for (int i = 0; i < m_nVexNum; i++) {
        dist[i] = (m_aAdjMatrix[nStart][i] == 0) ? INF : m_aAdjMatrix[nStart][i];
        visited[i] = false;
        preVex[i] = (dist[i] != INF) ? nStart : -1;
    }
    visited[nStart] = true;
    dist[nStart] = 0;

    // 遍历除源点外的所有顶点
    for (int i = 1; i < m_nVexNum; i++) {
        // 步骤1：找到距离源点最近的未访问顶点u
        int minDist = INF;
        int u = -1;
        for (int j = 0; j < m_nVexNum; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break; // 图不连通（实验中为连通图，此情况不会出现）
        visited[u] = true;

        // 步骤2：以u为中间点，更新源点到其他顶点的最短距离
        for (int v = 0; v < m_nVexNum; v++) {
            if (!visited[v] && m_aAdjMatrix[u][v] != 0 && dist[u] + m_aAdjMatrix[u][v] < dist[v]) {
                dist[v] = dist[u] + m_aAdjMatrix[u][v];
                preVex[v] = u;
            }
        }
    }

    // 步骤3：通过前驱数组回溯，生成最短路径（path：终点->起点）
    int p = nEnd;
    int pathIndex = 0;
    while (p != -1) {
        path[pathIndex++] = p;
        p = preVex[p];
    }
    path[pathIndex] = -1; // 路径结束标记

    // 返回最短距离（若为INF表示无路径，实验中为连通图，此情况不会出现）
    return dist[nEnd] == INF ? -1 : dist[nEnd];
}

// Prim算法 - 构建最小生成树，以顶点0为起始点
// treeEdges：存储生成树的所有边，返回值：生成树的总权值
int CGraph::Prim(Edge treeEdges[]) {
    if (m_nVexNum == 0) return 0;
    int totalWeight = 0;        // 最小生成树总权值
    int edgeCount = 0;          // 生成树的边数（n个顶点的生成树有n-1条边）
    int dist[MAX_VERTEX_NUM];   // 非生成树顶点到生成树的最小距离
    int preVex[MAX_VERTEX_NUM]; // 非生成树顶点的前驱顶点（在生成树中）
    bool inTree[MAX_VERTEX_NUM];// 顶点是否已加入生成树

    // 初始化
    for (int i = 0; i < m_nVexNum; i++) {
        dist[i] = (m_aAdjMatrix[0][i] == 0) ? INF : m_aAdjMatrix[0][i];
        preVex[i] = (dist[i] != INF) ? 0 : -1;
        inTree[i] = false;
    }
    inTree[0] = true; // 起始顶点0加入生成树
    dist[0] = 0;

    // 遍历除起始点外的所有顶点，构建生成树
    for (int i = 1; i < m_nVexNum; i++) {
        // 步骤1：找到非生成树中到生成树距离最小的顶点u
        int minDist = INF;
        int u = -1;
        for (int j = 0; j < m_nVexNum; j++) {
            if (!inTree[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break; // 图不连通（实验中为连通图，此情况不会出现）

        // 步骤2：将边(preVex[u], u)加入生成树
        treeEdges[edgeCount].vex1 = preVex[u];
        treeEdges[edgeCount].vex2 = u;
        treeEdges[edgeCount].weight = minDist;
        edgeCount++;
        totalWeight += minDist;
        inTree[u] = true;

        // 步骤3：更新非生成树顶点到生成树的最小距离
        for (int v = 0; v < m_nVexNum; v++) {
            if (!inTree[v] && m_aAdjMatrix[u][v] != 0 && m_aAdjMatrix[u][v] < dist[v]) {
                dist[v] = m_aAdjMatrix[u][v];
                preVex[v] = u;
            }
        }
    }
    return totalWeight;
}