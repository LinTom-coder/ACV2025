#include "Header.h"

// ==========================================
// Part 2: 基礎數學與矩陣運算
// ==========================================

struct Point2D {
    double r; // row (y)
    double c; // col (x)
};

struct MatchPair {
    Point2D p_src; // Right Image
    Point2D p_dst; // Left Image
};

// 3x3 矩陣行列式
double det3x3(double m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

// 3x3 矩陣反矩陣
bool invert3x3(double src[3][3], double dst[3][3]) {
    double det = det3x3(src);
    if (std::abs(det) < 1e-12) return false;
    double invDet = 1.0 / det;

    dst[0][0] = (src[1][1] * src[2][2] - src[1][2] * src[2][1]) * invDet;
    dst[0][1] = (src[0][2] * src[2][1] - src[0][1] * src[2][2]) * invDet;
    dst[0][2] = (src[0][1] * src[1][2] - src[0][2] * src[1][1]) * invDet;

    dst[1][0] = (src[1][2] * src[2][0] - src[1][0] * src[2][2]) * invDet;
    dst[1][1] = (src[0][0] * src[2][2] - src[0][2] * src[2][0]) * invDet;
    dst[1][2] = (src[0][2] * src[1][0] - src[0][0] * src[1][2]) * invDet;

    dst[2][0] = (src[1][0] * src[2][1] - src[1][1] * src[2][0]) * invDet;
    dst[2][1] = (src[0][1] * src[2][0] - src[0][0] * src[2][1]) * invDet;
    dst[2][2] = (src[0][0] * src[1][1] - src[0][1] * src[1][0]) * invDet;
    return true;
}

// 高斯消去法求解 Ax = b
void solve_linear_system_8x8(double A[8][8], double b[8], double x[8]) {
    int n = 8;
    // 簡單的高斯消去法，為了篇幅省略 pivot 檢查的嚴謹處理，但在圖像拼接場景通常夠用
    for (int i = 0; i < n; i++) {
        // Pivot
        int pivot = i;
        for (int j = i + 1; j < n; j++) {
            if (std::abs(A[j][i]) > std::abs(A[pivot][i])) pivot = j;
        }
        // Swap rows
        for (int k = 0; k < n; k++) std::swap(A[i][k], A[pivot][k]);
        std::swap(b[i], b[pivot]);

        // Eliminate
        for (int j = i + 1; j < n; j++) {
            double factor = A[j][i] / A[i][i];
            for (int k = i; k < n; k++) A[j][k] -= factor * A[i][k];
            b[j] -= factor * b[i];
        }
    }
    // Back substitution
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) sum += A[i][j] * x[j];
        x[i] = (b[i] - sum) / A[i][i];
    }
}

// 3x3 矩陣相乘: C = A * B
inline void multiply3x3(double A[3][3], double B[3][3], double C[3][3]) {
    double temp[3][3] = { 0 };
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            for (int k = 0; k < 3; k++) {
                temp[r][c] += A[r][k] * B[k][c];
            }
        }
    }
    std::memcpy(C, temp, 9 * sizeof(double));
}

