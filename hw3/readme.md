# Advanced Computer Vision - HW3: Lane Detection & Image Stitching

## 📝 Introduction
This project is part of the **Advanced Computer Vision** course at NTUT. The main objective is to implement classic computer vision algorithms for autonomous driving scenarios (lane detection) and panoramic imaging (stitching).

**Key Highlight:** The **Lane Detection** module is implemented **entirely in pure C++** (Standard Library only), building core algorithms like Sobel Edge Detection and Hough Transform from scratch without relying on OpenCV.

## 🚀 Features

### 1. Inverse Perspective Mapping (IPM)
- **Bird's Eye View:** Implemented geometric transformations to convert the driver's front-view image into a top-down view.
- **Manual Mapping:** Calculated the `Image-to-World` and `World-to-Image` coordinate transformations using camera parameters (height, pitch, focal length).
- **Distortion Correction:** Effectively removes perspective effects to facilitate accurate road analysis.

### 2. Road Lane Detection (Pure C++ Implementation)
Instead of using libraries, the following algorithms were manually implemented:
- **Custom Image I/O:** Implemented a raw byte reader/writer for **24-bit BMP** files (`readBMP`/`writeBMP`).
- **Edge Detection:**
  - Implemented **Grayscale conversion** and **Gaussian Blur** for noise reduction.
  - Built the **Sobel Operator** from scratch to calculate gradient magnitude and extract edges.
- **Line Detection (Hough Transform):**
  - Created a **Hough Voting Space (Accumulator)** to identify straight lines in the edge map.
  - Implemented a voting mechanism in the $(\rho, \theta)$ parameter space.
- **Robust Visualization:**
  - **Lane Filtering:** Applied geometric constraints (angle filtering) to distinguish between Left lanes (positive slope) and Right lanes (negative slope).
  - **Boundary Clipping:** Implemented a geometric intersection algorithm to correctly draw detected lines across the entire image frame, solving the issue where lines would be cut off at the bottom or sides.

### 3. Image Stitching
- Implemented image stitching to combine multiple overlapping images into a single panorama.
- Computed **Homography** matrices to align images based on feature points.

## 🛠️ Environment & Dependencies
- **IDE:** Visual Studio 2022
- **Language:** C/C++
- **Libraries:**
  - **Lane Detection:** **None** (Pure C++ Standard Library: `<vector>`, `<cmath>`, `<cstdio>`).
  - **Image Stitching:** OpenCV (4.x recommended).

## ⚠️ Limitations & Future Work
- **Robustness:** The manual Hough Transform relies on fixed voting thresholds, which may require tuning for different lighting conditions (e.g., shadows or night scenes).
- **Curved Lanes:** The current implementation detects straight lines. Future improvements could involve **Polynomial Fitting** or **Sliding Window Search** on the IPM image to accurately model curved roads.
