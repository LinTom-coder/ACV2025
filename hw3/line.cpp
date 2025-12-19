#include "Header.h"

std::vector<unsigned char> convertToGray(const BmpInfo& img) {
    std::vector<unsigned char> gray(img.width * img.height);
    for (int i = 0; i < img.width * img.height; ++i) {
        unsigned char r = img.data[i * 3 + 0];
        unsigned char g = img.data[i * 3 + 1];
        unsigned char b = img.data[i * 3 + 2];
        // 心理學灰階公式: 0.299R + 0.587G + 0.114B
        gray[i] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
    }
    return gray;
}

std::vector<unsigned char> simpleBlur(const std::vector<unsigned char>& src, int w, int h) {
    std::vector<unsigned char> dst = src; // 複製邊緣
    // 3x3 Kernel:
    // 1 2 1
    // 2 4 2
    // 1 2 1
    // Sum = 16
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int sum = 0;
            sum += src[(y - 1) * w + (x - 1)] * 1;
            sum += src[(y - 1) * w + (x)] * 2;
            sum += src[(y - 1) * w + (x + 1)] * 1;

            sum += src[(y)*w + (x - 1)] * 2;
            sum += src[(y)*w + (x)] * 4;
            sum += src[(y)*w + (x + 1)] * 2;

            sum += src[(y + 1) * w + (x - 1)] * 1;
            sum += src[(y + 1) * w + (x)] * 2;
            sum += src[(y + 1) * w + (x + 1)] * 1;

            dst[y * w + x] = (unsigned char)(sum / 16);
        }
    }
    return dst;
}

std::vector<unsigned char> sobelEdge(const std::vector<unsigned char>& src, int w, int h, int threshold) {
    std::vector<unsigned char> edges(w * h, 0);

    // Gx:         Gy:
    // -1  0  1    -1 -2 -1
    // -2  0  2     0  0  0
    // -1  0  1     1  2  1

    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            // Gx
            int gx = -1 * src[(y - 1) * w + (x - 1)] + 1 * src[(y - 1) * w + (x + 1)]
                - 2 * src[(y)*w + (x - 1)] + 2 * src[(y)*w + (x + 1)]
                - 1 * src[(y + 1) * w + (x - 1)] + 1 * src[(y + 1) * w + (x + 1)];

            // Gy
            int gy = -1 * src[(y - 1) * w + (x - 1)] - 2 * src[(y - 1) * w + (x)] - 1 * src[(y - 1) * w + (x + 1)]
                + 1 * src[(y + 1) * w + (x - 1)] + 2 * src[(y + 1) * w + (x)] + 1 * src[(y + 1) * w + (x + 1)];

            int magnitude = (int)std::sqrt(gx * gx + gy * gy);

            // 二值化
            if (magnitude > threshold) {
                edges[y * w + x] = 255;
            }
            else {
                edges[y * w + x] = 0;
            }
        }
    }
    return edges;
}

std::vector<HoughLine> houghTransform(
    const std::vector<unsigned char>& edgeImg, int w, int h, int threshold)
{
    // 1. 參數設定
    // Theta 解析度: 1度 (轉為弧度)
    int numThetas = 180;
    double dTheta = M_PI / numThetas;

    // Rho 解析度: 1 pixel
    // Max Rho 是對角線長度
    int maxRho = (int)std::ceil(std::sqrt(w * w + h * h));
    int numRhos = 2 * maxRho + 1; // Rho 可能是負的，所以要做 offset

    // 2. 建立 Accumulator (投票箱)
    std::vector<int> accumulator(numRhos * numThetas, 0);

    // 3. 預先計算 sin/cos 以加速
    std::vector<double> sinTable(numThetas);
    std::vector<double> cosTable(numThetas);
    for (int t = 0; t < numThetas; ++t) {
        double theta = t * dTheta;
        sinTable[t] = std::sin(theta);
        cosTable[t] = std::cos(theta);
    }

    // 4. 投票
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // 只有邊緣點參與投票
            if (edgeImg[y * w + x] > 0) {
                for (int t = 0; t < numThetas; ++t) {
                    // 計算 rho = x cos + y sin
                    double rhoVal = x * cosTable[t] + y * sinTable[t];
                    int rhoIdx = (int)std::round(rhoVal) + maxRho; // 加上 offset 轉為陣列索引

                    if (rhoIdx >= 0 && rhoIdx < numRhos) {
                        accumulator[rhoIdx * numThetas + t]++;
                    }
                }
            }
        }
    }
    // 5. 找出超過閾值的直線 (並且做簡單過濾)
    std::vector<HoughLine> lines;
    for (int r = 0; r < numRhos; ++r) {
        for (int t = 0; t < numThetas; ++t) {
            int votes = accumulator[r * numThetas + t];
            if (votes > threshold) {
                HoughLine line;
                line.rho = r - maxRho; // 還原 rho
                line.theta = t * dTheta;
                line.votes = votes;
                lines.push_back(line);
            }
        }
    }
    return lines;
}

