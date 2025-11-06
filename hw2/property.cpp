#include "Header.h"

// only support binary img
// caculate length and orientation of long axis of white object
AxisInfo length_and_longaxis(BmpInfo img){
    int width = img.bi.biWidth;
    int height = img.bi.biHeight;
    int channels = 3; // B, G, R
	
    int x, y, N = 0;
    double cx = 0, cy = 0;

	// centroid
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
			int idx = (y * width + x) * channels;
            if (img.imgDataDynamic[idx] > 0) {
                cx += x;
                cy += y;
                N++;
            }
        }
    }
    cx /= N;
    cy /= N;

	// covariance matrix
    double varx = 0, covar = 0, vary = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            if (img.imgDataDynamic[idx] > 0) {
                varx += (x - cx) * (x - cx);
                covar += (x - cx) * (y - cy);
                vary += (y - cy) * (y - cy);
            }
        }
    }
    varx /= N;
    covar /= N;
	vary /= N;

    //eigenvalues
    double tmp = sqrt((varx - vary) * (varx - vary) + 4 * covar * covar);
    double lambda1 = 0.5 * ((varx + vary) + tmp);
    double lambda2 = 0.5 * ((varx + vary) - tmp);

    // orientation
    double vx, vy;
    if (fabs(covar) > 1e-6)
        vx = lambda1 - vary, vy = covar;
    else
        vx = 1, vy = 0;

    double norm = sqrt(vx * vx + vy * vy);
    vx /= norm; vy /= norm;
    double theta = atan2(vy, vx) * 180.0 / 3.14159;

    //length
    double min_proj = 1e9, max_proj = -1e9;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            if (img.imgDataDynamic[idx] > 0) {
                double proj = (x - cx) * vx + (y - cy) * vy;
                if (proj < min_proj) min_proj = proj;
                if (proj > max_proj) max_proj = proj;
            }
        }
    }
    double length = max_proj - min_proj;

    AxisInfo result = { cx, cy, theta, length };
	return result;
}