# HW3: Lane Detection & Image Stitching

## 📝 Overview
This project implements **Lane Detection** (from scratch in Pure C++) and **Image Stitching** (using OpenCV) for the Advanced Computer Vision course.

## 🚀 Key Features

### 1. Lane Detection (Pure C++ / No OpenCV)
* **Algorithms from Scratch:** Manually implemented **Sobel Edge Detection** and **Hough Transform** without external vision libraries.
* **Geometric Processing:** Included **Inverse Perspective Mapping (IPM)** for Bird's Eye View transformation.
* **Robust Rendering:** Implemented **Boundary Clipping** to correctly draw lines extending beyond image edges and **Angle Filtering** to distinguish left/right lanes.
* **Custom I/O:** Manual **24-bit BMP** file handling.

### 2. Image Stitching
* Utilizes **OpenCV** to compute **Homography** matrices and stitch overlapping images into a panorama.

## 🛠️ Environment
* **Language:** C/C++ (Visual Studio 2022)
* **Dependencies:**
    * **Lane Detection:** None (Standard Library only: `<vector>`, `<cmath>`).
    * **Stitching:** OpenCV 4.x.

## ⚠️ Limitations
* Hough Transform uses fixed thresholds; adaptive thresholding or polynomial fitting could improve performance on curved roads.
