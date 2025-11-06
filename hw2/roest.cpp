#include "Header.h"

double rotation_estimation(BmpInfo img, BmpInfo rawimg) {
	BmpInfo binary_img = rgb2binary(rawimg, 100, 0.667, 0.0, 0.333);
	AxisInfo results = length_and_longaxis(binary_img);
	double theta = results.theta;

	BmpInfo rotated_binary_img = rgb2binary(img, 100, 0.667, 0.0, 0.333);
	AxisInfo rotated_results = length_and_longaxis(rotated_binary_img);
	double rotated_theta = rotated_results.theta;
	return theta - rotated_theta;
}