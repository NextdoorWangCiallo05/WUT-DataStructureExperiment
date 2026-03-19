#pragma once
#include <afxwin.h>

enum GameMode
{
    MODE_CLASSIC = 0,  // 经典模式（原尺寸）
    MODE_RELAX = 1,     // 休闲模式（小棋盘）
	MODE_TIMED = 2    // 计时模式（中棋盘）
};

// 经典模式尺寸
#define GAME_ROWS_CLASSIC 10
#define GAME_COLS_CLASSIC 16
// 休闲模式尺寸（缩小版）
#define GAME_ROWS_RELAX 8
#define GAME_COLS_RELAX 8
// 计时模式尺寸
#define GAME_ROWS_TIMED   10  
#define GAME_COLS_TIMED   12  
// 游戏地图配置
extern int GAME_ROWS;
extern int GAME_COLS;
#define PIC_NUM      16   // 图案种类数
#define ELEM_SIZE    40   // 每个图案像素大小
#define GAME_TOP_X   50   // 地图左上角X坐标
#define GAME_TOP_Y   50   // 地图左上角Y坐标

// 空白元素标记
#define BLANK        0

// 顶点结构体，存储地图中每个点的信息
typedef struct tagVertex
{
    int row;    // 行号
    int col;    // 列号
    int info;   // 图案编号，BLANK表示空白
}Vertex;