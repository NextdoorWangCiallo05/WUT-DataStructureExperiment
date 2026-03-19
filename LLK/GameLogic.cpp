#include "pch.h"
#include "GameLogic.h"

#include <vector>
#include <algorithm>

using namespace std;

CGameLogic::CGameLogic()
{
    m_nVexNum = 0;
    memset(m_avPath, 0, sizeof(m_avPath));
}

CGameLogic::~CGameLogic()
{
}

// 初始化地图
int** CGameLogic::InitMap(int rows, int cols) {
    // 1) 动态分配二维数组
    int** pGameMap = new int* [rows];
    for (int i = 0; i < rows; i++) {
        pGameMap[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            pGameMap[i][j] = BLANK;
        }
    }

    // 2) 计算总格数，必须为偶数（能配对）
    const int nTotal = rows * cols;
    if (nTotal % 2 != 0) { ReleaseMap(pGameMap); return NULL; }

    // 3) 确定使用的图案种类数（不超过 PIC_NUM，也不超过可配对数）
    const int picCount = min(PIC_NUM, nTotal / 2);

    // 4) 构建成对的图案池，尽量均匀分布
    std::vector<int> pool;
    pool.reserve(nTotal);

    // 每种图案的基础配对次数
    int basePairs = (nTotal / 2) / picCount;   // 每种至少有这么多对
    int extraPairs = (nTotal / 2) % picCount;   // 前 extraPairs 种多 1 对

    for (int id = 0; id < picCount; ++id) {
        int pairCnt = basePairs + (id < extraPairs ? 1 : 0);
        for (int k = 0; k < pairCnt; ++k) {
            pool.push_back(id);
            pool.push_back(id);
        }
    }

    // 5) 随机打乱
    srand((unsigned int)time(NULL));
    std::random_shuffle(pool.begin(), pool.end());

    // 6) 回填到地图
    for (int idx = 0; idx < nTotal; ++idx) {
        pGameMap[idx / cols][idx % cols] = pool[idx];
    }

    return pGameMap;
}

// 释放地图内存
void CGameLogic::ReleaseMap(int**& pGameMap)
{
    if (pGameMap == NULL) return;
    for (int i = 0; i < GAME_ROWS; i++)
    {
        delete[] pGameMap[i];
    }
    delete[] pGameMap;
    pGameMap = NULL;
}

// 横向连通判断
bool CGameLogic::LinkInRow(int** pGameMap, Vertex v1, Vertex v2, int cols)
{
    if (v1.row != v2.row) return false;
    // 补充坐标越界校验
    if (v1.col < 0 || v1.col >= GAME_COLS || v2.col < 0 || v2.col >= GAME_COLS)
        return false;

    int nCol1 = min(v1.col, v2.col);
    int nCol2 = max(v1.col, v2.col);

    // 遍历两个点之间的列（不包含起止列），确保无障碍物
    for (int i = nCol1 + 1; i < nCol2; i++)
    {
        // 补充行越界校验
        if (v1.row < 0 || v1.row >= GAME_ROWS)
            return false;
        if (pGameMap[v1.row][i] != BLANK)
            return false;
    }
    return true;
}

// 纵向连通判断
bool CGameLogic::LinkInCol(int** pGameMap, Vertex v1, Vertex v2, int rows)
{
    if (v1.col != v2.col) return false;
    // 坐标越界校验
    if (v1.row < 0 || v1.row >= GAME_ROWS || v2.row < 0 || v2.row >= GAME_ROWS)
        return false;

    int nRow1 = min(v1.row, v2.row);
    int nRow2 = max(v1.row, v2.row);

    for (int i = nRow1 + 1; i < nRow2; i++)
    {
        if (v1.col < 0 || v1.col >= GAME_COLS)
            return false;
        if (pGameMap[i][v1.col] != BLANK)
            return false;
    }
    return true;
}

// 通用横向直线判断
bool CGameLogic::LineX(int** pGameMap, int nRow, int nCol1, int nCol2, int cols)
{
    // 地图外的列直接判定为空白
    if (nRow < 0 || nRow >= GAME_ROWS) return true;

    int nMinCol = min(nCol1, nCol2);
    int nMaxCol = max(nCol1, nCol2);

    for (int i = nMinCol + 1; i < nMaxCol; i++)
    {
        // 列超出地图范围则跳过（视为空白）
        if (i < 0 || i >= GAME_COLS) continue;
        if (pGameMap[nRow][i] != BLANK)
            return false;
    }
    return true;
}

