//#include "Header.h"
//
//void d() {
//	char  input_img[] = "InputImage1.bmp";
//	char output_img_n[] = "bonus1_double.bmp";
//	char output_img_r[] = "bonus1_rotation.bmp";
//    char output_img_grb[] = "bonus1_grb.bmp";
//
//	BmpInfo img_b1 = bmpRead(input_img);
//    BmpInfo img_b1_r = bmpRead(input_img);
//    BmpInfo img_b1_grb = bmpRead(input_img);
//
//	//nearest
//	BmpInfo img_b1_nearest = nearest(img_b1);
//	bmpWrite(img_b1_nearest, output_img_n);
//
//	//rotation
//    int width = img_b1_nearest.bi.biWidth;
//    int height = img_b1_nearest.bi.biHeight;
//    int size = width * height * 3;
//
//    unsigned char* rotated = new unsigned char[size];
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            int srcIdx = (y * width + x) * 3;
//            int newX = height - 1 - y;
//            int newY = x;
//            int dstIdx = (newY * width + newX) * 3;
//
//            rotated[dstIdx] = img_b1_nearest.imgDataDynamic[srcIdx];       // B
//            rotated[dstIdx + 1] = img_b1_nearest.imgDataDynamic[srcIdx + 1]; // G
//            rotated[dstIdx + 2] = img_b1_nearest.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//    img_b1_r.imgDataDynamic = rotated;
//    img_b1_r.bi.biWidth = height;
//    img_b1_r.bi.biHeight = width;
//
//    bmpWrite(img_b1_r, output_img_r);
//	delete[] img_b1_r.imgDataDynamic;
//
//    //rgb
//    int size_grb = width * height;
//
//    // BGR → GRB
//    for (int i = 0; i < size_grb; i++) {
//        int idx = i * 3; // 每個像素 3 bytes
//        unsigned char temp = img_b1_nearest.imgDataDynamic[idx];       // B
//        img_b1_nearest.imgDataDynamic[idx] = img_b1_nearest.imgDataDynamic[idx + 1];   // G → B
//        img_b1_nearest.imgDataDynamic[idx + 1] = img_b1_nearest.imgDataDynamic[idx + 2]; // R → G
//        img_b1_nearest.imgDataDynamic[idx + 2] = temp;                // B → R
//    }
//
//    bmpWrite(img_b1_nearest, output_img_grb);
//    delete[] img_b1_nearest.imgDataDynamic;
//	delete[] img_b1_grb.imgDataDynamic;
//
//	//shrink
//    char output_img_s[] = "bonus1_shrink.bmp";
//    char output_imgs_r[] = "bonus1s_rotation.bmp";
//    char output_imgs_grb[] = "bonus1s_grb.bmp";
//
//    BmpInfo img_b1s = bmpRead(input_img);
//    BmpInfo img_b1s_r = bmpRead(input_img);
//	BmpInfo img_b1s_grb = bmpRead(input_img);
//	
//	BmpInfo img_b1s_shrink = shrink(img_b1s);
//	bmpWrite(img_b1s_shrink, output_img_s);
//
//	//rotation
//    int width_s = img_b1s_shrink.bi.biWidth;
//    int height_s = img_b1s_shrink.bi.biHeight;
//    int size_s = width_s * height_s * 3;
//    unsigned char* rotated_s = new unsigned char[size_s];
//    for (int y = 0; y < height_s; y++) {
//        for (int x = 0; x < width_s; x++) {
//            int srcIdx = (y * width_s + x) * 3;
//            int newX = height_s - 1 - y;
//            int newY = x;
//            int dstIdx = (newY * width_s + newX) * 3;
//            rotated_s[dstIdx] = img_b1s_shrink.imgDataDynamic[srcIdx];       // B
//            rotated_s[dstIdx + 1] = img_b1s_shrink.imgDataDynamic[srcIdx + 1]; // G
//            rotated_s[dstIdx + 2] = img_b1s_shrink.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//    img_b1s_r.imgDataDynamic = rotated_s;
//    img_b1s_r.bi.biWidth = height_s;
//    img_b1s_r.bi.biHeight = width_s;
//    bmpWrite(img_b1s_r, output_imgs_r);
//	delete[] img_b1s_r.imgDataDynamic;
//
//	//rgb
//    int size_grb_s = width_s * height_s;
//    for (int i = 0; i < size_grb_s; i++) {
//        int idx = i * 3; // 每個像素 3 bytes
//        unsigned char temp = img_b1s_shrink.imgDataDynamic[idx];       // B
//        img_b1s_shrink.imgDataDynamic[idx] = img_b1s_shrink.imgDataDynamic[idx + 1];   // G → B
//        img_b1s_shrink.imgDataDynamic[idx + 1] = img_b1s_shrink.imgDataDynamic[idx + 2]; // R → G
//        img_b1s_shrink.imgDataDynamic[idx + 2] = temp;                // B → R
//    }
//	bmpWrite(img_b1s_shrink, output_imgs_grb);
//    delete[] img_b1s_shrink.imgDataDynamic;
//
//    //bonus2
//    char output_img_b2[] = "bonus2_4096.bmp";
//    char output_img_b2_r[] = "bonus2_rotation.bmp";
//    char output_img_b2_grb[] = "bonus2_grb.bmp";
//
//    BmpInfo img_b2 = bmpRead(input_img);
//    BmpInfo img_b2_r = bmpRead(input_img);
//    BmpInfo img_b2_grb = bmpRead(input_img);
//
//    //nearest
//    BmpInfo img_b2_nearest = nearest(img_b2);
//    img_b2_nearest = nearest(img_b2_nearest);
//    img_b2_nearest = nearest(img_b2_nearest);
//    bmpWrite(img_b2_nearest, output_img_b2);
//
//    //rotation
//    int width2 = img_b2_nearest.bi.biWidth;
//    int height2 = img_b2_nearest.bi.biHeight;
//    int size2 = width2 * height2 * 3;
//
//    unsigned char* rotated2 = new unsigned char[size2];
//    for (int y = 0; y < height2; y++) {
//        for (int x = 0; x < width2; x++) {
//            int srcIdx = (y * width2 + x) * 3;
//            int newX = height2 - 1 - y;
//            int newY = x;
//            int dstIdx = (newY * width2 + newX) * 3;
//
//            rotated2[dstIdx] = img_b2_nearest.imgDataDynamic[srcIdx];       // B
//            rotated2[dstIdx + 1] = img_b2_nearest.imgDataDynamic[srcIdx + 1]; // G
//            rotated2[dstIdx + 2] = img_b2_nearest.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//    img_b2_r.imgDataDynamic = rotated2;
//    img_b2_r.bi.biWidth = height2;
//    img_b2_r.bi.biHeight = width2;
//
//    bmpWrite(img_b2_r, output_img_b2_r);
//    delete[] img_b2_r.imgDataDynamic;
//
//    //rgb
//    int size2_grb = width2 * height2;
//
//    // BGR → GRB
//    for (int i = 0; i < size2_grb; i++) {
//        int idx = i * 3; // 每個像素 3 bytes
//        unsigned char temp = img_b2_nearest.imgDataDynamic[idx];       // B
//        img_b2_nearest.imgDataDynamic[idx] = img_b2_nearest.imgDataDynamic[idx + 1];   // G → B
//        img_b2_nearest.imgDataDynamic[idx + 1] = img_b2_nearest.imgDataDynamic[idx + 2]; // R → G
//        img_b2_nearest.imgDataDynamic[idx + 2] = temp;                // B → R
//    }
//
//    bmpWrite(img_b2_nearest, output_img_b2_grb);
//    delete[] img_b2_nearest.imgDataDynamic;
//    delete[] img_b2_grb.imgDataDynamic;
//
//}
//
//BmpInfo shrink(BmpInfo imgInfoN) {
//    int width = imgInfoN.bi.biWidth;
//    int height = imgInfoN.bi.biHeight;
//    int newWidth = width /2;
//    int newHeight = height / 2;
//
//    // 建立新的動態陣列
//    unsigned char* newImgData = new unsigned char[newWidth * newHeight * 3];
//
//    for (int y = 0; y < newHeight; y++) {
//        for (int x = 0; x < newWidth; x++) {
//            // 對應原圖座標
//            int srcX = x * 2;
//            int srcY = y * 2;
//
//            int srcIdx = (srcY * width + srcX) * 3;
//            int dstIdx = (y * newWidth + x) * 3;
//
//            newImgData[dstIdx] = imgInfoN.imgDataDynamic[srcIdx];       // B
//            newImgData[dstIdx + 1] = imgInfoN.imgDataDynamic[srcIdx + 1]; // G
//            newImgData[dstIdx + 2] = imgInfoN.imgDataDynamic[srcIdx + 2]; // R
//        }
//    }
//
//    // 釋放原始影像記憶體
//    delete[] imgInfoN.imgDataDynamic;
//
//    // 更新影像資訊
//    imgInfoN.imgDataDynamic = newImgData;
//    imgInfoN.bi.biWidth = newWidth;
//    imgInfoN.bi.biHeight = newHeight;
//
//    return imgInfoN;
//}
