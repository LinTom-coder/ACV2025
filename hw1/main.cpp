//#include "Header.h"
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;


struct BmpInfo {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    unsigned char* imgDataDynamic;
};

BmpInfo bmpRead(char* filename);
void bmpWrite(BmpInfo, char*);
BmpInfo nearest(BmpInfo imgInfoN);
BmpInfo shrink(BmpInfo imgInfoN);
void a();
void b();
void c();
void d();
void e();


int main()
{
	printf("1: Image Read/Write\n");
	printf("2: Image Rotation\n");
	printf("3: Image Channel Interchange\n");
	printf("4: bonus\n");
	printf("5: opencv\n");
	char num;
	printf("Enter the question number to select output result:");
	scanf("%c", &num);
	switch (num)
	{
	case '1': a();
		break;
	case '2': b();
		break;
	case '3': c();
		break;
	case '4': d();
		break;
	case '5': e();
		break;
	//case '6': hw2_3_2();
	//	break;
	//case '7': hw2_3_2_test();
	//	break;
	default: printf("wrong number");
	}
}

void a() {
	char  input_img[] = "InputImage1.bmp";
	char output_img[] = "InputImage_out.bmp";

	BmpInfo img = bmpRead(input_img);
	bmpWrite(img, output_img);
}

void b() {
	char  input_img[] = "InputImage1.bmp";
	char output_img[] = "rotation270.bmp";
	BmpInfo imgInfo = bmpRead(input_img);

	int width = imgInfo.bi.biWidth;
	int height = imgInfo.bi.biHeight;
	int size = width * height * 3;

	// rotate270
	unsigned char* rotated = new unsigned char[size];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int srcIdx = (y * width + x) * 3;
			int newX = height - 1 - y;
			int newY = x;
			int dstIdx = (newY * width + newX) * 3;

			rotated[dstIdx] = imgInfo.imgDataDynamic[srcIdx];       // B
			rotated[dstIdx + 1] = imgInfo.imgDataDynamic[srcIdx + 1]; // G
			rotated[dstIdx + 2] = imgInfo.imgDataDynamic[srcIdx + 2]; // R
		}
	}

	// 更新影像資訊
	delete[] imgInfo.imgDataDynamic;
	imgInfo.imgDataDynamic = rotated;
	imgInfo.bi.biWidth = height;
	imgInfo.bi.biHeight = width;

	bmpWrite(imgInfo, output_img);
}

void c() {
	char  input_img[] = "InputImage1.bmp";
	char output_img[] = "grb.bmp";

	BmpInfo imgInfo = bmpRead(input_img);

	int width = imgInfo.bi.biWidth;
	int height = imgInfo.bi.biHeight;
	int size = width * height;

	// BGR → GRB
	for (int i = 0; i < size; i++) {
		int idx = i * 3; // 每個像素 3 bytes
		unsigned char temp = imgInfo.imgDataDynamic[idx];       // B
		imgInfo.imgDataDynamic[idx] = imgInfo.imgDataDynamic[idx + 1];   // G → B
		imgInfo.imgDataDynamic[idx + 1] = imgInfo.imgDataDynamic[idx + 2]; // R → G
		imgInfo.imgDataDynamic[idx + 2] = temp;                // B → R
	}

	bmpWrite(imgInfo, output_img);
}

