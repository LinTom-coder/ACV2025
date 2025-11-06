#include "Header.h"

void f() {
	auto t0 = chrono::high_resolution_clock::now();
	char  input_img[] = "Ian_island_square.bmp";
	BmpInfo img = bmpRead(input_img);

	// rotation 90度
	char bonus_rotation[] = "rotation_bonus.bmp";
	BmpInfo rotated_img = bmpRead(input_img);
	int width = rotated_img.bi.biWidth;
	int height = rotated_img.bi.biHeight;
	int size = width * height * 3;
	unsigned char* rotated = new unsigned char[size];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int srcIdx = (y * width + x) * 3;
			int newX = height - 1 - y;
			int newY = x;
			int dstIdx = (newY * height + newX) * 3;

			rotated[dstIdx] = rotated_img.imgDataDynamic[srcIdx];       // B
			rotated[dstIdx + 1] = rotated_img.imgDataDynamic[srcIdx + 1]; // G
			rotated[dstIdx + 2] = rotated_img.imgDataDynamic[srcIdx + 2]; // R
		}
	}
	rotated_img.imgDataDynamic = rotated;
	rotated_img.bi.biWidth = height;
	rotated_img.bi.biHeight = width;
	bmpWrite(rotated_img, bonus_rotation);

	double angle = rotation_estimation(rotated_img, img);
	printf("Estimated rotation angle: %.2f degrees\n", angle);

	// 釋放動態記憶體
	delete[] img.imgDataDynamic;

	auto t1 = chrono::high_resolution_clock::now();
	double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
	printf("f: elapsed %.2f ms\n", ms);
}