#include "Header.h"

// 基礎題：標準 Template Matching
void a() {
    cout << "=== Running Basic Task (White Hell) ===" << endl;

    // 1. 讀取影像 (建議轉灰階以加快運算並減少色彩干擾)
    // 檔名參照 
    Mat img_scene = imread("white_hell.bmp", IMREAD_GRAYSCALE);
    Mat img_template = imread("white_puzzle.bmp", IMREAD_GRAYSCALE);
    Mat img_display = imread("white_hell.bmp"); // 用於畫圖的彩色原圖

    if (img_scene.empty() || img_template.empty()) {
        cerr << "Error: Could not load basic task images!" << endl;
        return;
    }

    // 2. 建立結果矩陣
    Mat result;
    int num = 45;
	double threshold = 0.7;

    for (int i = 0; i <= num; i++) {
        // 3. 執行 Template Matching
        // TM_CCOEFF_NORMED 是最常用的方法，結果介於 -1 到 1 之間，1 為最相似
        matchTemplate(img_scene, img_template, result, TM_CCOEFF_NORMED);

        // 4. 尋找最佳匹配位置
        double minVal, maxVal;
        Point minLoc, maxLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        /*cout << "Basic Match Score: " << maxVal << endl;
        cout << "Location: " << maxLoc << endl;*/
        if (maxVal >= threshold) {
            // 5. 繪製 Bounding Box [cite: 4]
            rectangle(img_display, maxLoc, Point(maxLoc.x + img_template.cols, maxLoc.y + img_template.rows), Scalar(0, 0, 255), 3);
            // 將已找到的區域塗黑以避免重複偵測
            rectangle(img_scene, maxLoc, Point(maxLoc.x + img_template.cols, maxLoc.y + img_template.rows), Scalar(0), -1);
		}
        
    }

    // 6. 顯示與儲存結果
    imshow("Basic Task Result", img_display);
    imwrite("result_basic.png", img_display);
    waitKey(0);
    destroyAllWindows();
}