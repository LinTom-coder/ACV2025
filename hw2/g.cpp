#include "Header.h"
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// helper：從彩色影像做二值化並以 PCA 計算長軸角度與長度
static bool computeBinaryPCAOrientation(const Mat& colorImg,
                                        double rW, double gW, double bW,
                                        int thr,
                                        double& outThetaDeg,
                                        double& outLength,
                                        Mat& outBinary)
{
    if (colorImg.empty()) return false;
    Mat imgF;
    colorImg.convertTo(imgF, CV_32F);
    vector<Mat> ch(3);
    split(imgF, ch); // ch[0]=B, ch[1]=G, ch[2]=R

    Mat grayF = ch[2] * rW + ch[1] * gW + ch[0] * bW;
    Mat gray;
    grayF.convertTo(gray, CV_8U);

    threshold(gray, outBinary, thr, 255, THRESH_BINARY);

    vector<Point2f> pts;
    pts.reserve(1024);
    for (int y = 0; y < outBinary.rows; ++y) {
        const uchar* row = outBinary.ptr<uchar>(y);
        for (int x = 0; x < outBinary.cols; ++x) {
            if (row[x] > 0) pts.emplace_back((float)x, (float)y);
        }
    }
    if (pts.empty()) return false;

    Mat data((int)pts.size(), 2, CV_32F);
    for (size_t i = 0; i < pts.size(); ++i) {
        data.at<float>((int)i, 0) = pts[i].x;
        data.at<float>((int)i, 1) = pts[i].y;
    }

    PCA pca(data, Mat(), PCA::DATA_AS_ROW);
    Mat mean64; pca.mean.convertTo(mean64, CV_64F);
    double mx = mean64.at<double>(0, 0);
    double my = mean64.at<double>(0, 1);

    Mat ev0; pca.eigenvectors.row(0).convertTo(ev0, CV_64F);
    double vx = ev0.at<double>(0, 0);
    double vy = ev0.at<double>(0, 1);
    double norm = sqrt(vx * vx + vy * vy);
    if (norm > 1e-12) { vx /= norm; vy /= norm; }

    double minp = DBL_MAX, maxp = -DBL_MAX;
    for (const auto& p : pts) {
        double proj = (p.x - mx) * vx + (p.y - my) * vy;
        if (proj < minp) minp = proj;
        if (proj > maxp) maxp = proj;
    }
    outLength = maxp - minp;
    outThetaDeg = atan2(vy, vx) * 180.0 / CV_PI;
    return true;
}

// OpenCV 版：a -> rgb -> binary
void a_opencv() {
    auto t0 = chrono::high_resolution_clock::now();
    const string infile = "Ian_island_square.bmp";
    const string outfile = "binary_of_road_cv.bmp";

    Mat img = imread(infile, IMREAD_COLOR);
    if (img.empty()) { printf("a_opencv: cannot read %s\n", infile.c_str()); return; }

    Mat imgF;
    img.convertTo(imgF, CV_32F);
    vector<Mat> ch(3);
    split(imgF, ch); // ch[0]=B, ch[1]=G, ch[2]=R

    double r = 0.667, g = 0.0, b = 0.333; // 與原始權重相同
    Mat grayF = ch[2] * r + ch[1] * g + ch[0] * b;
    Mat gray;
    grayF.convertTo(gray, CV_8U);

    int thr = 100;
    Mat binary;
    threshold(gray, binary, thr, 255, THRESH_BINARY);

    imwrite(outfile, binary);

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
    printf("a_opencv: elapsed %.2f ms\n", ms);
}

// OpenCV 版：b -> binary closing (kernel size)
void b_opencv() {
    auto t0 = chrono::high_resolution_clock::now();
    const string infile = "binary_of_road.bmp";
    const string outfile = "reserved_road.bmp";

    Mat bin = imread(infile, IMREAD_GRAYSCALE);
    if (bin.empty()) { printf("b_opencv: cannot read %s\n", infile.c_str()); return; }

    Mat bin8;
    if (bin.type() != CV_8U) bin.convertTo(bin8, CV_8U);
    else bin8 = bin;

    threshold(bin8, bin8, 127, 255, THRESH_BINARY); // 確保為二值

    int ksize = 9;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(ksize, ksize));
    Mat closed;
    morphologyEx(bin8, closed, MORPH_CLOSE, kernel);

    imwrite(outfile, closed);

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
    printf("b_opencv: elapsed %.2f ms\n", ms);
}