// 通用纵向直线判断
bool CGameLogic::LineY(int** pGameMap, int nRow1, int nRow2, int nCol, int rows)
{
    // 地图外的行直接判定为空白
    if (nCol < 0 || nCol >= GAME_COLS) return true;

    int nMinRow = min(nRow1, nRow2);
    int nMaxRow = max(nRow1, nRow2);

    for (int i = nMinRow + 1; i < nMaxRow; i++)
    {
        // 行超出地图范围则跳过（视为空白）
        if (i < 0 || i >= GAME_ROWS) continue;
        if (pGameMap[i][nCol] != BLANK)
            return false;
    }
    return true;
}

// 路径顶点入栈
void CGameLogic::PushVertex(Vertex v)
{
    if (m_nVexNum < 4) // 路径点数不得超过4
    {
        m_avPath[m_nVexNum++] = v;
    }
}

// 清空路径
void CGameLogic::ClearPath()
{
    m_nVexNum = 0;
    memset(m_avPath, 0, sizeof(m_avPath));
}

// 一个拐点连通判断
// 修复：一个拐点连通判断（补充地图外拐点支持+完整路径校验）
bool CGameLogic::OneCornerLink(int** pGameMap, Vertex v1, Vertex v2)
{
    // 拐点1：v1.row, v2.col（横后纵）
    Vertex vCorner1 = { v1.row, v2.col, BLANK };
    // 校验拐点是否合法（地图内则必须空白，地图外直接合法）
    bool bCorner1Valid = (vCorner1.row < 0 || vCorner1.row >= GAME_ROWS ||
        vCorner1.col < 0 || vCorner1.col >= GAME_COLS) ||
        (pGameMap[vCorner1.row][vCorner1.col] == BLANK);

    if (bCorner1Valid)
    {
        // 校验v1→拐点（横向）、拐点→v2（纵向）
        if (LineX(pGameMap, v1.row, v1.col, v2.col, GAME_COLS) &&
            LineY(pGameMap, v1.row, v2.row, v2.col, GAME_ROWS))
        {
            PushVertex(vCorner1);
            return true;
        }
    }

    // 拐点2：v2.row, v1.col（纵后横）
    Vertex vCorner2 = { v2.row, v1.col, BLANK };
    bool bCorner2Valid = (vCorner2.row < 0 || vCorner2.row >= GAME_ROWS ||
        vCorner2.col < 0 || vCorner2.col >= GAME_COLS) ||
        (pGameMap[vCorner2.row][vCorner2.col] == BLANK);

    if (bCorner2Valid)
    {
        // 校验v1→拐点（纵向）、拐点→v2（横向）
        if (LineY(pGameMap, v1.row, v2.row, v1.col, GAME_ROWS) &&
            LineX(pGameMap, v2.row, v1.col, v2.col, GAME_COLS))
        {
            PushVertex(vCorner2);
            return true;
        }
    }

    return false;
}

