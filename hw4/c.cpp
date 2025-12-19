#include "Header.h"

using namespace cv;
using namespace std;

void c() {
    cout << "\n=== Running Bonus Task (Feature Matching) ===" << endl;

    // 1. 讀取影像
    Mat img_object = imread("Boh.bmp", IMREAD_GRAYSCALE);      // 模板 (Query)
    Mat img_scene = imread("SpiritedAway.bmp", IMREAD_GRAYSCALE); // 場景 (Train)
    Mat img_display = imread("SpiritedAway.bmp");             // 用於顯示彩色結果

    if (img_object.empty() || img_scene.empty()) {
        cerr << "Error: Could not load bonus task images!" << endl;
        return;
    }

    // 2. 初始化 SIFT 偵測器
    // 雖然 ORB 比較快，但 SIFT 在這種模糊與旋轉場景下效果通常比較穩
    Ptr<SIFT> detector = SIFT::create();

    vector<KeyPoint> keypoints_obj, keypoints_scene;
    Mat descriptors_obj, descriptors_scene;

    // 3. 偵測特徵點與計算描述子
    detector->detectAndCompute(img_object, noArray(), keypoints_obj, descriptors_obj);
    detector->detectAndCompute(img_scene, noArray(), keypoints_scene, descriptors_scene);

    // 4. 特徵匹配 (使用 KNN)
    // 使用 FLANN 或 BFMatcher 都可以，這裡用 BFMatcher (Brute Force) 簡單暴力
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knn_matches;
    matcher.knnMatch(descriptors_obj, descriptors_scene, knn_matches, 2);

    // 5. 篩選匹配點 (Lowe's Ratio Test)
    // 只有當最近鄰距離明顯小於次近鄰距離時 (ratio < 0.75)，才視為好特徵
    const float ratio_thresh = 0.75f;
    vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    cout << "Good matches found: " << good_matches.size() << endl;

    // 如果匹配點太少，就不畫框了 (通常至少要 4 點才能算 Homography，保險起見設 10)
    if (good_matches.size() >= 10) {
        // 6. 準備數據以計算 Homography
        vector<Point2f> obj;
        vector<Point2f> scene;

        for (size_t i = 0; i < good_matches.size(); i++) {
            // Get the keypoints from the good matches
            obj.push_back(keypoints_obj[good_matches[i].queryIdx].pt);
            scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
        }

        // 7. 計算單應性矩陣 (Homography) 並使用 RANSAC 剔除誤匹配
        Mat H = findHomography(obj, scene, RANSAC);

        if (!H.empty()) {
            // 8. 投影邊界框
            // 取得模板圖的四個角點
            vector<Point2f> obj_corners(4);
            obj_corners[0] = Point2f(0, 0);
            obj_corners[1] = Point2f((float)img_object.cols, 0);
            obj_corners[2] = Point2f((float)img_object.cols, (float)img_object.rows);
            obj_corners[3] = Point2f(0, (float)img_object.rows);

            vector<Point2f> scene_corners(4);

            // 將角點透過 H 矩陣投影到場景圖上
            perspectiveTransform(obj_corners, scene_corners, H);

            // 9. 畫出邊界線 (連接四個投影後的點)
            // 這裡使用 line 畫四條邊
            for (int i = 0; i < 4; i++) {
                line(img_display, scene_corners[i], scene_corners[(i + 1) % 4], Scalar(0, 255, 0), 4);
            }
        }
    }
    else {
        cout << "Not enough matches found!" << endl;
    }

    // 10. 顯示結果
    // 也可以畫出 matches 連線圖 (Optional)
    Mat img_matches;
    drawMatches(img_object, keypoints_obj, img_scene, keypoints_scene, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    imshow("Bonus Task: Matches", img_matches);
    imshow("Bonus Task: Detection", img_display);

    // 儲存結果
    imwrite("result_bonus.png", img_display);
    imwrite("result_bonus_match_lines.png", img_matches);

    waitKey(0);
    destroyAllWindows();
}