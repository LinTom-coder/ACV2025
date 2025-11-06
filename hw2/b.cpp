#include "Header.h"

void b() {
	auto t0 = chrono::high_resolution_clock::now();

	char  input_img[] = "binary_of_road.bmp";
	char output_img[] = "reserved_road.bmp";

	BmpInfo img = bmpRead(input_img);
	BmpInfo reserved_img = binary_closing(img, 9);

	bmpWrite(reserved_img, output_img);
	delete[] img.imgDataDynamic;

	// time measurement
	auto t1 = chrono::high_resolution_clock::now();
	double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
	printf("b: elapsed %.2f ms\n", ms);
}