//#include "Header.h"
//#include <iostream>
//using namespace std;
//
//void c() {
//    char  input_img[] = "InputImage1.bmp";
//    char output_img[] = "grb.bmp";
//
//    BmpInfo imgInfo = bmpRead(input_img);
//
//    int width = imgInfo.bi.biWidth;
//    int height = imgInfo.bi.biHeight;
//    int size = width * height;
//
//    // BGR ¡÷ GRB
//    for (int i = 0; i < size; i++) {
//        int idx = i * 3; // ¨C­Ó¹³¯À 3 bytes
//        unsigned char temp = imgInfo.imgDataDynamic[idx];       // B
//        imgInfo.imgDataDynamic[idx] = imgInfo.imgDataDynamic[idx + 1];   // G ¡÷ B
//        imgInfo.imgDataDynamic[idx + 1] = imgInfo.imgDataDynamic[idx + 2]; // R ¡÷ G
//        imgInfo.imgDataDynamic[idx + 2] = temp;                // B ¡÷ R
//    }
//
//    bmpWrite(imgInfo, output_img);
//}