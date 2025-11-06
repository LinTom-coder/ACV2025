#include "Header.h"

BmpInfo rgb2binary(const BmpInfo& src, int thr, double r, double g, double b) {
    BmpInfo out;
    out.bf = src.bf;
    out.bi = src.bi;

    int width = src.bi.biWidth;
    int height = src.bi.biHeight;
    int absHeight = (height > 0) ? height : -height;
    int size = width * absHeight;
    out.imgDataDynamic = new unsigned char[size * 3];

    for (int i = 0; i < size; i++) {
        int idx = i * 3;
        unsigned char gray = static_cast<unsigned char>(
            r * src.imgDataDynamic[idx + 2] +
            g * src.imgDataDynamic[idx + 1] +
            b * src.imgDataDynamic[idx]
            );
        unsigned char val = (gray >= thr) ? 255 : 0;
        out.imgDataDynamic[idx] = val; // B
        out.imgDataDynamic[idx + 1] = val; // G
        out.imgDataDynamic[idx + 2] = val; // R
    }
    return out;
}

//#include "Header.h"
//
//BmpInfo rgb2binary(BmpInfo imgInfoN, int thr, double r, double g, double b) {
//	//rgb to binary
//	int width = imgInfoN.bi.biWidth;
//	int height = imgInfoN.bi.biHeight;
//	int size = width * height;
//	for (int i = 0; i < size; i++) {
//		int idx = i * 3; // 每個像素 3 bytes
//		//計算灰階值
//		unsigned char gray = static_cast<unsigned char>(r * imgInfoN.imgDataDynamic[idx + 2] + g * imgInfoN.imgDataDynamic[idx + 1] + b * imgInfoN.imgDataDynamic[idx]);
//		//二值化
//		if (gray >= thr) {
//			imgInfoN.imgDataDynamic[idx] = 255;       // B
//			imgInfoN.imgDataDynamic[idx + 1] = 255;   // G
//			imgInfoN.imgDataDynamic[idx + 2] = 255;   // R
//		}
//		else {
//			imgInfoN.imgDataDynamic[idx] = 0;       // B
//			imgInfoN.imgDataDynamic[idx + 1] = 0;   // G
//			imgInfoN.imgDataDynamic[idx + 2] = 0;   // R
//		}
//	}
//	return imgInfoN;
//}