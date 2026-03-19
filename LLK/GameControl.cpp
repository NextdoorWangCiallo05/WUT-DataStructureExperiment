#include "pch.h"
#include "GameControl.h"
#include<iostream>
#include <algorithm>
#include <vector>

using namespace std;

// 静态常量初始化
//int CGameControl::s_nRows = GAME_ROWS;
//int CGameControl::s_nCols = GAME_COLS;
//int CGameControl::s_nPicNum = PIC_NUM;


CGameControl::CGameControl()
{
    m_pGameMap = NULL;
    m_gameMode = MODE_CLASSIC; // 默认经典模式
}

CGameControl::~CGameControl()
{
    EndGame();
}

// 设置游戏模式
void CGameControl::SetGameMode(GameMode mode)
{
    m_gameMode = mode;

    if (mode == MODE_RELAX)
    {
        GAME_ROWS = GAME_ROWS_RELAX;
        GAME_COLS = GAME_COLS_RELAX;
    }
    else if (mode == MODE_TIMED)
    {
        GAME_ROWS = GAME_ROWS_TIMED;
        GAME_COLS = GAME_COLS_TIMED;
    }
    else // MODE_CLASSIC
    {
        GAME_ROWS = GAME_ROWS_CLASSIC;
        GAME_COLS = GAME_COLS_CLASSIC;
    }
}

// 获取当前模式
GameMode CGameControl::GetGameMode()
{
    return m_gameMode;
}


// 开始游戏
void CGameControl::StartGame()
{
    // 先释放旧地图
    EndGame();
    // 初始化新地图
    // m_pGameMap = m_gameLogic.InitMap();
    m_pGameMap = m_gameLogic.InitMap(GAME_ROWS, GAME_COLS);
}

// 结束游戏
void CGameControl::EndGame()
{
    m_gameLogic.ReleaseMap(m_pGameMap);
    memset(&m_svSelFst, 0, sizeof(m_svSelFst));
    memset(&m_svSelSec, 0, sizeof(m_svSelSec));
}

// 获取图案编号
int CGameControl::GetElement(int nRow, int nCol)
{
    if (m_pGameMap == NULL || nRow < 0 || nRow >= GAME_ROWS || nCol < 0 || nCol >= GAME_COLS)
        return BLANK;
    return m_pGameMap[nRow][nCol];
}

// 设置选中点
void CGameControl::SetFirstPoint(int nRow, int nCol)
{
    // 越界校验
    if (nRow < 0 || nRow >= GAME_ROWS || nCol < 0 || nCol >= GAME_COLS)
    {
        memset(&m_svSelFst, 0, sizeof(m_svSelFst));
        return;
    }
    m_svSelFst.row = nRow;
    m_svSelFst.col = nCol;
    m_svSelFst.info = GetElement(nRow, nCol);
}

void CGameControl::SetSecPoint(int nRow, int nCol)
{
    if (nRow < 0 || nRow >= GAME_ROWS || nCol < 0 || nCol >= GAME_COLS)
    {
        memset(&m_svSelSec, 0, sizeof(m_svSelSec));
        return;
    }
    m_svSelSec.row = nRow;
    m_svSelSec.col = nCol;
    m_svSelSec.info = GetElement(nRow, nCol);
}

// 消子核心逻辑
bool CGameControl::Link(Vertex avPath[4], int& nVexNum)
{
    if (m_pGameMap == NULL) return false;

    // 调用逻辑层连通判断
    bool bSuc = m_gameLogic.IsLink(m_pGameMap, m_svSelFst, m_svSelSec, avPath, nVexNum, GAME_ROWS, GAME_COLS);
    if (bSuc)
    {
        // 消子，将两个点设为空白
        m_pGameMap[m_svSelFst.row][m_svSelFst.col] = BLANK;
        m_pGameMap[m_svSelSec.row][m_svSelSec.col] = BLANK;
    }
    // 清空选中点
    memset(&m_svSelFst, 0, sizeof(m_svSelFst));
    memset(&m_svSelSec, 0, sizeof(m_svSelSec));
    return bSuc;
}

// 判断游戏胜利
bool CGameControl::IsWin()
{
    return m_gameLogic.IsBlank(m_pGameMap, GAME_ROWS, GAME_COLS);
}

// 提示功能：遍历所有图案对，查找可消除的组合
bool CGameControl::FindTip(Vertex& v1, Vertex& v2, Vertex avPath[4], int& nVexNum)
{
    if (m_pGameMap == NULL) return false;

    // 遍历所有非空白的图案对
    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            if (m_pGameMap[i][j] == BLANK) continue;

            // 初始化第一个候选点
            Vertex tipV1 = { i, j, m_pGameMap[i][j] };
            // 遍历后续图案，避免重复判断
            for (int m = i; m < GAME_ROWS; m++)
            {
                int nStartCol = (m == i) ? j + 1 : 0;
                for (int n = nStartCol; n < GAME_COLS; n++)
                {
                    if (m_pGameMap[m][n] == BLANK) continue;
                    // 初始化第二个候选点
                    Vertex tipV2 = { m, n, m_pGameMap[m][n] };
                    // 调用逻辑层判断是否可连通
                    if (m_gameLogic.IsLink(m_pGameMap, tipV1, tipV2, avPath, nVexNum, GAME_ROWS, GAME_COLS))
                    {
                        v1 = tipV1;
                        v2 = tipV2;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// 重排功能：打乱剩余图案的位置
void CGameControl::ShuffleMap()
{
    if (m_pGameMap == NULL) return;

    // 1. 收集所有非空白的图案及其坐标
    std::vector<Vertex> vecValid;
    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            if (m_pGameMap[i][j] != BLANK)
            {
                Vertex v = { i, j, m_pGameMap[i][j] };
                vecValid.push_back(v);
            }
        }
    }
    if (vecValid.empty()) return;

    // 2. 随机打乱图案顺序
    srand((unsigned int)time(NULL));
    std::random_shuffle(vecValid.begin(), vecValid.end());

    // 3. 清空原地图非空白位置，重新填充打乱后的图案
    int idx = 0;
    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            if (m_pGameMap[i][j] != BLANK)
            {
                m_pGameMap[i][j] = vecValid[idx++].info;
            }
        }
    }
}