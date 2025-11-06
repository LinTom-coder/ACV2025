#include "Header.h"

void c() {
	char mata_img[] = "Ian_island_square.bmp";
	char input_img[] = "reserved_road.bmp";
	char output_img[] = "cc.bmp";

	BmpInfo img = bmpRead(input_img);
	BmpInfo mataimg = bmpRead(mata_img);
	BmpInfo bbox_img = connected_components(img, mataimg);

	bmpWrite(bbox_img, output_img);
	delete[] img.imgDataDynamic;
}