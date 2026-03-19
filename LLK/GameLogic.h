#pragma once
#include "global.h"

class CGameControl;

class CGameLogic
{
public:
    CGameLogic();
    ~CGameLogic();

    // 初始化游戏地图，返回二维数组
    int** InitMap(int rows, int cols);
    // 释放地图内存
    void ReleaseMap(int**& pGameMap);
    // 判断两个点是否可连通，avPath返回路径，nVexNum返回路径点数
    bool IsLink(int** pGameMap, Vertex v1, Vertex v2, Vertex avPath[4], int& nVexNum, int rows, int cols); // Updated
    bool IsBlank(int** pGameMap, int rows, int cols); // Updated

    bool IsBlankPoint(int** pGameMap, Vertex point);

    bool IsValidPath(int** pGameMap, Vertex v1, Vertex v2, bool isHorizontal);



private:
    Vertex m_avPath[4];  // 存储连通路径，最多2个拐点+起点终点，共4个点
    int m_nVexNum;       // 路径点数

    // 一个拐点（两条直线）连通判断
    bool LinkInRow(int** pGameMap, Vertex v1, Vertex v2, int cols); // Updated
    bool LinkInCol(int** pGameMap, Vertex v1, Vertex v2, int rows); // Updated

    bool CGameLogic::OneCornerLink(int** pGameMap, Vertex v1, Vertex v2);
	bool CGameLogic::TwoCornerLink(int** pGameMap, Vertex v1, Vertex v2);

    // Update generic functions to use dynamic rows and cols
    bool LineX(int** pGameMap, int nRow, int nCol1, int nCol2, int cols); // Updated
    bool LineY(int** pGameMap, int nRow1, int nRow2, int nCol, int rows); // Updated
    // 路径顶点入栈
    void PushVertex(Vertex v);
    // 清空路径
    void ClearPath();
};