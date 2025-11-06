#include "Header.h"

void d() {
	auto t0 = chrono::high_resolution_clock::now();

	char  input_img[] = "reserved_road.bmp";
	BmpInfo img = bmpRead(input_img);
	AxisInfo results = length_and_longaxis(img);
	double cx = results.cx, cy = results.cy, theta = results.theta, length = results.length;
	printf("Centroid: (%.2f, %.2f)\n", cx, cy);
	printf("Orientation: %.2f degrees\n", theta);
	printf("Length: %.2f pixels\n", length);
	delete[] img.imgDataDynamic;

	// time measurement
	auto t1 = chrono::high_resolution_clock::now();
	double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
	printf("d: elapsed %.2f ms\n", ms);
}