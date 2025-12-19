#include "Header.h"

void imageToWorld(double u, double v,
    int m, int n,
    double dx, double dy, double dz,
    double gamma0, double theta0, double alpha,
    double& x, double& y)
{
    double angleU = theta0 - alpha + u * (2 * alpha / (m - 1));
    double angleV = gamma0 - alpha + v * (2 * alpha / (n - 1));

    double cotU = 1.0 / tan(angleU);
    double L = dz * cotU;

    x = L * sin(angleV) + dx;
    y = L * cos(angleV) + dy;
}

void worldToImage(double x, double y,
    int m, int n,
    double dx, double dy, double dz,
    double gamma0, double theta0, double alpha,
    double& u, double& v)
{
    double phi = atan2(x - dx, y - dy);
    double denom = (x - dx);

    if (fabs(denom) < 1e-6)
        denom = (denom >= 0 ? 1e-6 : -1e-6);

    double tmp = atan(dz * sin(phi) / denom);

    u = (m - 1) / (2 * alpha) * (tmp - theta0 + alpha);
    v = (n - 1) / (2 * alpha) * (phi - gamma0 + alpha);
}

void uHorizon_IPM(BmpInfo img, char* output, int view = 20, double alpha = 15 * 3.14 / 180, double dx = 4, double dy = -10, double dz = 5, double r0 = 0, double theda = 0.025) {
    int m = img.height; // rows (u direction)
    int n = img.width;

    if (m <= 1 || n <= 1) {
        printf("Input image too small: %d x %d\n", n, m);
        return;
    }

    // 求地平線
    int uHorizon = (int)ceil((m - 1) / (2 * alpha) * (-theda + alpha)) + view;
    if (uHorizon < 0) uHorizon = 0;
    if (uHorizon >= m) uHorizon = m - 1;

    // forward: 求 (x,y) 範圍
    double x_min = DBL_MAX, x_max = -DBL_MAX;
    double y_min = DBL_MAX, y_max = -DBL_MAX;

    for (int u = uHorizon; u < m; u++) {
        for (int v = 0; v < n; v++) {
            double x, y;
            imageToWorld(u, v, m, n, dx, dy, dz, r0, theda, alpha, x, y);
            if (x < x_min) x_min = x;
            if (x > x_max) x_max = x;
            if (y < y_min) y_min = y;
            if (y > y_max) y_max = y;
        }
    }

    if (x_min == DBL_MAX || y_min == DBL_MAX) {
        printf("Failed to compute world bounds (no valid points).\n");
        return;
    }

    double r1 = 100.0; 
    double r2 = 100.0; 
    if (view >= 20) {
        r1 = 1.0;
        r2 = 1.0;
	}
    else {
        r1 = 100.0 ;
		r2 = 100.0 ;
    }


    double x_range = x_max - x_min;
    double y_range = y_max - y_min;
    if (x_range <= 0) x_range = r1;
    if (y_range <= 0) y_range = r2;

    // 計算新圖片的寬高 (使用 ceil)
    long long new_height_ll = (long long)ceil(x_range / r1);
    long long new_width_ll = (long long)ceil(y_range / r2);

    // 最小值保證與上限 clamping（避免意外爆大）
    if (new_height_ll < 1) new_height_ll = 1;
    if (new_width_ll < 1) new_width_ll = 1;
    const long long MAX_DIM = 10000; // 可調：避免分配過大記憶體
    if (new_height_ll > MAX_DIM || new_width_ll > MAX_DIM) {
        printf("Requested IPM size too large, clamping to %lld x %lld\n", MAX_DIM, MAX_DIM);
        new_height_ll = std::min(new_height_ll, MAX_DIM);
        new_width_ll = std::min(new_width_ll, MAX_DIM);
    }

    int new_height = (int)new_height_ll;
    int new_width = (int)new_width_ll;

    // 檢查總記憶體需求是否合理
    uint64_t totalPixels = (uint64_t)new_width * (uint64_t)new_height;
    uint64_t totalBytes = totalPixels * 3ULL;
    const uint64_t MAX_BYTES = 1024ULL * 1024ULL * 1024ULL; // 1GB 上限，可依需求調整
    if (totalBytes == 0 || totalBytes > MAX_BYTES) {
        printf("IPM allocation would be too large: %llu bytes (width=%d height=%d)\n",
            (unsigned long long)totalBytes, new_width, new_height);
        return;
    }

    printf("X Range: %f to %f, Y Range: %f to %f\n", x_min, x_max, y_min, y_max);
    printf("New Size: %d x %d\n", new_width, new_height);

    BmpInfo output_img;
    output_img.width = new_width;
    output_img.height = new_height;
    output_img.channels = 3;
    try {
        output_img.data.assign((size_t)totalBytes, 0);
    } catch (const bad_alloc&) {
        printf("Failed to allocate output image data (%llu bytes).\n", (unsigned long long)totalBytes);
        return;
    }

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            double x = x_min + i * r1;
            double y = y_min + j * r2;

            double u, v;
            worldToImage(x, y, m, n, dx, dy, dz, r0, theda, alpha, u, v);

            int ui = (int)round(u);
            int vi = (int)round(v);
            if (ui >= 0 && ui < m && vi >= 0 && vi < n) {
                size_t src_index = ((size_t)ui * (size_t)n + (size_t)vi) * 3;
                size_t dst_index = ((size_t)i * (size_t)new_width + (size_t)j) * 3;
                if (src_index + 2 < img.data.size() && dst_index + 2 < output_img.data.size()) {
                    output_img.data[dst_index] = img.data[src_index];
                    output_img.data[dst_index + 1] = img.data[src_index + 1];
                    output_img.data[dst_index + 2] = img.data[src_index + 2];
                }
            }
        }
    }
    //char output[] = "IPM.bmp";
    bmpWrite(output_img, output);
}