// 計算單應性矩陣 H (src -> dst)
// 改良版: 包含 Data Normalization 的 DLT
inline void calculate_homography(const std::array<Point2D, 4>& src, const std::array<Point2D, 4>& dst, double finalH[9]) {
    // 1. 計算重心 (Centroid)
    double src_mean_x = 0, src_mean_y = 0;
    double dst_mean_x = 0, dst_mean_y = 0;
    for (int i = 0; i < 4; i++) {
        src_mean_y += src[i].r; src_mean_x += src[i].c;
        dst_mean_y += dst[i].r; dst_mean_x += dst[i].c;
    }
    src_mean_y /= 4.0; src_mean_x /= 4.0;
    dst_mean_y /= 4.0; dst_mean_x /= 4.0;

    // 2. 計算平均距離 (Scale)
    double src_scale = 0, dst_scale = 0;
    for (int i = 0; i < 4; i++) {
        src_scale += std::abs(src[i].r - src_mean_y) + std::abs(src[i].c - src_mean_x);
        dst_scale += std::abs(dst[i].r - dst_mean_y) + std::abs(dst[i].c - dst_mean_x);
    }
    src_scale = (src_scale > 1e-6) ? (4.0 * sqrt(2.0) / src_scale) : 1.0;
    dst_scale = (dst_scale > 1e-6) ? (4.0 * sqrt(2.0) / dst_scale) : 1.0;

    // 3. 建立正規化矩陣 T (Shift & Scale)
    double T_src[3][3] = {
        {src_scale, 0, -src_scale * src_mean_y}, // 這裡用 Row 座標 (y)
        {0, src_scale, -src_scale * src_mean_x}, // 這裡用 Col 座標 (x)
        {0, 0, 1}
    };
    double T_dst[3][3] = {
        {dst_scale, 0, -dst_scale * dst_mean_y},
        {0, dst_scale, -dst_scale * dst_mean_x},
        {0, 0, 1}
    };

    // 4. 正規化點座標
    double norm_src_r[4], norm_src_c[4];
    double norm_dst_r[4], norm_dst_c[4];
    for (int i = 0; i < 4; i++) {
        norm_src_r[i] = T_src[0][0] * src[i].r + T_src[0][1] * src[i].c + T_src[0][2];
        norm_src_c[i] = T_src[1][0] * src[i].r + T_src[1][1] * src[i].c + T_src[1][2];

        norm_dst_r[i] = T_dst[0][0] * dst[i].r + T_dst[0][1] * dst[i].c + T_dst[0][2];
        norm_dst_c[i] = T_dst[1][0] * dst[i].r + T_dst[1][1] * dst[i].c + T_dst[1][2];
    }

    // 5. DLT 求解 (使用正規化後的點)
    double A[8][8] = { 0 };
    double b[8] = { 0 };
    for (int i = 0; i < 4; i++) {
        double u = norm_src_r[i];
        double v = norm_src_c[i];
        double y = norm_dst_r[i];
        double x = norm_dst_c[i];

        // Row 1 mapping y
        A[2 * i][0] = u; A[2 * i][1] = v; A[2 * i][2] = 1;
        A[2 * i][6] = -u * y; A[2 * i][7] = -v * y;
        b[2 * i] = y;

        // Row 2 mapping x
        A[2 * i + 1][3] = u; A[2 * i + 1][4] = v; A[2 * i + 1][5] = 1;
        A[2 * i + 1][6] = -u * x; A[2 * i + 1][7] = -v * x;
        b[2 * i + 1] = x;
    }

    double h_norm[8];
    solve_linear_system_8x8(A, b, h_norm);

    double H_prime[3][3] = {
        {h_norm[0], h_norm[1], h_norm[2]},
        {h_norm[3], h_norm[4], h_norm[5]},
        {h_norm[6], h_norm[7], 1.0}
    };

    // 6. 還原矩陣: H = T_dst_inv * H_prime * T_src
    double T_dst_inv[3][3];
    invert3x3(T_dst, T_dst_inv);

    double Temp[3][3];
    multiply3x3(H_prime, T_src, Temp);      // Temp = H' * T_src
    multiply3x3(T_dst_inv, Temp, H_prime);  // Final = T_dst_inv * Temp

    // 輸出到 finalH
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            finalH[r * 3 + c] = H_prime[r][c];
}

// ==========================================
// Part 3: 特徵偵測與配對
// ==========================================

// 輔助：取得某點的灰階值
double get_gray(const BmpInfo& img, int r, int c) {
    if (r < 0 || r >= img.height || c < 0 || c >= img.width) return 0.0;
    size_t idx = ((size_t)r * img.width + c) * 3;
    // RGB to Gray
    return 0.299 * img.data[idx] + 0.587 * img.data[idx + 1] + 0.114 * img.data[idx + 2];
}

// 1. Harris Corner Detector
std::vector<Point2D> detect_harris(const BmpInfo& img, double threshold = 100000.0) {
    std::vector<double> response(img.width * img.height, 0.0);
    int h = img.height;
    int w = img.width;
    double k = 0.04;

    // 計算 Harris Response
    for (int r = 1; r < h - 1; r++) {
        for (int c = 1; c < w - 1; c++) {
            // Window 3x3 gradient sum
            double Sxx = 0, Syy = 0, Sxy = 0;
            for (int wr = -1; wr <= 1; wr++) {
                for (int wc = -1; wc <= 1; wc++) {
                    double gx = get_gray(img, r + wr, c + wc + 1) - get_gray(img, r + wr, c + wc - 1);
                    double gy = get_gray(img, r + wr + 1, c + wc) - get_gray(img, r + wr - 1, c + wc);
                    Sxx += gx * gx;
                    Syy += gy * gy;
                    Sxy += gx * gy;
                }
            }
            double det = Sxx * Syy - Sxy * Sxy;
            double trace = Sxx + Syy;
            double R = det - k * trace * trace;
            if (R > threshold) response[r * w + c] = R;
        }
    }

    // NMS (非極大值抑制 10x10)
    std::vector<Point2D> corners;
    int margin = 10;
    for (int r = margin; r < h - margin; r++) {
        for (int c = margin; c < w - margin; c++) {
            double val = response[r * w + c];
            if (val == 0) continue;
            bool localMax = true;
            for (int nr = -margin; nr <= margin; nr++) {
                for (int nc = -margin; nc <= margin; nc++) {
                    if (response[(r + nr) * w + (c + nc)] > val) {
                        localMax = false; break;
                    }
                }
                if (!localMax) break;
            }
            if (localMax) corners.push_back({ (double)r, (double)c });
        }
    }
    return corners;
}

