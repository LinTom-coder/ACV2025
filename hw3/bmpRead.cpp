#include "Header.h"

bool bmpRead(char* filename, BmpInfo& img)
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, "rb") != 0 || !fp) {
        printf("Cannot open input BMP: %s\n", filename);
        return false;
    }

    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    if (fread(&fileHeader, sizeof(BMPFILEHEADER), 1, fp) != 1 ||
        fread(&infoHeader, sizeof(BMPINFOHEADER), 1, fp) != 1) {
        printf("Failed reading BMP headers.\n");
        fclose(fp);
        return false;
    }

    if (fileHeader.bfType != 0x4D42) {
        printf("Not BMP.\n");
        fclose(fp);
        return false;
    }
    if (infoHeader.biBitCount != 24) {
        printf("Only support 24-bit BMP.\n");
        fclose(fp);
        return false;
    }

    img.width = infoHeader.biWidth;
    img.height = abs(infoHeader.biHeight);
    img.channels = 3;

    // 檢查尺寸並避免 overflow
    uint64_t pixels = (uint64_t)img.width * (uint64_t)img.height;
    if (img.width <= 0 || img.height <= 0) {
        printf("Invalid image dimensions: %d x %d\n", img.width, img.height);
        fclose(fp);
        return false;
    }
    if (pixels > (uint64_t)(1ULL << 32)) { // 過大閾值 (調整為適合你需求)
        printf("Image too large to allocate: %llu pixels\n", (unsigned long long)pixels);
        fclose(fp);
        return false;
    }

    uint64_t totalBytes = pixels * img.channels;
    try {
        img.data.clear();
        img.data.reserve((size_t)totalBytes);
        img.data.resize((size_t)totalBytes);
    } catch (const bad_alloc&) {
        printf("Failed to allocate memory for image data (%llu bytes).\n", (unsigned long long)totalBytes);
        fclose(fp);
        return false;
    }

    int rowPadded = ((img.width * 3 + 3) / 4) * 4;
    std::vector<unsigned char> rowBuf(rowPadded);

    bool isBottomUp = infoHeader.biHeight > 0;

    fseek(fp, fileHeader.bfOffBits, SEEK_SET);

    for (int i = 0; i < img.height; i++) {
        size_t readCnt = fread(rowBuf.data(), 1, rowPadded, fp);
        if (readCnt != (size_t)rowPadded) {
            printf("Unexpected BMP row read size: %zu (expected %d)\n", readCnt, rowPadded);
            fclose(fp);
            return false;
        }

        int dstRow = isBottomUp ? (img.height - 1 - i) : i;

        for (int j = 0; j < img.width; j++) {
            unsigned char B = rowBuf[j * 3 + 0];
            unsigned char G = rowBuf[j * 3 + 1];
            unsigned char R = rowBuf[j * 3 + 2];

            size_t idx = ((size_t)dstRow * img.width + j) * 3;
            // bounds guard (防止意外越界)
            if (idx + 2 >= img.data.size()) {
                printf("Index out of range while reading BMP: idx=%zu, dataSize=%zu\n", idx, img.data.size());
                fclose(fp);
                return false;
            }
            img.data[idx + 0] = R;
            img.data[idx + 1] = G;
            img.data[idx + 2] = B;
        }
    }

    fclose(fp);
    return true;
}

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
//    if (fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp) != 1 ||
//        fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp) != 1) {
//        cout << "Failed reading BMP headers\n";
//        fclose(fp);
//        exit(1);
//    }
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
//    int width = imgInfo.bi.biWidth;
//    int height = imgInfo.bi.biHeight;
//    if (height == 0 || width <= 0) {
//        cout << "Invalid image dimensions\n";
//        fclose(fp);
//        exit(1);
//    }
//    int absHeight = (height > 0) ? height : -height;
//
//    int row_padded = (width * 3 + 3) & (~3); // 4 byte 對齊
//    // 讀入含 padding 的完整資料
//    size_t readSize = static_cast<size_t>(row_padded) * static_cast<size_t>(absHeight);
//    unsigned char* buffer = new unsigned char[readSize];
//
//    size_t actuallyRead = fread(buffer, 1, readSize, fp);
//    fclose(fp);
//    if (actuallyRead != readSize) {
//        // 有些 BMP header 的 biSizeImage 可能為 0，或檔案短於預期
//        // 仍可接受，但應檢查
//        if (actuallyRead < static_cast<size_t>(width) * static_cast<size_t>(absHeight) * 3) {
//            cout << "Unexpected BMP data size\n";
//            delete[] buffer;
//            exit(1);
//        }
//    }
//
//    // 若沒有 padding 則直接使用，否則移除 padding
//    if (row_padded == width * 3) {
//        imgInfo.imgDataDynamic = buffer;
//        return imgInfo;
//    }
//    else {
//        size_t outSize = static_cast<size_t>(width) * static_cast<size_t>(absHeight) * 3;
//        unsigned char* temp = new unsigned char[outSize];
//        unsigned char* dst = temp;
//
//        for (int y = 0; y < absHeight; y++) {
//            unsigned char* src = buffer + static_cast<size_t>(y) * row_padded;
//            // 複製該列有效像素資料
//            for (int x = 0; x < width * 3; x++) {
//                *dst++ = *src++;
//            }
//        }
//
//        delete[] buffer;
//        imgInfo.imgDataDynamic = temp;
//        return imgInfo;
//    }
//}