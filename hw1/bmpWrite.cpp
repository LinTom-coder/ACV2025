//#include "Header.h"
//#include <iostream>
//using namespace std;
//
//void bmpWrite(BmpInfo imgInfo, char* filename) {
//    FILE* fpw = nullptr;
//    errno_t err = fopen_s(&fpw, filename, "wb");
//    if (err != 0 || fpw == nullptr) {
//        cout << "Cannot open file for writing: " << filename << endl;
//        return;
//    }
//
//    // 寫入 BMP 標頭
//    fwrite(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fpw);
//    fwrite(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fpw);
//
//    int width = imgInfo.bi.biWidth;
//    int height = imgInfo.bi.biHeight;
//    int row_padded = (width * 3 + 3) & (~3); // 每行對 4 bytes
//    unsigned char padding[3] = { 0, 0, 0 };     // 最多 3 bytes padding
//
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            int idx = (y * width + x) * 3;
//            fwrite(&imgInfo.imgDataDynamic[idx], 1, 1, fpw);       // B
//            fwrite(&imgInfo.imgDataDynamic[idx + 1], 1, 1, fpw);   // G
//            fwrite(&imgInfo.imgDataDynamic[idx + 2], 1, 1, fpw);   // R
//        }
//        // 寫 padding
//        fwrite(padding, 1, row_padded - width * 3, fpw);
//    }
//
//    fclose(fpw);
//}