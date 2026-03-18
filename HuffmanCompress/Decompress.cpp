#define _CRT_SECURE_NO_WARNINGS
#include "decompress.h"
#include "compress.h"      // 仅需 HEAD 结构定义（也可单独定义，这里复用）
#include <map>
#include <string>
#include <cstring>
using namespace std;

int ReadHead(const char* pHufFilename, HEAD& sHead) {
    FILE* pFile = fopen(pHufFilename, "rb");
    if (!pFile) {
        cerr << "压缩文件打开失败！" << endl;
        return ERROR;
    }

    fread(&sHead, sizeof(HEAD), 1, pFile);
    fclose(pFile);
    pFile = NULL;

    if (strncmp(sHead.type, "HUF", 3) != 0) {
        cerr << "文件不是合法的HUF压缩文件！" << endl;
        return ERROR;
    }
    return OK;
}

void Byte2Str(char byte, char* pBinStr) {
    memset(pBinStr, '0', 8);
    pBinStr[8] = '\0';

    for (int i = 7; i >= 0; i--) {
        if (byte & (1 << i)) {
            pBinStr[7 - i] = '1';
        }
    }
}

int Decode(const char* pHufFilename, HuffmanTree HT, const HEAD& sHead, char*& pSrcBuffer) {
    pSrcBuffer = new char[sHead.length];
    memset(pSrcBuffer, 0, sHead.length);
    if (!pSrcBuffer) {
        cerr << "原始数据缓冲区开辟失败！" << endl;
        return ERROR;
    }

    FILE* pFile = fopen(pHufFilename, "rb");
    if (!pFile) {
        cerr << "压缩文件打开失败！" << endl;
        return ERROR;
    }
    fseek(pFile, sizeof(HEAD), SEEK_SET);

    // 重构编码映射表
    map<string, unsigned char> codeMap;
    HuffmanCode HC = NULL;
    CreateHuffmanCode(HT, HC, SIZE); // 生成编码表
    for (int i = 0; i < SIZE; i++) {
        codeMap[HC[i + 1]] = (unsigned char)i;
    }
    // 释放临时编码内存（注意：HC 是临时变量，释放时使用 dummy HT）
    HuffmanTree dummyHT = NULL;
    DestroyHuffman(dummyHT, HC, SIZE);

    char binStr[9] = { 0 };
    string tempCode = "";
    int srcPos = 0;
    int ch;

    while ((ch = fgetc(pFile)) != EOF && srcPos < sHead.length) {
        Byte2Str((char)ch, binStr);

        for (int i = 0; i < 8 && srcPos < sHead.length; i++) {
            tempCode += binStr[i];

            if (codeMap.find(tempCode) != codeMap.end()) {
                pSrcBuffer[srcPos++] = codeMap[tempCode];
                tempCode = "";
            }
        }
    }

    fclose(pFile);
    pFile = NULL;
    return OK;
}

int WriteSrcFile(const char* pSrcFilename, const char* pSrcBuffer, int nSrcSize) {
    FILE* pFile = fopen(pSrcFilename, "wb");
    if (!pFile) {
        cerr << "原始文件创建失败！" << endl;
        return ERROR;
    }

    fwrite(pSrcBuffer, sizeof(char), nSrcSize, pFile);
    fclose(pFile);
    pFile = NULL;
    cout << "生成解压文件：" << pSrcFilename << endl;
    return OK;
}