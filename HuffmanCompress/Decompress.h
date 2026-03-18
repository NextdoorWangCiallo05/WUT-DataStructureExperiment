#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "Compress.h" // 添加此行以包含HEAD定义
#include <cstdio>

// 解压相关声明
int ReadHead(const char* pHufFilename, HEAD& sHead);
void Byte2Str(char byte, char* pBinStr);
int Decode(const char* pHufFilename, HuffmanTree HT, const HEAD& sHead, char*& pSrcBuffer);
int WriteSrcFile(const char* pSrcFilename, const char* pSrcBuffer, int nSrcSize);

#endif // DECOMPRESS_H