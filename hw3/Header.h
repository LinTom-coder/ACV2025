#pragma once
#ifndef MYHEADER_H
#define MYHEADER_H

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stack>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <chrono>
#include <cfloat> // ¥Î©ó DBL_MAX, DBL_MIN
#include <random>
#include <limits>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// BMP µ²ºc
#pragma pack(push, 1)
struct BMPFILEHEADER {
    unsigned short bfType;
    unsigned int   bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;
};

struct BMPINFOHEADER {
    unsigned int biSize;
    int          biWidth;
    int          biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
};
#pragma pack(pop)

struct BmpInfo {
    int width;
    int height;
    int channels;
    std::vector<unsigned char> data;
};

struct HoughLine {
    double rho;
    double theta;
    int votes;
};

//utility functions
bool bmpRead(char*, BmpInfo&);
bool bmpWrite(BmpInfo&, char*);
void imageToWorld(double, double, int, int, double, double, double, double, double, double, double&, double&);
void uHorizon_IPM_OpenCV(const cv::Mat&, const std::string&, double, double, double, double, double, double);
void worldToImage(double, double, int, int, double, double, double, double, double, double, double&, double&);

// HW3 functions
void a();
void uHorizon_IPM(BmpInfo, char*, int, double, double, double, double, double, double);
void RunHoughLaneDetection();
void bonus_run();

// CV versions
void ipm_cv();
void RunHoughLaneDetection_OpenCV();

// other functions
void runAutoStitch();
void runImageStitchingOpenCV();

#endif  // MYHEADER_H#pragma once
