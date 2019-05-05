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

void Util::drawRotetedRectAxis(Mat & image, RotatedRect rr){
	Point2f vertices[4];
	rr.points(vertices);
	line(image, (vertices[0]+vertices[1])/2,  (vertices[2]+vertices[3])/2, Scalar(255,255,255), 1);
	line(image, (vertices[1]+vertices[2])/2,  (vertices[0]+vertices[3])/2, Scalar(255,255,255), 1);

	for(int i=0; i<4; i++){
		Util::drawText(image, to_string(i), vertices[i]);
	}
}

/*void updateBrightness(Mat & image, int beta){
	for( int y = 0; y < image.rows; y++ ) {
		for( int x = 0; x < image.cols; x++ ) {
			for( int c = 0; c < image.channels(); c++ ) {
				image.at<Vec3b>(y,x)[c] =
				  saturate_cast<uchar>( image.at<Vec3b>(y,x)[c] + beta );
			}
		}
	}
}*/
