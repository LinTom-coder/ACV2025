#include "Header.h"

BmpInfo binary_opening(const BmpInfo& img, int kernalsz) {
	BmpInfo eroded = binary_erosion(img, kernalsz);
	BmpInfo opened = binary_dilation(eroded, kernalsz);
	return opened;
}

BmpInfo binary_closing(const BmpInfo& img, int kernalsz) {
    BmpInfo dilated = binary_dilation(img, kernalsz);
    BmpInfo closed = binary_erosion(dilated, kernalsz);
	return closed;
}

BmpInfo binary_dilation(const BmpInfo& img, int kernalsz) {
    int width = img.bi.biWidth;
    int height = img.bi.biHeight;
    int channels = 3; // B, G, R
    int size = width * height * channels;
    unsigned char* dilatedData = new unsigned char[size];

    int pad = kernalsz / 2;
    BmpInfo paddedimg = padding(img, pad);
    int padded_width = width + 2 * pad;

    for (int x = 0; x < height; ++x) {
        for (int y = 0; y < width; ++y) {
            bool hasWhite = false;

            for (int i = 0; i < kernalsz; ++i) {
                for (int j = 0; j < kernalsz; ++j) {
                    int padded_index = ((x + i) * padded_width + (y + j)) * channels;

                    // 三通道都檢查，只要有任一通道是255就視為白
                    for (int c = 0; c < channels; ++c) {
                        if (paddedimg.imgDataDynamic[padded_index + c] == 255) {
                            hasWhite = true;
                            break;
                        }
                    }
                    if (hasWhite) break;
                }
                if (hasWhite) break;
            }

            unsigned char val = hasWhite ? 255 : 0;

            // 三通道都設相同值（保持binary一致）
            for (int c = 0; c < channels; ++c) {
                dilatedData[(x * width + y) * channels + c] = val;
            }
        }
    }

    BmpInfo result = img;
    result.imgDataDynamic = dilatedData;
    return result;
}

BmpInfo binary_erosion(const BmpInfo& img, int kernalsz) {
    int width = img.bi.biWidth;
    int height = img.bi.biHeight;
    int channels = 3; // B, G, R
    int size = width * height * channels;
    unsigned char* erodedData = new unsigned char[size];

    int pad = kernalsz / 2;
    BmpInfo paddedimg = padding(img, pad);
    int padded_width = width + 2 * pad;

    for (int x = 0; x < height; ++x) {
        for (int y = 0; y < width; ++y) {
            bool hasZero = false;

            // 對kernel區域進行檢查
            for (int i = 0; i < kernalsz; ++i) {
                for (int j = 0; j < kernalsz; ++j) {
                    int padded_index = ((x + i) * padded_width + (y + j)) * channels;

                    // 三個通道都要一起檢查
                    for (int c = 0; c < channels; ++c) {
                        if (paddedimg.imgDataDynamic[padded_index + c] == 0) {
                            hasZero = true;
                            break;
                        }
                    }
                    if (hasZero) break;
                }
                if (hasZero) break;
            }

            unsigned char val = hasZero ? 0 : 255;

            // 對輸出影像的三個通道都設定同樣的結果
            for (int c = 0; c < channels; ++c) {
                erodedData[(x * width + y) * channels + c] = val;
            }
        }
    }

    BmpInfo result = img;
    result.imgDataDynamic = erodedData;
    return result;
}
