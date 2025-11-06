#include "Header.h"

BmpInfo padding(BmpInfo img, int pdsz) {
    int width = img.bi.biWidth;
    int height = img.bi.biHeight;
    int channels = 3;

    int new_width = width + 2 * pdsz;
    int new_height = height + 2 * pdsz;
    int new_size = new_width * new_height;

    unsigned char* paddedData = new unsigned char[new_size * channels];

    // 初始化為黑色 (0,0,0)
    for (int i = 0; i < new_size * channels; ++i)
        paddedData[i] = 255;

    // 將原始影像拷貝到中央
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int src_idx = (y * width + x) * channels;
            int dst_idx = ((y + pdsz) * new_width + (x + pdsz)) * channels;

            paddedData[dst_idx] = img.imgDataDynamic[src_idx];     // B
            paddedData[dst_idx + 1] = img.imgDataDynamic[src_idx + 1]; // G
            paddedData[dst_idx + 2] = img.imgDataDynamic[src_idx + 2]; // R
        }
    }

    // 更新影像資訊
    img.bi.biWidth = new_width;
    img.bi.biHeight = new_height;
    img.bi.biSizeImage = new_size * channels;

    // 不要刪除舊的 data，讓呼叫端決定是否釋放
    img.imgDataDynamic = paddedData;
    return img;
}
