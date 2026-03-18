#include "Tourism.h"
#include<iostream>

using namespace std;

int main(void) {
    bool bRunning = true;
    int nChoice = 0;
    cout << "==========景区信息管理系统==========" << endl;
    while (bRunning) {
        cout << endl << "请选择操作编号（0-2）：" << endl;
        cout << "1. 创建景区景点图" << endl;
        cout << "2. 查询景点信息" << endl;
        cout << "3. 旅游景点导航" << endl;
        cout << "4. 搜索最短路径" << endl;
        cout << "5. 铺设电路规划" << endl;
        cout << "0. 退出系统" << endl;
        cout << "输入操作编号：";
        cin >> nChoice;

        // 根据选择执行对应功能
        switch (nChoice) {
        case 1:
            CreateGraph();
            break;
        case 2:
            GetSpotInfo();
            break;
        case 3:
            TravelPath();
            break;
        case 4:
            FindShortPath();
            break;
        case 5: 
            DesignCircuitPath();
            break;
        case 0:
            bRunning = false;
            cout << "已退出景区信息管理系统，感谢使用！" << endl;
            break;
        default:
            cout << "输入的操作编号无效，请重新输入！" << endl;
            break;
        }
    }
    system("pause");
    return 0;
}