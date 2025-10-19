//#include "Header.h"
//#include <iostream>
//#include <cstdlib>
//using namespace std;
//
//BmpInfo bmpRead(char* filename) {
//    BmpInfo imgInfo;
//    FILE* fp = nullptr;
//
//    errno_t err = fopen_s(&fp, filename, "rb");
//    if (err != 0 || fp == nullptr) {
//        cout << "Loading file failed: " << filename << endl;
//        exit(1);
//    }
//
//    // 讀 BMP 標頭
//    fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp);
//    fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp);
//
//    if (imgInfo.bi.biBitCount != 24) {
//        cout << "Not a usable format (only 24-bit supported)!" << endl;
//        fclose(fp);
//        exit(1);
//    }
//
//    // 移到影像資料偏移
//    fseek(fp, imgInfo.bf.bfOffBits, SEEK_SET);
//
//    // 分配連續記憶體存整張圖 (每個像素 3 bytes)
//    int width = imgInfo.bi.biWidth;
//    int height = imgInfo.bi.biHeight;
//    int size = width * height * 3;
//    unsigned char* buffer = new unsigned char[size];
//
//    // 讀入 BMP 資料
//    fread(buffer, 1, size, fp);
//    fclose(fp);
//
//    // BMP 的每行可能有 padding (每行要 4 byte 對齊)
//    int row_padded = (width * 3 + 3) & (~3); // 4 byte 對齊
//    if (row_padded != width * 3) {
//        unsigned char* temp = new unsigned char[size];
//        unsigned char* src = buffer;
//        unsigned char* dst = temp;
//
//        for (int y = 0; y < height; y++) {
//            for (int x = 0; x < width * 3; x++) {
//                *dst++ = *src++;
//            }
//            src += row_padded - width * 3; // 跳過 padding
//        }
//
//        delete[] buffer;
//        buffer = temp;
//    }
//
//    // 將 buffer 存入 imgInfo
//    imgInfo.imgDataDynamic = buffer; // 假設 BmpInfo 改成有這個指標
//    return imgInfo;
//}