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
using namespace std;



struct BmpInfo {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    unsigned char* imgDataDynamic;
};
struct RegionInfo {
    int label;
    int area;
    int minx, miny, maxx, maxy;
    double sumx, sumy;   // 累加 x, y
    double cx, cy;       // 計算後的 centroid
};
struct AxisInfo {
    double cx, cy;
    double theta;
    double length;
};

BmpInfo bmpRead(char* filename);
void bmpWrite(BmpInfo, char*);
BmpInfo rgb2binary(const BmpInfo&, int, double, double, double);
BmpInfo padding(BmpInfo img, int pdsz);
BmpInfo binary_dilation(const BmpInfo& img, int kernalsz);
BmpInfo binary_erosion(const BmpInfo& img, int kernalsz);
BmpInfo binary_opening(const BmpInfo& img, int kernalsz);
BmpInfo binary_closing(const BmpInfo& img, int kernalsz);
BmpInfo connected_components(const BmpInfo& img, BmpInfo);
AxisInfo length_and_longaxis(BmpInfo img);
double rotation_estimation(BmpInfo, BmpInfo);
void a();
void b();
void c();
void d();
void e();
void f();
void a_opencv();
void b_opencv();
void c_opencv();
void d_opencv();
void f_opencv();

#endif  // MYHEADER_H#pragma once
