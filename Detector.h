#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define LOGI(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGD(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGE(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)

class Detector{
public:
	const static Size pSize = Size(1280,720);
	Mat homography;

private:
	const static Size patternSize = Size(9,6);
	vector<Point2f> pCorners, cCorners;

};