// ------------------------------------------------------
// Draw thick line
// ------------------------------------------------------
void putPixel3_2(BmpInfo& img, int x, int y,
    unsigned char R, unsigned char G, unsigned char B)
{
    // 計算地平線（保留原始邏輯，但使用 double 精度以避免整數除法誤差）
    int m = img.height;
    int uHorizon = (int)std::ceil((m - 1) / (2.0 * 15.0 * 3.14 / 180.0) * (-0.025 + 15.0 * 3.14 / 180.0));

    // 邊界檢查：任何超出影像或在地平線之上的點都不畫
    if (x < 0 || x >= img.width || y < uHorizon || y >= img.height) {
        return;
    }

    // 使用 channels（如果沒設定則預設 3）來計算索引，並確保不會越界
    int channels = (img.channels > 0 ? img.channels : 3);
    size_t idx = static_cast<size_t>((y * img.width + x) * channels);
    if (idx + (size_t)channels - 1 >= img.data.size()) return;

    img.data[idx + 0] = R;
    img.data[idx + 1] = G;
    img.data[idx + 2] = B;
}

void drawThickLine3_2(BmpInfo& img, int x0, int y0, int x1, int y1,
    unsigned char R, unsigned char G, unsigned char B, int thickness)
{
    double dx = x1 - x0;
    double dy = y1 - y0;
    double len = std::sqrt(dx * dx + dy * dy);

    // 處理退化情況（起點與終點相同）以避免除以 0
    if (len <= 0.0) {
        for (int tx = -thickness; tx <= thickness; tx++) {
            for (int ty = -thickness; ty <= thickness; ty++) {
                putPixel3_2(img, x0 + tx, y0 + ty, R, G, B);
            }
        }
        return;
    }

    dx /= len; dy /= len;

    for (int i = 0; i <= (int)len; i++) {
        int px = x0 + static_cast<int>(std::round(dx * i));
        int py = y0 + static_cast<int>(std::round(dy * i));

        for (int tx = -thickness; tx <= thickness; tx++) {
            for (int ty = -thickness; ty <= thickness; ty++) {
                putPixel3_2(img, px + tx, py + ty, R, G, B);
            }
        }
    }
}

void drawHoughLine(BmpInfo& img, double rho, double theta,
    unsigned char R, unsigned char G, unsigned char B)
{
    double a = std::cos(theta);
    double b = std::sin(theta);

    int W = img.width;
    int H = img.height;

    struct Point { int x, y; };
    std::vector<Point> validPoints;

    // x = (rho - 0 * b) / a
    if (std::abs(a) > 1e-5) {
        int x = (int)(rho / a);
        if (x >= 0 && x < W) validPoints.push_back({ x, 0 });
    }
    // x = (rho - (H-1) * b) / a
    if (std::abs(a) > 1e-5) {
        int x = (int)((rho - (H - 1) * b) / a);
        if (x >= 0 && x < W) validPoints.push_back({ x, H - 1 });
    }
    // y = (rho - 0 * a) / b
    if (std::abs(b) > 1e-5) {
        int y = (int)(rho / b);
        bool duplicate = false;
        for (auto& p : validPoints) if (p.x == 0 && p.y == y) duplicate = true;

        if (!duplicate && y >= 0 && y < H) validPoints.push_back({ 0, y });
    }

    // y = (rho - (W-1) * a) / b
    if (std::abs(b) > 1e-5) {
        int y = (int)((rho - (W - 1) * a) / b);

        bool duplicate = false;
        for (auto& p : validPoints) if (p.x == W - 1 && p.y == y) duplicate = true;

        if (!duplicate && y >= 0 && y < H) validPoints.push_back({ W - 1, y });
    }

    // 畫線
    if (validPoints.size() >= 2) {
        // 通常只會有兩個交點，取最遠的兩個畫（確保線段最長）
        Point p1 = validPoints.front();
        Point p2 = validPoints.back();

        drawThickLine3_2(img, p1.x, p1.y, p2.x, p2.y, R, G, B, 3);
    }
}

