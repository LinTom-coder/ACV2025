//#include "Header.h"
//#include <iostream>
//using namespace std;
//
//void b() {
//    char  input_img[] = "InputImage1.bmp";
//    char output_img[] = "rotation270.bmp";
//    BmpInfo imgInfo = bmpRead(input_img);
//
//    int width = imgInfo.bi.biWidth;
//    int height = imgInfo.bi.biHeight;
//    int size = width * height * 3;
//
//    // rotate270
//    unsigned char* rotated = new unsigned char[size];
//
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            int srcIdx = (y * width + x) * 3;
//            int newX = height - 1 - y;
//            int newY = x;
//            int dstIdx = (newY * width + newX) * 3;
//
//            rotated[dstIdx] = imgInfo.imgDataDynamic[srcIdx];       // B
//            rotated[dstIdx + 1] = imgInfo.imgDataDynamic[srcIdx + 1]; // G
//            rotated[dstIdx + 2] = imgInfo.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//
//    // 更新影像資訊
//    delete[] imgInfo.imgDataDynamic;
//    imgInfo.imgDataDynamic = rotated;
//    imgInfo.bi.biWidth = height;
//    imgInfo.bi.biHeight = width;
//
//    bmpWrite(imgInfo, output_img);
//}