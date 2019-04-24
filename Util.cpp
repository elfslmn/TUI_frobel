#include "Util.h"

void Util::drawText(Mat & image, string label, Point pt)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.3;
    int thickness = 1;
    int baseline = 0;
    Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height),Scalar(255,255,255), CV_FILLED);
    putText(image, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}
