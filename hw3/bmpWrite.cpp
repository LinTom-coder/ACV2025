#include "Header.h"
#include <iostream>
using namespace std;

bool bmpWrite(BmpInfo& img, char* filename)
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, "wb") != 0 || !fp) {
        printf("Cannot write BMP.\n");
        return false;
    }

    int rowPadded = ((img.width * 3 + 3) / 4) * 4;
    unsigned int dataSize = rowPadded * img.height;

    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER) + dataSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER);

    infoHeader.biSize = sizeof(BMPINFOHEADER);
    infoHeader.biWidth = img.width;
    infoHeader.biHeight = img.height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = dataSize;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);

    std::vector<unsigned char> rowBuf(rowPadded);

    for (int i = 0; i < img.height; i++) {
        int srcRow = img.height - 1 - i;

        for (int j = 0; j < img.width; j++) {
            int idx = (srcRow * img.width + j) * 3;
            unsigned char R = img.data[idx + 0];
            unsigned char G = img.data[idx + 1];
            unsigned char B = img.data[idx + 2];

            rowBuf[j * 3 + 0] = B;
            rowBuf[j * 3 + 1] = G;
            rowBuf[j * 3 + 2] = R;
        }
        fwrite(rowBuf.data(), 1, rowPadded, fp);
    }

    fclose(fp);
    return true;
}

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