// 2. SSD Matching (Block Matching)
std::vector<MatchPair> match_features(const BmpInfo& img1, const std::vector<Point2D>& c1,
    const BmpInfo& img2, const std::vector<Point2D>& c2) {
    std::vector<MatchPair> matches;
    int patch = 4; // 9x9 window (-4 to +4)

    for (const auto& p1 : c1) {
        double minSSD = std::numeric_limits<double>::max();
        Point2D bestP2 = { -1, -1 };

        for (const auto& p2 : c2) {
            // 簡單篩選：假設垂直位移不大 (全景圖特性)，加速比對
            if (std::abs(p1.r - p2.r) > 20) continue;

            double ssd = 0;
            for (int dr = -patch; dr <= patch; dr++) {
                for (int dc = -patch; dc <= patch; dc++) {
                    size_t idx1 = ((size_t)(p1.r + dr) * img1.width + (p1.c + dc)) * 3;
                    size_t idx2 = ((size_t)(p2.r + dr) * img2.width + (p2.c + dc)) * 3;
                    double diffR = img1.data[idx1] - img2.data[idx2];
                    double diffG = img1.data[idx1 + 1] - img2.data[idx2 + 1];
                    double diffB = img1.data[idx1 + 2] - img2.data[idx2 + 2];
                    ssd += diffR * diffR + diffG * diffG + diffB * diffB;
                }
            }
            if (ssd < minSSD) {
                minSSD = ssd;
                bestP2 = p2;
            }
        }
        // Threshold check
        if (minSSD < 2000000) { // 數值需視情況調整
            matches.push_back({ p1, bestP2 });
        }
    }
    return matches;
}

// 3. RANSAC 找最佳矩陣
void compute_best_homography(const std::vector<MatchPair>& matches, double finalH[9],
    int offset_r, int offset_c) {
    int max_iters = 2000;
    int best_inliers = 0;

    // === 修改 1: 初始化為單位矩陣 (Identity Matrix) ===
    // 這樣如果 RANSAC 失敗，至少矩陣是 "不變動"，可以被反轉，不會報錯
    double best_H[9] = { 1.0, 0.0, 0.0,
                         0.0, 1.0, 0.0,
                         0.0, 0.0, 1.0 };

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, (int)matches.size() - 1);

    // 檢查配對數量
    if (matches.size() < 4) {
        printf("Not enough matches for RANSAC (%zu < 4). Using Identity matrix.\n", matches.size());
        std::memcpy(finalH, best_H, 9 * sizeof(double));
        return;
    }

    for (int iter = 0; iter < max_iters; iter++) {
        std::array<Point2D, 4> src, dst;

        // ... (中間 RANSAC 抽樣與計算邏輯保持不變) ...
        // 請保留你原本的 for 迴圈內容
        // ...
        for (int k = 0; k < 4; k++) {
            int idx = dist(rng);
            src[k] = matches[idx].p_src;
            dst[k] = { matches[idx].p_dst.r + offset_r, matches[idx].p_dst.c + offset_c };
        }

        double H[9];
        calculate_homography(src, dst, H);

        int inliers = 0;
        for (const auto& m : matches) {
            double u = m.p_src.r, v = m.p_src.c;
            double true_y = m.p_dst.r + offset_r;
            double true_x = m.p_dst.c + offset_c;

            double pred_y = H[0] * u + H[1] * v + H[2];
            double pred_x = H[3] * u + H[4] * v + H[5];
            double w = H[6] * u + H[7] * v + H[8];

            if (std::abs(w) > 1e-5) {
                pred_y /= w;
                pred_x /= w;
                double dist_val = (pred_y - true_y) * (pred_y - true_y) + (pred_x - true_x) * (pred_x - true_x);
                // 放寬一點距離容忍度，原本 9.0 (3pixel)，改成 25.0 (5pixel)
                if (dist_val < 25.0) inliers++;
            }
        }

        // ... (更新 best_inliers 邏輯) ...
        if (inliers > best_inliers) {
            best_inliers = inliers;
            std::memcpy(best_H, H, 9 * sizeof(double)); // 更新最佳矩陣
        }
    }

    // 將結果複製回 finalH
    std::memcpy(finalH, best_H, 9 * sizeof(double));
    printf("RANSAC found %d inliers from %zu matches.\n", best_inliers, matches.size());
}

