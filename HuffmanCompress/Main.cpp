#define _CRT_SECURE_NO_WARNINGS
#include "compress.h"
#include "decompress.h"
#include <iostream>
using namespace std;

int main() {
    cout << "========== 哈夫曼文件压缩/解压程序 ==========" << endl;
    cout << "请选择操作：1-压缩  2-解压" << endl;
    int choice;
    cin >> choice;

    if (choice == 1) {
        cout << "请输入待压缩文件的完整路径（如F:\\Pic.bmp）：";
        char filename[512] = { 0 };
        cin >> filename;

        if (Compress(filename) == OK) {
            cout << "文件压缩成功！" << endl;
        }
        else {
            cout << "文件压缩失败！" << endl;
        }
    }
    else if (choice == 2) {
        cout << "请输入待解压文件的完整路径（如F:\\Pic.bmp.huf）：";
        char filename[512] = { 0 };
        cin >> filename;

        HEAD sHead;
        if (ReadHead(filename, sHead) == ERROR) {
            cout << "读取文件头失败，可能不是有效的HUF文件！" << endl;
            system("pause");
            return -1;
        }

        HuffmanTree HT = NULL;
        CreateHuffmanTree(HT, sHead.weight, SIZE);

        char* pSrcBuffer = NULL;
        if (Decode(filename, HT, sHead, pSrcBuffer) == ERROR) {
            cout << "解码失败！" << endl;
            HuffmanCode dummyHC = NULL;
            DestroyHuffman(HT, dummyHC, SIZE);
            system("pause");
            return -1;
        }

        char outFilename[512] = { 0 };
        strcpy(outFilename, filename);
        char* p = strstr(outFilename, ".huf");
        if (p != NULL) {
            *p = '\0';
        }
        else {
            strcat(outFilename, "_decompressed");
        }

        if (WriteSrcFile(outFilename, pSrcBuffer, sHead.length) == ERROR) {
            cout << "写入解压文件失败！" << endl;
            delete[] pSrcBuffer;
            HuffmanCode dummyHC = NULL;
            DestroyHuffman(HT, dummyHC, SIZE);
            system("pause");
            return -1;
        }

        cout << "文件解压成功！生成文件：" << outFilename << endl;

        delete[] pSrcBuffer;
        HuffmanCode dummyHC = NULL;
        DestroyHuffman(HT, dummyHC, SIZE);
    }
    else {
        cout << "无效选择！" << endl;
    }

    system("pause");
    return 0;
}