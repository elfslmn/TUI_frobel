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
    shapes.clear();
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

    //processContours
    for( unsigned int i = 0; i< contours.size(); i++ )
    {
        double area = contourArea(contours[i]);

        if(area > Params::minShapeArea && area <  Params::maxShapeArea)
        {
            drawContours( debug , contours, i, Scalar(0,255,0),2);
        }
        else
        {
            drawContours( debug , contours, i, Scalar(0,0,255),2);
            continue;
        }

        RotatedRect rr = minAreaRect(contours[i]);
        Util::drawRotetedRectAxis(debug, rr);
        //cout << rr.angle << "\t"<< rr.size <<"\t" << rr.size.height / rr.size.width << endl;

        // Ratio
        double ratio = rr.size.height / rr.size.width ;
        if( ratio < 1 ) ratio = 1/ratio;

        // Convexivity defects
        vector<int> hull_indices;
        vector<cv::Vec4i> defects;
        convexHull(contours[i], hull_indices);
        convexityDefects(contours[i], hull_indices, defects);
        int defectCount = 0;
        float defectDepth = 0;
        double angle = -1;
        for(int j=0; j< defects.size(); j++){
            float depth = defects[j][3]/256;
            if(depth <= Params::minDefectDepth) continue; // 5

            defectCount++;
            defectDepth = depth;

            int startidx = defects[j][0];
            Point ptStart( contours[i][startidx] ); // point of the contour where the defect begins
            int endidx = defects[j][1];
            Point ptEnd( contours[i][endidx] ); // point of the contour where the defect ends
            int faridx = defects[j][2];
            Point ptFar( contours[i][faridx]);// the farthest from the convex hull point within the defect

            line( debug, ptStart, ptEnd, CV_RGB(255,255,0), 2 );
            line( debug, (ptStart+ptEnd)/2, ptFar, CV_RGB(255,255,0), 2 );
            circle( debug, ptStart, 4, Scalar(255,0,0), 2 );
            circle( debug, ptEnd, 4, Scalar(255,0,255), 2 );

            double y_diff = (ptEnd.y - ptStart.y)*-1; // *-1 since y coordinate is different in opencv
            double x_diff = ptEnd.x - ptStart.x;
            angle = atan2(y_diff, x_diff) * 180 / PI;
            if(angle < 0) angle += 360;

            Util::drawText(debug, to_string(angle) ,Point(rr.center.x-100,rr.center.y));
            //Util::drawText(debug, to_string(depth) ,Point(rr.center.x+50,rr.center.y));
        }
        if( defectCount > 1){ // shapes should have 1 convexivity defect at most
            drawContours( debug , contours, i, Scalar(0,0,255),2);
            continue;
        }

        if(ratio > 5 && defectDepth == 0)	// stick
        {
            Util::drawText(debug, "STK" , rr.center);
            // stick does not have convexivity defect, calculate angle from rect
            Point2f vertices[4];
            rr.points(vertices);
            double y_diff, x_diff;
            if(norm(vertices[0] - vertices[1]) > norm(vertices[1] - vertices[2]) ){
                y_diff = (vertices[1].y - vertices[0].y)*-1; // *-1 since y coordinate is different in opencv
                x_diff = vertices[1].x - vertices[0].x;
            }
            else{
                y_diff = (vertices[2].y - vertices[1].y)*-1; // *-1 since y coordinate is different in opencv
                x_diff = vertices[2].x - vertices[1].x;
            }

            angle = atan2(y_diff, x_diff) * 180 / PI;
            Util::drawText(debug, to_string(angle) ,Point(rr.center.x-100,rr.center.y));

            shapes.push_back(Shape(STICK, rr.center, angle));
        }
        else if( ratio <= 1.2 && defectDepth == 0) {
            Util::drawText(debug, "YEL" , rr.center);
            shapes.push_back(Shape(CIRCLE, rr.center, -1));
        }
        else if( ratio > 1.2 && ratio < 3)
        {
            double perim = rr.size.height + rr.size.width;
            //cout << perim << endl;
            if(perim < 95 && defectDepth > 12 && defectDepth < 22){
                Util::drawText(debug, "GRN" , rr.center);
                shapes.push_back(Shape(GREEN, rr.center, angle));
            }
            else if(perim >= 95 && perim <= 120 && defectDepth > 25 && defectDepth < 34){
                Util::drawText(debug, "ORG" , rr.center);
                shapes.push_back(Shape(ORANGE, rr.center, angle));
            }
            else if(perim >= 121 && perim <= 170 && defectDepth >= 34 && defectDepth < 47){
                Util::drawText(debug, "RED" , rr.center);
                shapes.push_back(Shape(RED, rr.center, angle));
                /* TODO green+stick or green+green can be detected as red sometimes
                 you can check the defect lenght or angle of the defect*/
            }
            else{
                drawContours( debug , contours, i, Scalar(0,0,255),2);
            }
        }
        else{
            drawContours( debug , contours, i, Scalar(0,0,255),2);
        }
    }

    //if(shapes.size()) LOGD("%d shapes are found", shapes.size());
}