// OpenCV 版：c -> connected components on black regions, draw boxes on raw image
void c_opencv() {
    auto t0 = chrono::high_resolution_clock::now();
    const string maskFile = "reserved_road.bmp";
    const string rawFile = "Ian_island_square.bmp";
    const string outFile = "cc.bmp";

    Mat mask = imread(maskFile, IMREAD_GRAYSCALE);
    Mat raw = imread(rawFile, IMREAD_COLOR);
    if (mask.empty() || raw.empty()) { printf("c_opencv: cannot read files\n"); return; }

    // 我們要找原實作中 "黑色區域 (value==0)"，因此把黑色反轉為前景
    Mat bw;
    threshold(mask, bw, 127, 255, THRESH_BINARY_INV); // 黑 => 255

    Mat labels, stats, centroids;
    int ncomp = connectedComponentsWithStats(bw, labels, stats, centroids, 8, CV_32S);

    const Scalar colors[3] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) }; // BGR

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
    printf("cc: elapsed %.2f ms\n", ms);

    auto t0d = chrono::high_resolution_clock::now();
    for (int i = 1; i < ncomp; ++i) {
        int left = stats.at<int>(i, CC_STAT_LEFT);
        int top = stats.at<int>(i, CC_STAT_TOP);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int area = stats.at<int>(i, CC_STAT_AREA);
        double cx = centroids.at<double>(i, 0);
        double cy = centroids.at<double>(i, 1);

        Scalar color = colors[(i - 1) % 3];
        rectangle(raw, Rect(left, top, width, height), color, 1);
        circle(raw, Point(int(round(cx)), int(round(cy))), 2, Scalar(255, 255, 255), -1);

        printf("Region %d: Area=%d, Centroid=(%.1f, %.1f)\n", i, area, cx, cy);
    }

    imwrite(outFile, raw);

    auto t1d = chrono::high_resolution_clock::now();
    double msd = chrono::duration_cast<chrono::duration<double, milli>>(t1d - t0d).count();
    printf("drawing: elapsed %.2f ms\n", msd);
}

// OpenCV 版：d -> length and long axis using PCA (white pixels)
void d_opencv() {
    auto t0 = chrono::high_resolution_clock::now();
    const string infile = "reserved_road.bmp";

    Mat img = imread(infile, IMREAD_GRAYSCALE);
    if (img.empty()) { printf("d_opencv: cannot read %s\n", infile.c_str()); return; }

    Mat bin;
    threshold(img, bin, 127, 255, THRESH_BINARY);

    vector<Point2f> pts;
    pts.reserve(1024);
    for (int y = 0; y < bin.rows; ++y) {
        const uchar* row = bin.ptr<uchar>(y);
        for (int x = 0; x < bin.cols; ++x) {
            if (row[x] > 0) pts.emplace_back((float)x, (float)y);
        }
    }
    if (pts.empty()) { printf("d_opencv: no foreground pixels\n"); return; }

    Mat data((int)pts.size(), 2, CV_32F);
    for (size_t i = 0; i < pts.size(); ++i) {
        data.at<float>((int)i, 0) = pts[i].x;
        data.at<float>((int)i, 1) = pts[i].y;
    }

    PCA pca(data, Mat(), PCA::DATA_AS_ROW);

    Mat mean64; pca.mean.convertTo(mean64, CV_64F);
    double mx = mean64.at<double>(0, 0);
    double my = mean64.at<double>(0, 1);

    Mat ev0; pca.eigenvectors.row(0).convertTo(ev0, CV_64F);
    double vx = ev0.at<double>(0, 0);
    double vy = ev0.at<double>(0, 1);
    double norm = sqrt(vx * vx + vy * vy);
    if (norm > 1e-12) { vx /= norm; vy /= norm; }

    double minp = DBL_MAX, maxp = -DBL_MAX;
    for (const auto& p : pts) {
        double proj = (p.x - mx) * vx + (p.y - my) * vy;
        if (proj < minp) minp = proj;
        if (proj > maxp) maxp = proj;
    }
    double length = maxp - minp;
    double theta = atan2(vy, vx) * 180.0 / CV_PI;

    printf("Orientation: %.2f degrees\n", theta);
    printf("Length: %.2f pixels\n", length);

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
    printf("d_opencv: elapsed %.2f ms\n", ms);
}

