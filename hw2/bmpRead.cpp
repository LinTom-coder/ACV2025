#include "Header.h"


BmpInfo bmpRead(char* filename) {
    BmpInfo imgInfo;
    FILE* fp = nullptr;

    errno_t err = fopen_s(&fp, filename, "rb");
    if (err != 0 || fp == nullptr) {
        cout << "Loading file failed: " << filename << endl;
        exit(1);
    }

    // 讀 BMP 標頭
    if (fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp) != 1 ||
        fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp) != 1) {
        cout << "Failed reading BMP headers\n";
        fclose(fp);
        exit(1);
    }

    if (imgInfo.bi.biBitCount != 24) {
        cout << "Not a usable format (only 24-bit supported)!" << endl;
        fclose(fp);
        exit(1);
    }

    // 移到影像資料偏移
    fseek(fp, imgInfo.bf.bfOffBits, SEEK_SET);

    int width = imgInfo.bi.biWidth;
    int height = imgInfo.bi.biHeight;
    if (height == 0 || width <= 0) {
        cout << "Invalid image dimensions\n";
        fclose(fp);
        exit(1);
    }
    int absHeight = (height > 0) ? height : -height;

    int row_padded = (width * 3 + 3) & (~3); // 4 byte 對齊
    // 讀入含 padding 的完整資料
    size_t readSize = static_cast<size_t>(row_padded) * static_cast<size_t>(absHeight);
    unsigned char* buffer = new unsigned char[readSize];

    size_t actuallyRead = fread(buffer, 1, readSize, fp);
    fclose(fp);
    if (actuallyRead != readSize) {
        // 有些 BMP header 的 biSizeImage 可能為 0，或檔案短於預期
        // 仍可接受，但應檢查
        if (actuallyRead < static_cast<size_t>(width) * static_cast<size_t>(absHeight) * 3) {
            cout << "Unexpected BMP data size\n";
            delete[] buffer;
            exit(1);
        }
    }

    // 若沒有 padding 則直接使用，否則移除 padding
    if (row_padded == width * 3) {
        imgInfo.imgDataDynamic = buffer;
        return imgInfo;
    }
    else {
        size_t outSize = static_cast<size_t>(width) * static_cast<size_t>(absHeight) * 3;
        unsigned char* temp = new unsigned char[outSize];
        unsigned char* dst = temp;

        for (int y = 0; y < absHeight; y++) {
            unsigned char* src = buffer + static_cast<size_t>(y) * row_padded;
            // 複製該列有效像素資料
            for (int x = 0; x < width * 3; x++) {
                *dst++ = *src++;
            }
        }

        delete[] buffer;
        imgInfo.imgDataDynamic = temp;
        return imgInfo;
    }
}