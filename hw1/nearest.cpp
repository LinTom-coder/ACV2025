//#include "Header.h"
//#include <iostream>
//using namespace std;
//
//BmpInfo nearest(BmpInfo imgInfoN) {
//    int width = imgInfoN.bi.biWidth;
//    int height = imgInfoN.bi.biHeight;
//    int newWidth = width * 2;
//    int newHeight = height * 2;
//
//    // 建立新的動態陣列
//    unsigned char* newImgData = new unsigned char[newWidth * newHeight * 3];
//
//    for (int y = 0; y < newHeight; y++) {
//        for (int x = 0; x < newWidth; x++) {
//            // 對應原圖座標
//            int srcX = x / 2;
//            int srcY = y / 2;
//
//            int srcIdx = (srcY * width + srcX) * 3;
//            int dstIdx = (y * newWidth + x) * 3;
//
//            newImgData[dstIdx] = imgInfoN.imgDataDynamic[srcIdx];       // B
//            newImgData[dstIdx + 1] = imgInfoN.imgDataDynamic[srcIdx + 1]; // G
//            newImgData[dstIdx + 2] = imgInfoN.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//
//    // 釋放原始影像記憶體
//    delete[] imgInfoN.imgDataDynamic;
//
//    // 更新影像資訊
//    imgInfoN.imgDataDynamic = newImgData;
//    imgInfoN.bi.biWidth = newWidth;
//    imgInfoN.bi.biHeight = newHeight;
//
//    return imgInfoN;
//}