void d() {
    char  input_img[] = "InputImage1.bmp";
    char output_img_n[] = "bonus1_double.bmp";
    char output_img_r[] = "bonus1_rotation.bmp";
    char output_img_grb[] = "bonus1_grb.bmp";

    BmpInfo img_b1 = bmpRead(input_img);
    BmpInfo img_b1_r = bmpRead(input_img);
    BmpInfo img_b1_grb = bmpRead(input_img);

    //nearest
    BmpInfo img_b1_nearest = nearest(img_b1);
    bmpWrite(img_b1_nearest, output_img_n);

    //rotation
    int width = img_b1_nearest.bi.biWidth;
    int height = img_b1_nearest.bi.biHeight;
    int size = width * height * 3;

    unsigned char* rotated = new unsigned char[size];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIdx = (y * width + x) * 3;
            int newX = height - 1 - y;
            int newY = x;
            int dstIdx = (newY * width + newX) * 3;

            rotated[dstIdx] = img_b1_nearest.imgDataDynamic[srcIdx];       // B
            rotated[dstIdx + 1] = img_b1_nearest.imgDataDynamic[srcIdx + 1]; // G
            rotated[dstIdx + 2] = img_b1_nearest.imgDataDynamic[srcIdx + 2]; // R
        }
    }
    img_b1_r.imgDataDynamic = rotated;
    img_b1_r.bi.biWidth = height;
    img_b1_r.bi.biHeight = width;

    bmpWrite(img_b1_r, output_img_r);
    delete[] img_b1_r.imgDataDynamic;

    //rgb
    int size_grb = width * height;

    // BGR → GRB
    for (int i = 0; i < size_grb; i++) {
        int idx = i * 3; // 每個像素 3 bytes
        unsigned char temp = img_b1_nearest.imgDataDynamic[idx];       // B
        img_b1_nearest.imgDataDynamic[idx] = img_b1_nearest.imgDataDynamic[idx + 1];   // G → B
        img_b1_nearest.imgDataDynamic[idx + 1] = img_b1_nearest.imgDataDynamic[idx + 2]; // R → G
        img_b1_nearest.imgDataDynamic[idx + 2] = temp;                // B → R
    }

    bmpWrite(img_b1_nearest, output_img_grb);
    delete[] img_b1_nearest.imgDataDynamic;
    delete[] img_b1_grb.imgDataDynamic;

    //shrink
    char output_img_s[] = "bonus1_shrink.bmp";
    char output_imgs_r[] = "bonus1s_rotation.bmp";
    char output_imgs_grb[] = "bonus1s_grb.bmp";

    BmpInfo img_b1s = bmpRead(input_img);
    BmpInfo img_b1s_r = bmpRead(input_img);
    BmpInfo img_b1s_grb = bmpRead(input_img);

    BmpInfo img_b1s_shrink = shrink(img_b1s);
    bmpWrite(img_b1s_shrink, output_img_s);

    //rotation
    int width_s = img_b1s_shrink.bi.biWidth;
    int height_s = img_b1s_shrink.bi.biHeight;
    int size_s = width_s * height_s * 3;
    unsigned char* rotated_s = new unsigned char[size_s];
    for (int y = 0; y < height_s; y++) {
        for (int x = 0; x < width_s; x++) {
            int srcIdx = (y * width_s + x) * 3;
            int newX = height_s - 1 - y;
            int newY = x;
            int dstIdx = (newY * width_s + newX) * 3;
            rotated_s[dstIdx] = img_b1s_shrink.imgDataDynamic[srcIdx];       // B
            rotated_s[dstIdx + 1] = img_b1s_shrink.imgDataDynamic[srcIdx + 1]; // G
            rotated_s[dstIdx + 2] = img_b1s_shrink.imgDataDynamic[srcIdx + 2]; // R
        }
    }
    img_b1s_r.imgDataDynamic = rotated_s;
    img_b1s_r.bi.biWidth = height_s;
    img_b1s_r.bi.biHeight = width_s;
    bmpWrite(img_b1s_r, output_imgs_r);
    delete[] img_b1s_r.imgDataDynamic;

    //rgb
    int size_grb_s = width_s * height_s;
    for (int i = 0; i < size_grb_s; i++) {
        int idx = i * 3; // 每個像素 3 bytes
        unsigned char temp = img_b1s_shrink.imgDataDynamic[idx];       // B
        img_b1s_shrink.imgDataDynamic[idx] = img_b1s_shrink.imgDataDynamic[idx + 1];   // G → B
        img_b1s_shrink.imgDataDynamic[idx + 1] = img_b1s_shrink.imgDataDynamic[idx + 2]; // R → G
        img_b1s_shrink.imgDataDynamic[idx + 2] = temp;                // B → R
    }
    bmpWrite(img_b1s_shrink, output_imgs_grb);
    delete[] img_b1s_shrink.imgDataDynamic;

    //bonus2
    char output_img_b2[] = "bonus2_4096.bmp";
    char output_img_b2_r[] = "bonus2_rotation.bmp";
    char output_img_b2_grb[] = "bonus2_grb.bmp";

    BmpInfo img_b2 = bmpRead(input_img);
    BmpInfo img_b2_r = bmpRead(input_img);
    BmpInfo img_b2_grb = bmpRead(input_img);

    //nearest
    BmpInfo img_b2_nearest = nearest(img_b2);
    img_b2_nearest = nearest(img_b2_nearest);
    img_b2_nearest = nearest(img_b2_nearest);
    bmpWrite(img_b2_nearest, output_img_b2);

    //rotation
    int width2 = img_b2_nearest.bi.biWidth;
    int height2 = img_b2_nearest.bi.biHeight;
    int size2 = width2 * height2 * 3;

    unsigned char* rotated2 = new unsigned char[size2];
    for (int y = 0; y < height2; y++) {
        for (int x = 0; x < width2; x++) {
            int srcIdx = (y * width2 + x) * 3;
            int newX = height2 - 1 - y;
            int newY = x;
            int dstIdx = (newY * width2 + newX) * 3;

            rotated2[dstIdx] = img_b2_nearest.imgDataDynamic[srcIdx];       // B
            rotated2[dstIdx + 1] = img_b2_nearest.imgDataDynamic[srcIdx + 1]; // G
            rotated2[dstIdx + 2] = img_b2_nearest.imgDataDynamic[srcIdx + 2]; // R
        }
    }
    img_b2_r.imgDataDynamic = rotated2;
    img_b2_r.bi.biWidth = height2;
    img_b2_r.bi.biHeight = width2;

    bmpWrite(img_b2_r, output_img_b2_r);
    delete[] img_b2_r.imgDataDynamic;

    //rgb
    int size2_grb = width2 * height2;

    // BGR → GRB
    for (int i = 0; i < size2_grb; i++) {
        int idx = i * 3; // 每個像素 3 bytes
        unsigned char temp = img_b2_nearest.imgDataDynamic[idx];       // B
        img_b2_nearest.imgDataDynamic[idx] = img_b2_nearest.imgDataDynamic[idx + 1];   // G → B
        img_b2_nearest.imgDataDynamic[idx + 1] = img_b2_nearest.imgDataDynamic[idx + 2]; // R → G
        img_b2_nearest.imgDataDynamic[idx + 2] = temp;                // B → R
    }

    bmpWrite(img_b2_nearest, output_img_b2_grb);
    delete[] img_b2_nearest.imgDataDynamic;
    delete[] img_b2_grb.imgDataDynamic;

}

