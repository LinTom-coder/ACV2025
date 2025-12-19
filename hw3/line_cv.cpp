#include "Header.h"

void RunHoughLaneDetection_OpenCV()
{
    std::string input_path = "road.bmp";
    cv::Mat src = cv::imread(input_path);
    if (src.empty()) {
        std::cerr << "Cannot read " << input_path << std::endl;
        return;
    }

    // 1. 轉灰階
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // 2. 高斯模糊 (取代 simpleBlur)
    // 3x3 kernel, sigmaX=0 (自動計算)
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);

    // 3. 邊緣偵測 (取代 sobelEdge)
    // Canny 演算法通常比單純 Sobel 更適合直線偵測
    // 閾值 50, 150 是一組常見的經驗值
    cv::Mat edges;
    cv::Canny(blurred, edges, 50, 150);

    // 儲存 Edge 結果 (除錯用)
    cv::imwrite("debug_edges_opencv.bmp", edges);

    // 4. 霍夫變換 (標準 HoughLines)
    // 參數: edges, rho解析度(1), theta解析度(1度), 閾值(100)
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

    std::cout << "Hough found " << lines.size() << " lines." << std::endl;

    // 5. 過濾與繪製
    // OpenCV 的 HoughLines 預設會根據投票數(votes)由高到低排序回傳
    // 所以我們只要找到符合角度條件的「第一條」線，通常就是那一類中最好的線

    bool foundLeft = false;
    bool foundRight = false;
    bool foundMiddle = false;

    // 複製原圖用來畫線
    cv::Mat resultImg = src.clone();

    // [新增] 計算地平線 uHorizon (與 IPM 參數保持一致)
    double alpha = 15 * CV_PI / 180;
    double theda = 0.025;
    int m = src.rows;
    int uHorizon = (int)std::ceil((m - 1) / (2 * alpha) * (-theda + alpha));

    // 確保 uHorizon 在合理範圍
    if (uHorizon < 0) uHorizon = 0;
    if (uHorizon >= m) uHorizon = m - 1;

    // 定義「路面區域」：從地平線到圖片底部
    // cv::clipLine 會將線條限制在這個矩形內
    cv::Rect roadROI(0, uHorizon, src.cols, src.rows - uHorizon);

    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1]; // 弧度 [0, PI]

        double degree = theta * 180.0 / CV_PI;

        // 判斷要畫哪種顏色的線 (BGR格式)
        cv::Scalar color = cv::Scalar(0, 0, 0); // 預設黑
        bool draw = false;
        std::string label = "";

        // 邏輯: 如果該方向還沒找到過最好的線，且角度符合，就選它
        if (!foundLeft && degree > 40 && degree < 80) {
            color = cv::Scalar(255, 0, 0); // Blue
            foundLeft = true;
            draw = true;
            label = "Left";
            std::cout << "Left Lane: rho=" << rho << ", theta=" << degree << std::endl;
        }
        else if (!foundMiddle && degree > 20 && degree < 40) {
            color = cv::Scalar(0, 0, 255); // Red
            foundMiddle = true;
            draw = true;
            label = "Middle";
            std::cout << "Middle Lane: rho=" << rho << ", theta=" << degree << std::endl;
        }
        else if (!foundRight && degree > 135 && degree < 175) {
            color = cv::Scalar(0, 255, 0); // Green
            foundRight = true;
            draw = true;
            label = "Right";
            std::cout << "Right Lane: rho=" << rho << ", theta=" << degree << std::endl;
        }

        if (draw) {
            // 將 (rho, theta) 轉換成兩個點 (x0, y0) 以畫線
            double a = std::cos(theta);
            double b = std::sin(theta);
            double x0 = a * rho;
            double y0 = b * rho;

            // 延伸線段 (乘以 1000 確保貫穿畫面)
            cv::Point pt1, pt2;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));

            if (cv::clipLine(roadROI, pt1, pt2)) {
                cv::line(resultImg, pt1, pt2, color, 3, cv::LINE_AA);
            }
        }

        // 如果三條都找到了，就可以提早結束迴圈 (因為後面的線 votes 比較低)
        if (foundLeft && foundRight && foundMiddle) break;
    }

    cv::imwrite("road_hough_opencv.bmp", resultImg);
    std::cout << "Saved road_hough_opencv.bmp" << std::endl;

    // 6. IPM 轉換 (使用畫好線的圖，還是原圖？依照你原本邏輯是用 src 原圖)
    uHorizon_IPM_OpenCV(src, "road_hough_ipm_opencv.bmp", 15 * CV_PI / 180, 4, -10, 5, 0, 0.025);
}