// 两个拐点连通判断
bool CGameLogic::TwoCornerLink(int** pGameMap, Vertex v1, Vertex v2)
{
    // ===== 扫描列（垂直拐点）：覆盖地图外列 =====
    // 扩展扫描范围：-GAME_COLS ~ 2*GAME_COLS（包含地图外）
    for (int col = -GAME_COLS; col < 2 * GAME_COLS; col++)
    {
        Vertex vCorner1 = { v1.row, col, BLANK };
        Vertex vCorner2 = { v2.row, col, BLANK };

        // 拐点合法：地图内则空白，地图外则直接合法
        bool bCorner1Valid = (vCorner1.row < 0 || vCorner1.row >= GAME_ROWS ||
            vCorner1.col < 0 || vCorner1.col >= GAME_COLS) ||
            (pGameMap[vCorner1.row][vCorner1.col] == BLANK);
        bool bCorner2Valid = (vCorner2.row < 0 || vCorner2.row >= GAME_ROWS ||
            vCorner2.col < 0 || vCorner2.col >= GAME_COLS) ||
            (pGameMap[vCorner2.row][vCorner2.col] == BLANK);

        if (bCorner1Valid && bCorner2Valid)
        {
            // 校验三段路径：v1→拐点1 → 拐点1→拐点2 → 拐点2→v2
            if (LineX(pGameMap, v1.row, v1.col, col, GAME_COLS) &&
                LineY(pGameMap, v1.row, v2.row, col, GAME_ROWS) &&
                LineX(pGameMap, v2.row, col, v2.col, GAME_COLS))
            {
                PushVertex(vCorner1);
                PushVertex(vCorner2);
                return true;
            }
        }
    }

    // ===== 扫描行（水平拐点）：覆盖地图外行 =====
    // 扩展扫描范围：-GAME_ROWS ~ 2*GAME_ROWS（包含地图外）
    for (int row = -GAME_ROWS; row < 2 * GAME_ROWS; row++)
    {
        Vertex vCorner1 = { row, v1.col, BLANK };
        Vertex vCorner2 = { row, v2.col, BLANK };

        bool bCorner1Valid = (vCorner1.row < 0 || vCorner1.row >= GAME_ROWS ||
            vCorner1.col < 0 || vCorner1.col >= GAME_COLS) ||
            (pGameMap[vCorner1.row][vCorner1.col] == BLANK);
        bool bCorner2Valid = (vCorner2.row < 0 || vCorner2.row >= GAME_ROWS ||
            vCorner2.col < 0 || vCorner2.col >= GAME_COLS) ||
            (pGameMap[vCorner2.row][vCorner2.col] == BLANK);

        if (bCorner1Valid && bCorner2Valid)
        {
            if (LineY(pGameMap, v1.row, row, v1.col, GAME_ROWS) &&
                LineX(pGameMap, row, v1.col, v2.col, GAME_COLS) &&
                LineY(pGameMap, row, v2.row, v2.col, GAME_ROWS))
            {
                PushVertex(vCorner1);
                PushVertex(vCorner2);
                return true;
            }
        }
    }

    return false;
}

// 核心连通判断
bool CGameLogic::IsLink(int** pGameMap, Vertex v1, Vertex v2, Vertex avPath[4], int& nVexNum, int rows, int cols)
{
    ClearPath();
    PushVertex(v1); // 起点入栈

    // 1. 先判断两个点是否相同，图案是否一致
    if (v1.row == v2.row && v1.col == v2.col)
        return false;
    if (pGameMap[v1.row][v1.col] != pGameMap[v2.row][v2.col])
        return false;

    // 2. 一条直线连通判断
    if (LinkInRow(pGameMap, v1, v2, GAME_COLS) || LinkInCol(pGameMap, v1, v2, GAME_ROWS))
    {
        PushVertex(v2);
        // 输出路径
        nVexNum = m_nVexNum;
        memcpy(avPath, m_avPath, sizeof(Vertex) * m_nVexNum);
        return true;
    }

    // 3. 两条直线（一个拐点）连通判断
    if (OneCornerLink(pGameMap, v1, v2))
    {
        PushVertex(v2);
        nVexNum = m_nVexNum;
        memcpy(avPath, m_avPath, sizeof(Vertex) * m_nVexNum);
        return true;
    }

    // 4. 三条直线（两个拐点）连通判断
    if (TwoCornerLink(pGameMap, v1, v2))
    {
        PushVertex(v2);
        nVexNum = m_nVexNum;
        memcpy(avPath, m_avPath, sizeof(Vertex) * m_nVexNum);
        return true;
    }

    ClearPath();
    return false;
}

// 判断地图是否全空白
bool CGameLogic::IsBlank(int** pGameMap, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (pGameMap[i][j] != BLANK) return false;
        }
    }
    return true;
}

// 检查点是否为空白位置
bool CGameLogic::IsBlankPoint(int** pGameMap, Vertex point)
{
    return pGameMap[point.row][point.col] == BLANK;
}

// 检查两点之间是否形成有效路径
bool CGameLogic::IsValidPath(int** pGameMap, Vertex v1, Vertex v2, bool isHorizontal)
{
    if (isHorizontal)
    {
        // 横向路径：需要行一致，列逐步扫描
        if (v1.row != v2.row) return false; // 必须在同一行
        return LineX(pGameMap, v1.row, v1.col, v2.col, GAME_COLS); // 调用横向路径判定
    }
    else
    {
        // 纵向路径：需要列一致，行逐步扫描
        if (v1.col != v2.col) return false; // 必须在同一列
        return LineY(pGameMap, v1.row, v2.row, v1.col, GAME_ROWS); // 调用纵向路径判定
    }
}