BmpInfo shrink(BmpInfo imgInfoN) {
    int width = imgInfoN.bi.biWidth;
    int height = imgInfoN.bi.biHeight;
    int newWidth = width / 2;
    int newHeight = height / 2;

    // 建立新的動態陣列
    unsigned char* newImgData = new unsigned char[newWidth * newHeight * 3];

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // 對應原圖座標
            int srcX = x * 2;
            int srcY = y * 2;

            int srcIdx = (srcY * width + srcX) * 3;
            int dstIdx = (y * newWidth + x) * 3;

            newImgData[dstIdx] = imgInfoN.imgDataDynamic[srcIdx];       // B
            newImgData[dstIdx + 1] = imgInfoN.imgDataDynamic[srcIdx + 1]; // G
            newImgData[dstIdx + 2] = imgInfoN.imgDataDynamic[srcIdx + 2]; // R
        }
    }

    // 釋放原始影像記憶體
    delete[] imgInfoN.imgDataDynamic;

    // 更新影像資訊
    imgInfoN.imgDataDynamic = newImgData;
    imgInfoN.bi.biWidth = newWidth;
    imgInfoN.bi.biHeight = newHeight;

    return imgInfoN;
}

void e() {
    // 1. 讀取 BMP 檔
    Mat img = imread("InputImage1.bmp", IMREAD_COLOR);
    if (img.empty()) {
        printf("無法讀取圖片\n");
        return;
    }
	imwrite("opencv_out.bmp", img);

	//rotate90
	Mat img_rotated;
	cv::rotate(img, img_rotated, ROTATE_90_COUNTERCLOCKWISE);
	imwrite("opencv_out_rotated.bmp", img_rotated);

	//channel swap
	Mat img_swapped;
    Mat channels[3];
    split(img, channels);               // channels[0] = B, [1] = G, [2] = R
    Mat chs[] = { channels[1], channels[2], channels[0] };  // G, R, B
    merge(chs, 3, img_swapped);
	imwrite("opencv_out_swapped.bmp", img_swapped);

	// bonus1. 放大（scale 2倍）
    Mat img_double;
    cv::resize(img, img_double, Size(img.cols * 2, img.rows * 2), 0, 0, INTER_NEAREST);
    imwrite("opencv_out_double.bmp", img_double);

    Mat imgd_rotated;
    cv::rotate(img_double, imgd_rotated, ROTATE_90_COUNTERCLOCKWISE);
    imwrite("opencvd_out_r.bmp", imgd_rotated);

    Mat imgd_swapped;
    Mat channelsd[3];
    split(img_double, channelsd);               // channels[0] = B, [1] = G, [2] = R
    Mat chsd[] = { channelsd[1], channelsd[2], channelsd[0] };  // G, R, B
    merge(chsd, 3, imgd_swapped);
    imwrite("opencvd_out_c.bmp", imgd_swapped);

	//bonus1 縮小（scale 0.5倍）
	Mat img_shrink;
	cv::resize(img, img_shrink, Size(img.cols / 2, img.rows / 2), 0, 0, INTER_NEAREST);
	imwrite("opencv_out_shrink.bmp", img_shrink);
	Mat imgs_rotated;
	cv::rotate(img_shrink, imgs_rotated, ROTATE_90_COUNTERCLOCKWISE);
	imwrite("opencvs_out_r.bmp", imgs_rotated);
	Mat imgs_swapped;
    Mat channelss[3];
    split(img_shrink, channelss);               // channels[0] = B, [1] = G, [2] = R
    Mat chss[] = { channelss[1], channelss[2], channelss[0] };  // G, R, B
    merge(chss, 3, imgs_swapped);
	imwrite("opencvs_out_c.bmp", imgs_swapped);

	//bonus2 放大4096
	Mat img_4096;
	cv::resize(img, img_4096, Size(4096, 4096), 0, 0, INTER_NEAREST);
	imwrite("opencv_out_4096.bmp", img_4096);
	Mat img4096_rotated;
	cv::rotate(img_4096, img4096_rotated, ROTATE_90_COUNTERCLOCKWISE);
	imwrite("opencv4096_out_r.bmp", img4096_rotated);
	Mat img4096_swapped;
    Mat channels4096[3];
    split(img_4096, channels4096);               // channels[0] = B, [1] = G, [2] = R
    Mat chs4096[] = { channels4096[1], channels4096[2], channels4096[0] };  // G, R, B
    merge(chs4096, 3, img4096_swapped);
	imwrite("opencv4096_out_c.bmp", img4096_swapped);
}

