#include "Header.h"

void uHorizon_IPM_OpenCV(const cv::Mat& img, const std::string& output_filename,
    double alpha = 15 * CV_PI / 180,
    double dx = 4, double dy = -10, double dz = 5,
    double r0 = 0, double theda = 0.025)
{
    if (img.empty()) {
        std::cerr << "Input image is empty!" << std::endl;
        return;
    }

    int m = img.rows; // u direction (height)
    int n = img.cols; // v direction (width)

    // 1. 求地平線 (與原程式邏輯相同)
    int uHorizon = (int)std::ceil((m - 1) / (2 * alpha) * (-theda + alpha));
    if (uHorizon < 0) uHorizon = 0;
    if (uHorizon >= m) uHorizon = m - 1;

    // 2. Forward Mapping: 計算世界座標邊界 (Bounding Box)
    // 這部分必須遍歷一次有效區域來決定輸出圖的大小
    double x_min = std::numeric_limits<double>::max();
    double x_max = -std::numeric_limits<double>::max();
    double y_min = std::numeric_limits<double>::max();
    double y_max = -std::numeric_limits<double>::max();

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

    if (x_min == std::numeric_limits<double>::max()) {
        std::cerr << "Failed to compute world bounds." << std::endl;
        return;
    }

    // 3. 設定解析度比例 (這兩個參數控制輸出圖的縮放)
    double r1 = 100.0;
    double r2 = 100.0;

    double x_range = x_max - x_min;
    double y_range = y_max - y_min;
    if (x_range <= 0) x_range = r1;
    if (y_range <= 0) y_range = r2;

    int new_height = (int)std::ceil(x_range / r1);
    int new_width = (int)std::ceil(y_range / r2);

    // 簡單的防呆限制，避免記憶體爆炸
    const int MAX_DIM = 10000;
    if (new_height > MAX_DIM) new_height = MAX_DIM;
    if (new_width > MAX_DIM) new_width = MAX_DIM;
    if (new_height < 1) new_height = 1;
    if (new_width < 1) new_width = 1;

    std::cout << "IPM Output Size: " << new_width << " x " << new_height << std::endl;

    // 4. 建立 Remap 用的映射表 (Map X 和 Map Y)
    // map_x 存的是每個像素對應回原圖的 col (v)
    // map_y 存的是每個像素對應回原圖的 row (u)
    cv::Mat map_x(new_height, new_width, CV_32FC1);
    cv::Mat map_y(new_height, new_width, CV_32FC1);

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            // 計算輸出圖像素 (i, j) 對應的世界座標
            double x = x_min + i * r1;
            double y = y_min + j * r2;

            double u, v;
            // 反算出原圖座標 (u, v)
            worldToImage(x, y, m, n, dx, dy, dz, r0, theda, alpha, u, v);

            // 填入 Map
            // 注意：OpenCV 的 remap 需要 float 型別
            // map_x 對應的是 col (也就是你的 v)
            // map_y 對應的是 row (也就是你的 u)
            map_x.at<float>(i, j) = (float)v;
            map_y.at<float>(i, j) = (float)u;
        }
    }

    // 5. 使用 cv::remap 進行逆向映射
    // 這是 OpenCV 取代手寫雙層迴圈填色的標準做法
    // INTER_LINEAR 會自動做雙線性插值，比原本的 round(u) 效果更平滑
    // BORDER_CONSTANT 代表超出邊界的地方填黑色
    cv::Mat output_img;
    cv::remap(img, output_img, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

    // 6. 存檔
    cv::imwrite(output_filename, output_img);
}

void ipm_cv() {
    std::string input_path = "road.bmp";
    std::string output_path = "IPM_opencv.bmp";

    cv::Mat img = cv::imread(input_path);
    if (img.empty()) {
        std::cerr << "Cannot read " << input_path << std::endl;
        return;
    }

    uHorizon_IPM_OpenCV(img, output_path, 15 * CV_PI / 180, 4, -10, 5, 0, 0.025);
}