void RunHoughLaneDetection()
{
	char input[] = "road.bmp";
    BmpInfo src;
    if (!bmpRead(input, src)) {
        printf("Cannot read road.bmp\n");
        return;
    }

    // 1. 轉灰階
    std::vector<unsigned char> gray = convertToGray(src);

    // 2. 模糊 (降噪)
    std::vector<unsigned char> blurred = simpleBlur(gray, src.width, src.height);

    // 3. 邊緣偵測 (Sobel)
    // 閾值設為 100 (可調整)，越高只抓越明顯的線
    std::vector<unsigned char> edges = sobelEdge(blurred, src.width, src.height, 100);

    //邊緣圖
    BmpInfo edgeImg = src; // Copy meta
    for (int i = 0; i < src.width * src.height; ++i) {
        unsigned char val = edges[i];
        edgeImg.data[i * 3 + 0] = val;
        edgeImg.data[i * 3 + 1] = val;
        edgeImg.data[i * 3 + 2] = val;
    }
	char edges_img[] = "debug_edges.bmp";
    bmpWrite(edgeImg, edges_img);


    // 4. 霍夫變換
    std::vector<HoughLine> lines = houghTransform(edges, src.width, src.height, 100);

    printf("Hough found %zu lines.\n", lines.size());

    // 5. 過濾與繪製
    HoughLine bestLeft = { 0, 0, -1 };
    HoughLine bestRight = { 0, 0, -1 };
    HoughLine middle = { 0, 0, -1 };

    for (const auto& line : lines) {
        double degree = line.theta * 180.0 / M_PI;
        if (degree > 40 && degree < 80) {
            if (line.votes > bestLeft.votes) bestLeft = line;
        }
        else if (degree > 20 && degree < 40) {
            if (line.votes > middle.votes) middle = line;
        }
        else if (degree > 135 && degree < 175) {
            if (line.votes > bestRight.votes) bestRight = line;
        }
    }
	printf("staryt drawing lines...\n");
    // 畫在原圖上
    if (bestLeft.votes > 0) {
        drawHoughLine(src, bestLeft.rho, bestLeft.theta+0.01, 0, 0, 255);
        printf("Left Lane: rho=%.1f, theta=%.1f\n", bestLeft.rho, bestLeft.theta * 180 / M_PI);
    }
    if (bestRight.votes > 0) {
        drawHoughLine(src, bestRight.rho, bestRight.theta+0.005, 0, 255, 0); 
        printf("Right Lane: rho=%.1f, theta=%.1f\n", bestRight.rho, bestRight.theta * 180 / M_PI);
    }
    if (middle.votes > 0) {
        drawHoughLine(src, middle.rho, middle.theta + 0.03, 255, 0, 0); 
        printf("middle Lane: rho=%.1f, theta=%.1f\n", middle.rho, middle.theta * 180 / M_PI);
    }
	char out[] = "road_hough.bmp";
    bmpWrite(src, out);
    printf("Saved road_hough.bmp\n");

	char ipm[] = "road_hough_ipm.bmp";
    char ipm2[] = "road_hough_ipm2.bmp";
    uHorizon_IPM(src, ipm, 0, 15 * 3.14 / 180, 4, -10, 5, 0, 0.025);
	uHorizon_IPM(src, ipm2, 20, 15 * 3.14 / 180, 4, -10, 5, 0, 0.025);
}