// ==========================================
// Part 4: 影像拼接 (Inverse Warping)
// ==========================================

void stitch_image(BmpInfo& canvas, const BmpInfo& src, double H[9]) {
    double H_inv[3][3], H_src[3][3];
    // Convert 1D H array to 2D
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) H_src[i][j] = H[i * 3 + j];

    if (!invert3x3(H_src, H_inv)) {
        printf("Cannot invert Homography matrix.\n");
        return;
    }

    // 為了效能，只掃描需要的範圍 (或者掃描全圖)
    // 這裡掃描全 Canvas
    for (int r = 0; r < canvas.height; r++) {
        for (int c = 0; c < canvas.width; c++) {
            // 如果 Canvas 該點已經有顏色 (來自 Left)，我們可以選擇不覆蓋或融合
            // 這裡簡單實作：如果是黑色 (0,0,0) 才填入 Right 的像素
            size_t idx = ((size_t)r * canvas.width + c) * 3;
        /*    if (canvas.data[idx] != 0 || canvas.data[idx + 1] != 0 || canvas.data[idx + 2] != 0)
                continue;*/

            // Inverse Mapping: Canvas(r,c) -> Src(u,v)
            double y = r, x = c;
            double u = H_inv[0][0] * y + H_inv[0][1] * x + H_inv[0][2];
            double v = H_inv[1][0] * y + H_inv[1][1] * x + H_inv[1][2];
            double w = H_inv[2][0] * y + H_inv[2][1] * x + H_inv[2][2];

            if (std::abs(w) < 1e-5) continue;
            u /= w; // row in src
            v /= w; // col in src

            if (u >= 0 && u < src.height - 1 && v >= 0 && v < src.width - 1) {
                // 雙線性插值 (Bilinear Interpolation)
                int r0 = (int)u; int c0 = (int)v;
                int r1 = r0 + 1; int c1 = c0 + 1;
                double dr = u - r0; double dc = v - c0;

                auto pixel = [&](int rr, int cc, int ch) {
                    return (double)src.data[((size_t)rr * src.width + cc) * 3 + ch];
                    };

                for (int ch = 0; ch < 3; ch++) {
                    double val = (1 - dr) * (1 - dc) * pixel(r0, c0, ch) +
                        (1 - dr) * dc * pixel(r0, c1, ch) +
                        dr * (1 - dc) * pixel(r1, c0, ch) +
                        dr * dc * pixel(r1, c1, ch);

                    canvas.data[idx + ch] = (unsigned char)std::min(255.0, std::max(0.0, val));
                }
            }
        }
    }
}

// ==========================================
// Part 5: 主執行流程
// ==========================================

void runAutoStitch() {
	char leftPath[] = "left.bmp";
	char rightPath[] = "right.bmp";

    BmpInfo left, right;
    if (!bmpRead(leftPath, left)) return;
    if (!bmpRead(rightPath, right)) return;

    // 1. 建立 Canvas
    BmpInfo canvas;
    canvas.width = 900; // 預估大小
    canvas.height = 480;
    canvas.channels = 3;
    canvas.data.resize(canvas.width * canvas.height * 3, 0); // 初始全黑

    // 2. 將 Left 貼到 Canvas 的固定位置
    int offset_r = 0; // 根據需要調整
    int offset_c = 0;

    for (int r = 0; r < left.height; r++) {
        for (int c = 0; c < left.width; c++) {
            if (r + offset_r >= canvas.height || c + offset_c >= canvas.width) continue;
            size_t srcIdx = ((size_t)r * left.width + c) * 3;
            size_t dstIdx = ((size_t)(r + offset_r) * canvas.width + (c + offset_c)) * 3;
            canvas.data[dstIdx] = left.data[srcIdx];
            canvas.data[dstIdx + 1] = left.data[srcIdx + 1];
            canvas.data[dstIdx + 2] = left.data[srcIdx + 2];
        }
    }

    printf("Detecting features...\n");
    // 3. 特徵偵測
    auto cornersR = detect_harris(right);
    auto cornersL = detect_harris(left);
    printf("Left corners: %zu, Right corners: %zu\n", cornersL.size(), cornersR.size());

    // 4. 特徵配對 (Right -> Left)
    auto matches = match_features(right, cornersR, left, cornersL);
    printf("Matches found: %zu\n", matches.size());

    // 5. RANSAC 計算矩陣
    double M[9];
    compute_best_homography(matches, M, offset_r, offset_c);

    // 6. 拼接 Right Image
    stitch_image(canvas, right, M);

    // 7. 儲存
	char stitchPath[] = "auto_stitched.bmp";
    bmpWrite(canvas, stitchPath);
    printf("Done. Saved auto_stitched.bmp\n");
}