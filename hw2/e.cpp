#include "Header.h"

void e() {
	char input_img[] = "Switzerland_square.bmp";
	char bonus_binary_road[] = "binary_of_road_bonus.bmp";

	// RGB to Binary
	//gray = 0.299*R + 0.587*G + 0.114*B
	BmpInfo img = bmpRead(input_img);
	BmpInfo binary_img = rgb2binary(img, 220, 0.299, 0.587, 0.144);
	bmpWrite(binary_img, bonus_binary_road);

	// reserve the road
	char bonus_reserved_road[] = "reserved_road_bonus.bmp";
	BmpInfo reserved_img = binary_closing(binary_img, 9);
	bmpWrite(reserved_img, bonus_reserved_road);

	// connected components
	char bonus_cc[] = "cc_bonus.bmp";
	BmpInfo bbox_img = connected_components(reserved_img, img);
	bmpWrite(bbox_img, bonus_cc);

	// caculate property
	AxisInfo results = length_and_longaxis(reserved_img);
	double cx = results.cx, cy = results.cy, theta = results.theta, length = results.length;
	printf("Orientation: %.2f degrees\n", theta);
	printf("Length: %.2f pixels\n", length);

	// 釋放動態記憶體
	delete[] img.imgDataDynamic;
	delete[] binary_img.imgDataDynamic;
	delete[] reserved_img.imgDataDynamic;
	delete[] bbox_img.imgDataDynamic;
}