void f_opencv() {
    auto t_total_start = chrono::high_resolution_clock::now();

    const string infile = "Ian_island_square.bmp";
    const string out_rot_file = "rotation_bonus.bmp";
    const string out_bin_orig = "orig_binary.bmp";
    const string out_bin_rot = "rot_binary.bmp";

    Mat img = imread(infile, IMREAD_COLOR);
    if (img.empty()) { printf("f_opencv: cannot read %s\n", infile.c_str()); return; }

    // rotate 90° CW
    auto t_rot_start = chrono::high_resolution_clock::now();
    Mat rotated;
    transpose(img, rotated);
    flip(rotated, rotated, 1);
    imwrite(out_rot_file, rotated);
    auto t_rot_end = chrono::high_resolution_clock::now();

    // parameters (與其他函式一致)
    double rW = 0.667, gW = 0.0, bW = 0.333;
    int thr = 100;

    // original binary + PCA
    auto t_orig_start = chrono::high_resolution_clock::now();
    double theta_orig = 0.0, length_orig = 0.0;
    Mat bin_orig;
    bool ok_orig = computeBinaryPCAOrientation(img, rW, gW, bW, thr, theta_orig, length_orig, bin_orig);
    if (ok_orig) imwrite(out_bin_orig, bin_orig);
    auto t_orig_end = chrono::high_resolution_clock::now();

    // rotated binary + PCA
    auto t_rotbin_start = chrono::high_resolution_clock::now();
    double theta_rot = 0.0, length_rot = 0.0;
    Mat bin_rot;
    bool ok_rot = computeBinaryPCAOrientation(rotated, rW, gW, bW, thr, theta_rot, length_rot, bin_rot);
    if (ok_rot) imwrite(out_bin_rot, bin_rot);
    auto t_rotbin_end = chrono::high_resolution_clock::now();

    if (!ok_orig || !ok_rot) {
        printf("f_opencv: unable to compute orientation (no foreground pixels?)\n");
        return;
    }

    // normalize angle to (-180,180]
    auto normalize = [](double a) {
        while (a <= -180.0) a += 360.0;
        while (a > 180.0) a -= 360.0;
        return a;
    };

    double diff = normalize(theta_rot - theta_orig);
    // PCA 主軸無方向性：角度等價於角度+180，將差值映射到 (-90,90]
    if (diff > 90.0) diff -= 180.0;
    else if (diff <= -90.0) diff += 180.0;

    printf("Orientation (orig binary): %.2f deg, length=%.2f px\n", theta_orig, length_orig);
    printf("Orientation (rot  binary): %.2f deg, length=%.2f px\n", theta_rot, length_rot);
    printf("Measured rotation (rot - orig, mapped to (-90,90]): %.2f deg\n", diff);

    double rot_ms = chrono::duration_cast<chrono::duration<double, milli>>(t_rot_end - t_rot_start).count();
    double orig_ms = chrono::duration_cast<chrono::duration<double, milli>>(t_orig_end - t_orig_start).count();
    double rotbin_ms = chrono::duration_cast<chrono::duration<double, milli>>(t_rotbin_end - t_rotbin_start).count();
    double total_ms = chrono::duration_cast<chrono::duration<double, milli>>(chrono::high_resolution_clock::now() - t_total_start).count();

    printf("f_opencv: rotation=%.2f ms, orig_binary+PCA=%.2f ms, rot_binary+PCA=%.2f ms, total=%.2f ms\n",
        rot_ms, orig_ms, rotbin_ms, total_ms);
}