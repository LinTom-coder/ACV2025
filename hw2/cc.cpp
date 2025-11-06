#include "Header.h"

BmpInfo connected_components(const BmpInfo& img, BmpInfo rawimg) {
    auto t0 = chrono::high_resolution_clock::now();

    int width = img.bi.biWidth;
    int height = img.bi.biHeight;
    int channels = 3;
    int size = width * height;

    unsigned char* data = img.imgDataDynamic;
    unsigned char* rawdata = rawimg.imgDataDynamic;
    int* labelMap = new int[size];
    for (int i = 0; i < size; ++i) labelMap[i] = 0;

    int label = 0;
    int dx[4] = { 1, -1, 0, 0 };
    int dy[4] = { 0, 0, 1, -1 };
    std::vector<RegionInfo> regions;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            if (data[idx * 3] == 0 && labelMap[idx] == 0) {
                label++;
                std::stack<std::pair<int, int>> s;
                s.push(std::make_pair(x, y));
                labelMap[idx] = label;

                RegionInfo region;
                region.label = label;
                region.area = 0;
                region.minx = region.maxx = x;
                region.miny = region.maxy = y;
                region.sumx = 0.0;
                region.sumy = 0.0;
                region.cx = region.cy = 0.0;

                while (!s.empty()) {
                    auto cur = s.top(); s.pop();
                    int cx = cur.first;
                    int cy = cur.second;
                    int cidx = cy * width + cx;

                    region.area++;
                    region.sumx += (double)cx;
                    region.sumy += (double)cy;
                    region.minx = std::min(region.minx, cx);
                    region.maxx = std::max(region.maxx, cx);
                    region.miny = std::min(region.miny, cy);
                    region.maxy = std::max(region.maxy, cy);

                    for (int k = 0; k < 4; ++k) {
                        int nx = cx + dx[k];
                        int ny = cy + dy[k];
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            int nidx = ny * width + nx;
                            if (data[nidx * 3] == 0 && labelMap[nidx] == 0) {
                                labelMap[nidx] = label;
                                s.push(std::make_pair(nx, ny));
                            }
                        }
                    }
                }

                // 璸衡 centroid  region参ㄏノㄓ方
                if (region.area > 0) {
                    region.cx = region.sumx / (double)region.area;
                    region.cy = region.sumy / (double)region.area;
                }
                else {
                    region.cx = region.cy = 0.0;
                }

                regions.push_back(region);
            }
        }
    }

    // time measurement
    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();
    printf("cc: elapsed %.2f ms\n", ms);

    auto t0d = chrono::high_resolution_clock::now();

    // ミ肅︹舱BGR
     // 礶 bounding box 籔 centroid场ノ region  cx/cy
    unsigned char colors[3][3] = { {255,0,0}, {0,255,0}, {0,0,255} };
    for (auto& r : regions) {
        // draw bbox
        unsigned char* color = colors[(r.label - 1) % 3];
        for (int x = r.minx; x <= r.maxx; ++x) {
            int top_idx = r.miny * width + x;
            int bot_idx = r.maxy * width + x;
            for (int c = 0; c < 3; ++c) {
                rawdata[top_idx * 3 + c] = color[c];
                rawdata[bot_idx * 3 + c] = color[c];
            }
        }
        for (int y = r.miny; y <= r.maxy; ++y) {
            int left_idx = y * width + r.minx;
            int right_idx = y * width + r.maxx;
            for (int c = 0; c < 3; ++c) {
                rawdata[left_idx * 3 + c] = color[c];
                rawdata[right_idx * 3 + c] = color[c];
            }
        }

        // draw centroid (use rounded stored cx/cy)
        int cx_i = (int)std::round(r.cx);
        int cy_i = (int)std::round(r.cy);

        if (cx_i >= 0 && cx_i < width && cy_i >= 0 && cy_i < height) {
            // radius: fixed small radius, can be tuned or scaled with region.area
            int radius = 2; // produces a 5x5 filled dot
            // optional scaling (uncomment to enable adaptive size)
            // radius = std::min(6, std::max(1, (int)std::round(std::sqrt((double)r.area) / 20.0)));

            for (int dy_i = -radius; dy_i <= radius; ++dy_i) {
                int yy = cy_i + dy_i;
                if (yy < 0 || yy >= height) continue;
                for (int dx_i = -radius; dx_i <= radius; ++dx_i) {
                    int xx = cx_i + dx_i;
                    if (xx < 0 || xx >= width) continue;
                    // optional circle mask for nicer shape:
                    if (dx_i * dx_i + dy_i * dy_i > radius * radius) continue;
                    int cent_idx = yy * width + xx;
                    rawdata[cent_idx * 3 + 0] = 255;
                    rawdata[cent_idx * 3 + 1] = 255;
                    rawdata[cent_idx * 3 + 2] = 255;
                }
            }
        }
        // debug: ㄓ方 centroid 籔俱计畒夹絋粄璓┦
       /* printf("Region %d: Area=%d, Centroid=(%.3f, %.3f), Centroid_int=(%d,%d), BBox=[%d,%d]-[%d,%d]\n",
            r.label, r.area, r.cx, r.cy, cx_i, cy_i, r.minx, r.miny, r.maxx, r.maxy);*/
    }
    delete[] labelMap;

    // time measurement
    auto t1d = chrono::high_resolution_clock::now();
    double msd = chrono::duration_cast<chrono::duration<double, milli>>(t1d - t0d).count();
    printf("drawing: elapsed %.2f ms\n", msd);
    return rawimg;
}
