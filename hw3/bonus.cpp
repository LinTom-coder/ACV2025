#include "Header.h"

struct Point2D {
    double r; // Row coordinate (corresponds to x in the prompt's notation)
    double c; // Col coordinate (corresponds to y in the prompt's notation)
};

// 求解 8x8 線性方程組 (Gaussian Elimination)
// 用於解 PDF 中的 Eq.(3)
inline void solve_system_8x8(double A[8][8], double b[8], double x[8]) {
    int n = 8;
    for (int i = 0; i < n; i++) {
        // Pivot selection
        int pivot = i;
        for (int j = i + 1; j < n; j++) {
            if (std::abs(A[j][i]) > std::abs(A[pivot][i])) pivot = j;
        }
        // Swap
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

// src: (u, v) - Right Image coordinates
// dst: (x, y) - Canvas coordinates
inline void evaluate_M_Eq3(const std::array<Point2D, 4>& src, const std::array<Point2D, 4>& dst, double M[9]) {
    double A[8][8] = { 0 };
    double b[8] = { 0 };

    // Eq.(3) form:
    // x_k = (a*u + b*v + c) / (g*u + h*v + 1)
    // y_k = (d*u + e*v + f) / (g*u + h*v + 1)
    // Linearized:
    // u*a + v*b + c - u*x*g - v*x*h = x
    // u*d + v*e + f - u*y*g - v*y*h = y

    for (int k = 0; k < 4; k++) {
        double u = src[k].r; // source row
        double v = src[k].c; // source col
        double x = dst[k].r; // target row
        double y = dst[k].c; // target col

        // Row 2*k: equation for x (row)
        A[2 * k][0] = u; A[2 * k][1] = v; A[2 * k][2] = 1;
        A[2 * k][3] = 0; A[2 * k][4] = 0; A[2 * k][5] = 0;
        A[2 * k][6] = -u * x; A[2 * k][7] = -v * x;
        b[2 * k] = x;

        // Row 2*k+1: equation for y (col)
        A[2 * k + 1][0] = 0; A[2 * k + 1][1] = 0; A[2 * k + 1][2] = 0;
        A[2 * k + 1][3] = u; A[2 * k + 1][4] = v; A[2 * k + 1][5] = 1;
        A[2 * k + 1][6] = -u * y; A[2 * k + 1][7] = -v * y;
        b[2 * k + 1] = y;
    }

    double coeffs[8];
    solve_system_8x8(A, b, coeffs);

    // M = [[a, b, c], [d, e, f], [g, h, 1]]
    M[0] = coeffs[0]; M[1] = coeffs[1]; M[2] = coeffs[2];
    M[3] = coeffs[3]; M[4] = coeffs[4]; M[5] = coeffs[5];
    M[6] = coeffs[6]; M[7] = coeffs[7]; M[8] = 1.0;
}

// 3x3 矩陣反轉 (用於 Inverse Warping)
inline bool invert3x3(const double src[9], double dst[9]) {
    double m[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) m[i][j] = src[i * 3 + j];

    double det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    if (std::abs(det) < 1e-12) return false;
    double invDet = 1.0 / det;

    dst[0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
    dst[1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
    dst[2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    dst[3] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
    dst[4] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    dst[5] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;
    dst[6] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
    dst[7] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet;
    dst[8] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;
    return true;
}

// ==========================================
// Part 3: 主流程
// ==========================================
void bonus_run() {
    // 1. 讀取圖片
    char leftPath[] = "left.bmp";
    char rightPath[] = "right.bmp";
    BmpInfo left, right;
    if (!bmpRead(leftPath, left)) return;
    if (!bmpRead(rightPath, right)) return;

    printf("Left: %dx%d, Right: %dx%d\n", left.width, left.height, right.width, right.height);

    // 2. 建立 Canvas (900x480)
    BmpInfo canvas;
    canvas.width = 900;
    canvas.height = 480;
    canvas.channels = 3;
    canvas.data.resize(900 * 480 * 3, 0); // Initialize black

    // 3. 貼上左圖: [80:449, 0:509]
    // 根據作業說明，Row 範圍 80~449 (高度 370)，Col 範圍 0~509 (寬度 510)
    int paste_r_start = 80;
    int paste_r_end = 449;
    int paste_c_start = 0;
    int paste_c_end = 509;

    for (int r = paste_r_start; r <= paste_r_end; r++) {
        for (int c = paste_c_start; c <= paste_c_end; c++) {
            // 從 left.bmp 的 (r-80, c) 取值
            int lr = r - paste_r_start;
            int lc = c - paste_c_start;

            // 邊界檢查
            if (lr >= 0 && lr < left.height && lc >= 0 && lc < left.width) {
                size_t srcIdx = ((size_t)lr * left.width + lc) * 3;
                size_t dstIdx = ((size_t)r * canvas.width + c) * 3;
                canvas.data[dstIdx + 0] = left.data[srcIdx + 0];
                canvas.data[dstIdx + 1] = left.data[srcIdx + 1];
                canvas.data[dstIdx + 2] = left.data[srcIdx + 2];
            }
        }
    }

    // 4. 設定對應點 (Refer to slide)
    // 假設作業中的 (x, y) 指的是 (Row, Col)，因為 900 (Canvas Width) 出現在第二個分量
    // Src Points (Right Image Corners) - Cyclic order: TL, BL, BR, TR
    std::array<Point2D, 4> src_pts = {
        Point2D{0.0, 0.0},                       // Top-Left
        Point2D{(double)right.height, 0.0},      // Bottom-Left
        Point2D{(double)right.height, (double)right.width}, // Bottom-Right
        Point2D{0.0, (double)right.width}        // Top-Right
    };

    // Dst Points (Canvas Coordinates) - From slide
    // x0=130, y0=250 (TL)
    // x1=470, y1=310 (BL)
    // x2=475, y2=900 (BR)
    // x3=0,   y3=770 (TR)
    std::array<Point2D, 4> dst_pts = {
        Point2D{130.0, 250.0},
        Point2D{470.0, 310.0},
        Point2D{475.0, 900.0},
        Point2D{0.0,   770.0}
    };

    // 5. 計算 M (Eq.3)
    double M[9];
    evaluate_M_Eq3(src_pts, dst_pts, M);

    printf("Transformation Matrix M evaluated.\n");

    // 6. 計算 M_inv 用於 Inverse Warping
    double M_inv[9];
    if (!invert3x3(M, M_inv)) {
        printf("Matrix Inversion Failed!\n");
        return;
    }

    // 7. Warp Right Image onto Canvas (覆蓋模式)
    // 掃描整個 Canvas 或是只掃描 Right Image 投影範圍 (Bounding Box)
    // 為了簡單，掃描全 Canvas
    for (int r = 0; r < canvas.height; r++) {
        for (int c = 0; c < canvas.width; c++) {
            // Inverse Mapping: Canvas(r,c) -> Right(u,v)
            // [u, v, w]' = M_inv * [r, c, 1]'
            double u = M_inv[0] * r + M_inv[1] * c + M_inv[2];
            double v = M_inv[3] * r + M_inv[4] * c + M_inv[5];
            double w = M_inv[6] * r + M_inv[7] * c + M_inv[8];

            if (std::abs(w) < 1e-6) continue;
            u /= w;
            v /= w;

            // 檢查是否在 Right Image 範圍內
            if (u >= 0 && u < right.height - 1 && v >= 0 && v < right.width - 1) {
                // Bilinear Interpolation
                int r0 = (int)u; int c0 = (int)v;
                int r1 = r0 + 1; int c1 = c0 + 1;
                double dr = u - r0; double dc = v - c0;

                auto get_val = [&](int rr, int cc, int ch) {
                    return (double)right.data[((size_t)rr * right.width + cc) * 3 + ch];
                    };

                size_t dstIdx = ((size_t)r * canvas.width + c) * 3;

                // 覆蓋寫入 (不檢查黑色)
                for (int ch = 0; ch < 3; ch++) {
                    double val = (1 - dr) * (1 - dc) * get_val(r0, c0, ch) +
                        (1 - dr) * dc * get_val(r0, c1, ch) +
                        dr * (1 - dc) * get_val(r1, c0, ch) +
                        dr * dc * get_val(r1, c1, ch);

                    canvas.data[dstIdx + ch] = (unsigned char)std::min(255.0, std::max(0.0, val));
                }
            }
        }
    }

    // 8. 儲存結果
    char stitchPath[] = "bonus_stitched.bmp";
    bmpWrite(canvas, stitchPath);
    printf("Saved result_projective.bmp\n");
}