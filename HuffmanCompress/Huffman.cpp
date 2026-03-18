#define _CRT_SECURE_NO_WARNINGS
#include "huffman.h"

// 查找两个权值最小的未使用节点 s1<s2
void Select(HuffmanTree HT, int n, int& s1, int& s2) {
    int i;
    for (i = 1; i <= n; i++) {
        if (HT[i].parent == 0) {
            s1 = i;
            break;
        }
    }
    for (i = 1; i <= n; i++) {
        if (HT[i].parent == 0 && HT[i].weight < HT[s1].weight) {
            s1 = i;
        }
    }
    for (i = 1; i <= n; i++) {
        if (HT[i].parent == 0 && i != s1) {
            s2 = i;
            break;
        }
    }
    for (i = 1; i <= n; i++) {
        if (HT[i].parent == 0 && i != s1 && HT[i].weight < HT[s2].weight) {
            s2 = i;
        }
    }
}

// 创建哈夫曼树：n为叶子节点数（固定256），weight为权值数组
void CreateHuffmanTree(HuffmanTree& HT, int* weight, int n) {
    if (n <= 1) return;
    int m = 2 * n - 1;
    HT = new HTNode[m + 1];

    for (int i = 1; i <= n; i++) {
        HT[i].weight = weight[i - 1];
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }
    for (int i = n + 1; i <= m; i++) {
        HT[i].weight = 0;
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }

    for (int i = n + 1; i <= m; i++) {
        int s1, s2;
        Select(HT, i - 1, s1, s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }
}

// 生成哈夫曼编码：从叶子到根逆向遍历
void CreateHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n) {
    HC = new char* [n + 1]; 
    char* cd = new char[n]; 
    cd[n - 1] = '\0'; 

    for (int i = 1; i <= n; i++) {
        int start = n - 1;
        int c = i;
        int f = HT[i].parent;
        while (f != 0) {
            start--;
            if (HT[f].lchild == c) {
                cd[start] = '0';
            }
            else {
                cd[start] = '1';
            }
            c = f;
            f = HT[f].parent;
        }
        HC[i] = new char[n - start];
        strcpy(HC[i], &cd[start]);
    }
    delete[] cd;
}

// 释放内存
void DestroyHuffman(HuffmanTree& HT, HuffmanCode& HC, int n) {
    if (HT) {
        delete[] HT;
        HT = NULL;
    }
    if (HC) {
        for (int i = 1; i <= n; i++) {
            delete[] HC[i];
        }
        delete[] HC;
        HC = NULL;
    }
}