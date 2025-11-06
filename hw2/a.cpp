#include "Header.h"

void a() {
	auto t0 = chrono::high_resolution_clock::now();
	
	// RGB to Binary
	char  input_img[] = "Ian_island_square.bmp";
	char output_img[] = "binary_of_road.bmp";
	BmpInfo img = bmpRead(input_img);
	BmpInfo binary_img = rgb2binary(img, 100, 0.667, 0.0, 0.333);
	bmpWrite(binary_img, output_img);
	delete[] img.imgDataDynamic;

	// time measurement
	auto t1 = chrono::high_resolution_clock::now();
	double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
	printf("a: elapsed %.2f ms\n", ms);
}