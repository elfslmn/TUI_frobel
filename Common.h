#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//#define DEBUG

#define LOGI(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGD(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGE(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)

#define PI 3.14159265

enum Mode { RGB , HSV };
