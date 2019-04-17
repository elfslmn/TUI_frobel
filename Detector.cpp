#include "Detector.h"

bool initCalibrationPattern(const string filepath){
	Mat image = imread(filepath, CV_LOAD_IMAGE_GRAYSCALE);
	resize(image,image,pSize);
	bool found = findChessboardCorners(image, patternSize, pCorners);
	if(found)
	{
		cornerSubPix(image, pCorners, Size(11, 11), Size(-1, -1),
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		LOGD("Chessboard corners are detected in pattern image");
	}
	else
	{
		LOGE("Cannot found chessboard in projected image!");
	}

	imshow("Projector", projImage);	//TODO
	return found;
}

bool findHomography(Mat & image){
	Mat gray;	// TODO check how android handle native elements ??
	cvtColor(image, gray, COLOR_RGB2GRAY);
	bool found = findChessboardCorners(gray, patternSize, cCorners);
	if(found)
	{
		LOGD("Chessboard corners are detected in camera image");
		cornerSubPix(gray, cCorners, Size(11, 11), Size(-1, -1),
						TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		drawChessboardCorners(image, patternSize, Mat(cCorners), found);
		homography = findHomography( cCorners, pCorners, CV_RANSAC );
	}
	else
	{
		LOGE("Cannot found chessboard in camera image!");
	}
	return found;
}

bool applyHomography(vector<Point2f> & points){
	if(!homography.empty()){
		if(points.size()){
			perspectiveTransform(points, points, homography);
		}
		return true;
	}
	else{
		LOGE("No homography found!");
		return false;
	}
}
