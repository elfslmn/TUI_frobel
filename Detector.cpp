#include "Detector.h"

Detector::Detector(){
    isSnapshotCaptured = false;
}

void Detector::resetSnapshot(){
    isSnapshotCaptured = false;
}

void pos(Mat &src)
{
    threshold(-src, src, 0, 0, CV_THRESH_TRUNC);
    src = -src;
    return;
}

bool Detector::processFrame(Mat & frame){
    if(isSnapshotCaptured){
        absdiff(frame, snapshot, diff);
        //diff = frame - snapshot ;
        //pos(diff);
        namedWindow("Diff",1); imshow("Diff", diff);
        extractShapes();
        namedWindow("Debug",1); imshow("Debug", debug);
        return false;
    }
    else{
        // Save current frame;
        snapshot = frame.clone();
        namedWindow("Snapshot",1); imshow("Snapshot", snapshot);
        isSnapshotCaptured = true;
        return true;
    }
}

void Detector::extractShapes(){
    centers.clear();
    if(colorMode == RGB){
        vector<Mat> rgbChannels(3);
        split(diff, rgbChannels);

        threshold(rgbChannels[0], rgbChannels[0], Params::blue_thresh, 255, THRESH_BINARY); // Blue
        threshold(rgbChannels[1], rgbChannels[1], Params::green_thresh, 255, THRESH_BINARY); // Green
        threshold(rgbChannels[2], rgbChannels[2], Params::red_thresh, 255, THRESH_BINARY); // Red

        medianBlur(rgbChannels[0],rgbChannels[0],7);
        //medianBlur(rgbChannels[1],rgbChannels[1],7);
        //medianBlur(rgbChannels[2],rgbChannels[2],7);

        mask = rgbChannels[0] | rgbChannels[1] | rgbChannels[2]; // sum them

        #ifdef DEBUG
        namedWindow("B",1);imshow("B", rgbChannels[0]);
        namedWindow("G",1);imshow("G", rgbChannels[1]);
        namedWindow("R",1);imshow("R", rgbChannels[2]);
        #endif
    }
    else if(colorMode == HSV){
        Mat hsv;
    	cvtColor(diff, hsv, CV_BGR2HSV);
        vector<Mat> channels(3);
        split(hsv, channels);
        threshold(channels[2], channels[2], Params::hue_thresh, 255, THRESH_BINARY); // Hue
        mask = channels[2];
    }
    else{
        assert(false);
    }

    medianBlur(mask,mask,7);
    //namedWindow("Mask",1); imshow("Mask", mask);

    vector<vector<Point> > contours;
    findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    cvtColor(mask, debug, CV_GRAY2RGB);
    bitwise_and(diff,debug,debug);
    for( unsigned int i = 0; i< contours.size(); i++ )
    {
        double area = contourArea(contours[i]);
        //cout << area / (diff.rows * diff.cols) << endl;
        if(area > 300 && area < 3000)
        {
            drawContours( debug , contours, i, Scalar(0,255,0),2);
        }
        else
        {
            drawContours( debug , contours, i, Scalar(0,0,255),2);
            continue;
        }

        RotatedRect rr = minAreaRect(contours[i]);
        //cout << rr.angle << "\t"<< rr.size <<"\t" << rr.size.height / rr.size.width << endl;

        double ratio = rr.size.height / rr.size.width ;
        if( ratio < 1 ) ratio = 1/ratio;
        if(ratio > 5)	// stick
        {
            Util::drawText(debug, "STK" , rr.center);
            centers.push_back(rr.center);
        }
        else if( ratio <= 1.2) {
            Util::drawText(debug, "CRC" , rr.center);
            centers.push_back(rr.center);
        }
        else if( ratio > 1.2 && ratio < 3)
        {
            double perim = rr.size.height + rr.size.width;
            if(perim <= 90) Util::drawText(debug, "GRN" , rr.center);
            else if(perim > 90 && perim <= 120) Util::drawText(debug, "ORG" , rr.center);
            else Util::drawText(debug, "RED" , rr.center);

            centers.push_back(rr.center);
        }


    }
}
