#pragma once

#include "GameLogic.h"
#include "global.h" 

class CGameControl
{
public:
    CGameControl();
    ~CGameControl();

    // 新增：模式相关
    void SetGameMode(GameMode mode); // 设置游戏模式
    GameMode GetGameMode();          // 获取当前模式


    // 静态常量，全局地图配置
    static int s_nRows;
    static int s_nCols;
    static int s_nPicNum;

    // 开始游戏，初始化地图
    void StartGame();
    // 结束游戏，释放资源
    void EndGame();
    // 获取指定位置的图案编号
    int GetElement(int nRow, int nCol);
    // 设置选中的点
    void SetFirstPoint(int nRow, int nCol);
    void SetSecPoint(int nRow, int nCol);
    // 消子判断，返回是否成功，avPath返回连通路径
    bool Link(Vertex avPath[4], int& nVexNum);
    // 判断游戏是否胜利
    bool IsWin();

    // 提示功能 - 查找可消除的一对图案
    bool FindTip(Vertex& v1, Vertex& v2, Vertex avPath[4], int& nVexNum);
    // 重排功能 - 打乱剩余图案
    void ShuffleMap();

private:
    int** m_pGameMap;    // 游戏地图二维数组
    Vertex m_svSelFst;    // 第一个选中的点
    Vertex m_svSelSec;    // 第二个选中的点
    CGameLogic m_gameLogic; // 游戏逻辑对象
    GameMode m_gameMode;
};