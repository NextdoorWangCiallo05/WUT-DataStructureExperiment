#pragma once
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

// 哈夫曼树节点结构体
struct HTNode {
    int weight;    // 权值
    int parent;    // 父节点
    int lchild;    // 左孩子
    int rchild;    // 右孩子
};
typedef HTNode* HuffmanTree; // 哈夫曼树类型

// 哈夫曼编码类型：字符串存储每个字节的编码
typedef char** HuffmanCode;

// 查找哈夫曼树中权值最小的两个未被选的节点
void Select(HuffmanTree HT, int n, int& s1, int& s2);

// 创建哈夫曼树
void CreateHuffmanTree(HuffmanTree& HT, int* weight, int n);

// 生成哈夫曼编码（从叶子到根逆向生成）
void CreateHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n);

// 释放哈夫曼树和编码的内存
void DestroyHuffman(HuffmanTree& HT, HuffmanCode& HC, int n);

#endif