BmpInfo nearest(BmpInfo imgInfoN) {
    int width = imgInfoN.bi.biWidth;
    int height = imgInfoN.bi.biHeight;
    int newWidth = width * 2;
    int newHeight = height * 2;

    // 建立新的動態陣列
    unsigned char* newImgData = new unsigned char[newWidth * newHeight * 3];

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // 對應原圖座標
            int srcX = x / 2;
            int srcY = y / 2;

            int srcIdx = (srcY * width + srcX) * 3;
            int dstIdx = (y * newWidth + x) * 3;

            newImgData[dstIdx] = imgInfoN.imgDataDynamic[srcIdx];       // B
            newImgData[dstIdx + 1] = imgInfoN.imgDataDynamic[srcIdx + 1]; // G
            newImgData[dstIdx + 2] = imgInfoN.imgDataDynamic[srcIdx + 2]; // R
        }
    }

    // 釋放原始影像記憶體
    delete[] imgInfoN.imgDataDynamic;

    // 更新影像資訊
    imgInfoN.imgDataDynamic = newImgData;
    imgInfoN.bi.biWidth = newWidth;
    imgInfoN.bi.biHeight = newHeight;

    return imgInfoN;
}

BmpInfo bmpRead(char* filename) {
    BmpInfo imgInfo;
    FILE* fp = nullptr;

    errno_t err = fopen_s(&fp, filename, "rb");
    if (err != 0 || fp == nullptr) {
        cout << "Loading file failed: " << filename << endl;
        exit(1);
    }

    // 讀 BMP 標頭
    fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp);

    if (imgInfo.bi.biBitCount != 24) {
        cout << "Not a usable format (only 24-bit supported)!" << endl;
        fclose(fp);
        exit(1);
    }

    // 移到影像資料偏移
    fseek(fp, imgInfo.bf.bfOffBits, SEEK_SET);

    // 分配連續記憶體存整張圖 (每個像素 3 bytes)
    int width = imgInfo.bi.biWidth;
    int height = imgInfo.bi.biHeight;
    int size = width * height * 3;
    unsigned char* buffer = new unsigned char[size];

    // 讀入 BMP 資料
    fread(buffer, 1, size, fp);
    fclose(fp);

    // BMP 的每行可能有 padding (每行要 4 byte 對齊)
    int row_padded = (width * 3 + 3) & (~3); // 4 byte 對齊
    if (row_padded != width * 3) {
        unsigned char* temp = new unsigned char[size];
        unsigned char* src = buffer;
        unsigned char* dst = temp;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width * 3; x++) {
                *dst++ = *src++;
            }
            src += row_padded - width * 3; // 跳過 padding
        }

        delete[] buffer;
        buffer = temp;
    }

    // 將 buffer 存入 imgInfo
    imgInfo.imgDataDynamic = buffer; // 假設 BmpInfo 改成有這個指標
    return imgInfo;
}

void bmpWrite(BmpInfo imgInfo, char* filename) {
    FILE* fpw = nullptr;
    errno_t err = fopen_s(&fpw, filename, "wb");
    if (err != 0 || fpw == nullptr) {
        cout << "Cannot open file for writing: " << filename << endl;
        return;
    }

    // 寫入 BMP 標頭
    fwrite(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fpw);
    fwrite(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fpw);

    int width = imgInfo.bi.biWidth;
    int height = imgInfo.bi.biHeight;
    int row_padded = (width * 3 + 3) & (~3); // 每行對 4 bytes
    unsigned char padding[3] = { 0, 0, 0 };     // 最多 3 bytes padding

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            fwrite(&imgInfo.imgDataDynamic[idx], 1, 1, fpw);       // B
            fwrite(&imgInfo.imgDataDynamic[idx + 1], 1, 1, fpw);   // G
            fwrite(&imgInfo.imgDataDynamic[idx + 2], 1, 1, fpw);   // R
        }
        // 寫 padding
        fwrite(padding, 1, row_padded - width * 3, fpw);
    }

    fclose(fpw);
}