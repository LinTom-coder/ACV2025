#include "Header.h"

// 進階題：多尺度 Template Matching (Scaled Template Matching)
void b() {
    cout << "\n=== Running Advanced Task (Multi Scale) ===" << endl;

    Mat img_scene = imread("multi_white.bmp", IMREAD_GRAYSCALE);
    Mat img_template_orig = imread("white.bmp", IMREAD_GRAYSCALE);
    Mat img_display = imread("multi_white.bmp");

    if (img_scene.empty() || img_template_orig.empty()) {
        cerr << "Error: Could not load advanced task images!" << endl;
        return;
    }

    double best_maxVal = -1;
    Point best_maxLoc;
    double best_scale = 1.0;

    // 暫存變數
    Mat resized_template;
    Mat result;
    double minVal, maxVal;
    Point minLoc, maxLoc;

    // --- 第一部分：尋找最佳 Scale ---
    // 這裡設定從 0.2 倍到 2.0 倍
    for (double scale = 0.2; scale <= 2.0; scale += 0.05) {
        resize(img_template_orig, resized_template, Size(), scale, scale);

        // 邊界檢查
        if (resized_template.rows > img_scene.rows || resized_template.cols > img_scene.cols) {
            continue;
        }

        matchTemplate(img_scene, resized_template, result, TM_CCOEFF_NORMED);
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal > best_maxVal) {
            best_maxVal = maxVal;
            best_maxLoc = maxLoc;
            best_scale = scale;
        }
    }

    cout << "Advanced Best Scale: " << best_scale << endl;
    cout << "Advanced Match Score: " << best_maxVal << endl;

    // --- 第二部分：使用最佳 Scale 找出所有目標 ---

    // [修正1] 必須使用 best_scale 重新產生正確大小的模板
    Mat final_template;
    resize(img_template_orig, final_template, Size(), best_scale, best_scale);

    // [優化] 只執行一次 matchTemplate
    matchTemplate(img_scene, final_template, result, TM_CCOEFF_NORMED);

    int num = 45; // 預計要找的數量
    double threshold = 0.7; // 建議提高一點閾值，避免雜訊

    for (int i = 0; i < num; i++) {
        // 在 result 矩陣中找最大值
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        // 如果分數低於門檻，表示找不到了，直接跳出
        if (maxVal < threshold) {
            break;
        }

        // [修正2] 語法錯誤修正：使用 final_template.cols
        //[cite_start]// 繪製 Bounding Box [cite: 4]
        rectangle(img_display, maxLoc, Point(maxLoc.x + final_template.cols, maxLoc.y + final_template.rows), Scalar(0, 0, 255), 2);

        // 標記數字 (Optional)
        putText(img_display, to_string(i + 1), Point(maxLoc.x, maxLoc.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);

        // [修正3] 遮罩策略：把 result 矩陣中已經找到的位置塗黑 (-1)
        // 這樣下一次 minMaxLoc 就不會再抓到這裡
        // 遮罩大小建議設為模板的一半左右，避免蓋到隔壁太近的拼圖
        int margin_x = final_template.cols / 2;
        int margin_y = final_template.rows / 2;

        int start_x = max(0, maxLoc.x - margin_x);
        int start_y = max(0, maxLoc.y - margin_y);
        int end_x = min(result.cols, maxLoc.x + margin_x);
        int end_y = min(result.rows, maxLoc.y + margin_y);

        rectangle(result, Point(start_x, start_y), Point(end_x, end_y), Scalar(-1), -1);
    }

    imshow("Advanced Task Result", img_display);
    imwrite("result_advanced.png", img_display);
    waitKey(0);
    destroyAllWindows();
}