#ifndef COMPRESS_H
#define COMPRESS_H

#include "huffman.h"
#include <cstdio>
#include <map>
#include <string>

#define SIZE 256
#define OK 1
#define ERROR 0

struct HEAD {
    char type[4];
    int length;
    int weight[SIZE];
};

// 压缩相关声明
int InitHead(const char* pFilename, HEAD& sHead);
char Str2byte(const char* pBinStr);
int Encode(const char* pFilename, HuffmanCode HC, char*& pBuffer, int& nBufSize);
int WriteFile(const char* pFilename, const HEAD& sHead, const char* pBuffer, int nBufSize);
int Compress(const char* pFilename);

#endif // COMPRESS_H