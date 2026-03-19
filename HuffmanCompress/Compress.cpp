#define _CRT_SECURE_NO_WARNINGS
#include "compress.h"

int InitHead(const char* pFilename, HEAD& sHead) {
    strcpy(sHead.type, "HUF");
    sHead.length = 0;
    memset(sHead.weight, 0, sizeof(sHead.weight));

    FILE* pFile = fopen(pFilename, "rb");
    if (!pFile) {
        cerr << "文件打开失败！" << endl;
        return ERROR;
    }

    int ch;
    while ((ch = fgetc(pFile)) != EOF) {
        sHead.weight[(unsigned char)ch]++;
        sHead.length++;
    }
    fclose(pFile);
    pFile = NULL;

    if (sHead.length == 0) {
        cerr << "原文件为空！" << endl;
        return ERROR;
    }
    return OK;
}

char Str2byte(const char* pBinStr) {
    char b = 0x00;
    for (int i = 0; i < 8; i++) {
        b = b << 1;
        if (pBinStr[i] == '1') {
            b = b | 0x01;
        }
    }
    return b;
}

int Encode(const char* pFilename, HuffmanCode HC, char*& pBuffer, int& nBufSize) {
    FILE* pFile = fopen(pFilename, "rb");
    if (!pFile) {
        cerr << "文件打开失败！" << endl;
        return ERROR;
    }

    HEAD sHead;
    InitHead(pFilename, sHead);
    int nTotalBits = 0;
    for (int i = 0; i < SIZE; i++) {
        nTotalBits += sHead.weight[i] * strlen(HC[i + 1]);
    }
    nBufSize = (nTotalBits % 8 == 0) ? (nTotalBits / 8) : (nTotalBits / 8 + 1);
    pBuffer = new char[nBufSize];
    memset(pBuffer, 0, nBufSize);
    if (!pBuffer) {
        cerr << "缓冲区开辟失败！" << endl;
        fclose(pFile);
        return ERROR;
    }

    char cd[1024 * 10] = { 0 };
    int ch;
    while ((ch = fgetc(pFile)) != EOF) {
        strcat(cd, HC[(unsigned char)ch + 1]);
        while (strlen(cd) >= 8) {
            static int pos = 0;
            pBuffer[pos++] = Str2byte(cd);
            memmove(cd, cd + 8, strlen(cd) - 7);
        }
    }

    if (strlen(cd) > 0) {
        int pos = nTotalBits / 8;
        pBuffer[pos] = Str2byte(cd);
    }

    fclose(pFile);
    pFile = NULL;
    return OK;
}

int WriteFile(const char* pFilename, const HEAD& sHead, const char* pBuffer, int nBufSize) {
    char hufFilename[512] = { 0 };
    strcpy(hufFilename, pFilename);
    strcat(hufFilename, ".huf");

    FILE* pFile = fopen(hufFilename, "wb");
    if (!pFile) {
        cerr << "压缩文件创建失败！" << endl;
        return ERROR;
    }

    fwrite(&sHead, sizeof(HEAD), 1, pFile);
    fwrite(pBuffer, sizeof(char), nBufSize, pFile);

    fclose(pFile);
    pFile = NULL;
    cout << "生成压缩文件：" << hufFilename << endl;
    return OK;
}

int Compress(const char* pFilename) {
    HEAD sHead;
    if (InitHead(pFilename, sHead) == ERROR) {
        return ERROR;
    }

    HuffmanTree HT = NULL;
    CreateHuffmanTree(HT, sHead.weight, SIZE);

    HuffmanCode HC = NULL;
    CreateHuffmanCode(HT, HC, SIZE);

    char* pBuffer = NULL;
    int nBufSize = 0;
    if (Encode(pFilename, HC, pBuffer, nBufSize) == ERROR) {
        DestroyHuffman(HT, HC, SIZE);
        return ERROR;
    }

    if (WriteFile(pFilename, sHead, pBuffer, nBufSize) == ERROR) {
        DestroyHuffman(HT, HC, SIZE);
        delete[] pBuffer;
        return ERROR;
    }

    int nSrcSize = sHead.length;
    int nDesSize = sizeof(HEAD) + nBufSize;
    double dRate = (double)nDesSize / nSrcSize * 100;
    cout << "原文件大小：" << nSrcSize << " 字节" << endl;
    cout << "压缩文件大小：" << nDesSize << " 字节" << endl;
    cout << "压缩比率：" << dRate << "%" << endl;

    DestroyHuffman(HT, HC, SIZE);
    delete[] pBuffer;
    return OK;
}
