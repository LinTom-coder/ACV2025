#include "Header.h"

void a() {
	char  input_img[] = "road.bmp";
	char output[] = "IPM.bmp";
	char output2[] = "IPM2.bmp";
	BmpInfo img;
	if (!bmpRead(input_img, img)) {
		printf("Cannot read road.bmp\n");
		return;
	}
	uHorizon_IPM(img, output,0, 15 * 3.14 / 180, 4, -10, 5, 0, 0.025);
	uHorizon_IPM(img, output2, 20, 15 * 3.14 / 180, 4, -10, 5, 0, 0.025);
}