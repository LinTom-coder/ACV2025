#include "Header.h"

void runImageStitchingOpenCV() {
    // 1. 讀取圖片 (Left & Right)
    // 確保圖片在專案目錄下
    cv::Mat left = cv::imread("left.bmp");
    cv::Mat right = cv::imread("right.bmp");

    if (left.empty() || right.empty()) {
        std::cerr << "Error: Cannot load left.bmp or right.bmp" << std::endl;
        return;
    }

    std::cout << "Left size: " << left.size() << std::endl;
    std::cout << "Right size: " << right.size() << std::endl;

    // 2. 建立 Canvas (900x480)
    // CV_8UC3 代表 8-bit unsigned char, 3 channels (BGR)
    int canvas_w = 900;
    int canvas_h = 480;
    cv::Mat canvas = cv::Mat::zeros(canvas_h, canvas_w, CV_8UC3);

    // 3. 貼上左圖 (Paste Left Image)
    // 作業要求位置: Row [80:449], Col [0:509]
    // 也就是左上角起點為 (x=0, y=80)
    int left_x_offset = 0;
    int left_y_offset = 80;

    // 定義感興趣區域 (ROI: Region of Interest)
    // cv::Rect(x, y, width, height)
    cv::Rect roi(left_x_offset, left_y_offset, left.cols, left.rows);

    // 將左圖複製到 Canvas 的 ROI 區域
    // 這裡做個簡單的防呆，避免貼出邊界
    if (roi.x + roi.width <= canvas_w && roi.y + roi.height <= canvas_h) {
        left.copyTo(canvas(roi));
    }
    else {
        std::cerr << "Warning: Left image size exceeds canvas bounds!" << std::endl;
    }

    // 4. 定義對應點 (Source & Destination Points)
    // 重要：OpenCV 的座標系統是 (x, y) = (Column, Row)
    // 而你原本作業的座標輸入是 (Row, Column)

    // --- 來源點 (Right Image 4 Corners) ---
    // 順序: Top-Left, Bottom-Left, Bottom-Right, Top-Right
    float w = (float)right.cols;
    float h = (float)right.rows;

    std::vector<cv::Point2f> src_pts;
    src_pts.push_back(cv::Point2f(0, 0));       // TL
    src_pts.push_back(cv::Point2f(0, h));       // BL
    src_pts.push_back(cv::Point2f(w, h));       // BR
    src_pts.push_back(cv::Point2f(w, 0));       // TR

    // --- 目標點 (Canvas Coordinates) ---
    // 根據作業投影片數值 (注意: 題目給的是 Row, Col，這裡要轉成 x, y)
    // (130, 250) -> x=250, y=130
    // (470, 310) -> x=310, y=470
    // (475, 900) -> x=900, y=475 (注意: 900其實在邊界外，但 OpenCV 支援浮點數座標)
    // (0, 770)   -> x=770, y=0

    std::vector<cv::Point2f> dst_pts;
    dst_pts.push_back(cv::Point2f(250, 130));   // TL
    dst_pts.push_back(cv::Point2f(310, 470));   // BL
    dst_pts.push_back(cv::Point2f(900, 475));   // BR
    dst_pts.push_back(cv::Point2f(770, 0));     // TR

    // 5. 計算透視變換矩陣 (Homography Matrix)
    // 這行取代了你原本手寫的 calculate_M + solve_system_8x8
    cv::Mat M = cv::getPerspectiveTransform(src_pts, dst_pts);

    // 6. 影像變形 (Warping)
    // 這行取代了 invert3x3 + stitch_image (inverse warping + bilinear interpolation)
    cv::Mat warped_right;
    cv::warpPerspective(right, warped_right, M, cv::Size(canvas_w, canvas_h));

    // 7. 將變形後的右圖「覆蓋」到 Canvas 上
    // 策略：找出 warped_right 中非黑色的區域，然後複製到 canvas
    // 這樣可以達成 "Right image pasted in front" 的效果

    cv::Mat gray, mask;
    // 轉灰階用來製作遮罩
    cv::cvtColor(warped_right, gray, cv::COLOR_BGR2GRAY);
    // 製作遮罩：只要像素值 > 0 (非黑) 就設為 255
    cv::threshold(gray, mask, 0, 255, cv::THRESH_BINARY);

    // 使用遮罩複製：只複製 mask 中為白色的部分
    warped_right.copyTo(canvas, mask);

    // 8. 存檔
    std::string output_filename = "result_opencv_stitch.bmp";
    cv::imwrite(output_filename, canvas);

    std::cout << "Processing done." << std::endl;
    std::cout << "Saved result to: " << output_filename << std::endl;

    // 顯示結果 (可選)
    // cv::imshow("Result", canvas);
    // cv::waitKey(0);
}