#include <iostream>
#include <opencv2/opencv.hpp>
#include <cxxopts.hpp>

using namespace std;
using namespace cv;

int MODE = 0; // 0: snapshot, 1:diff

void drawText(Mat & image, string label, Point pt)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.3;
    int thickness = 1;
    int baseline = 0;
    Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height),Scalar(255,255,255), CV_FILLED);
    putText(image, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

void extractShapes(Mat & diff, Mat & gray)
{
	/*cvtColor(diff, gray, CV_RGB2GRAY);
	medianBlur(gray,gray,5);
    threshold(gray, gray, 50, 255, THRESH_BINARY); // 30 olunca sari gozukuyor, ama 50 daha robust */
    
    vector<Mat> rgbChannels(3);
    split(diff, rgbChannels);
    threshold(rgbChannels[0], rgbChannels[0], 70, 255, THRESH_BINARY); // Blue
    threshold(rgbChannels[1], rgbChannels[1], 70, 255, THRESH_BINARY); // Green
    threshold(rgbChannels[2], rgbChannels[2], 70, 255, THRESH_BINARY); // Red
    
    gray = rgbChannels[0] | rgbChannels[1] | rgbChannels[2]; // sum them
    medianBlur(gray,gray,5);
    //namedWindow("All",1);imshow("All", fin_img);
    
    vector<vector<Point> > contours;
    findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    cvtColor(gray, gray, CV_GRAY2RGB);
    bitwise_and(diff,gray,gray);
    for( unsigned int i = 0; i< contours.size(); i++ )
	{
		double area = contourArea(contours[i]);
		//cout << area / (diff.rows * diff.cols) << endl;
		if(area > 300 && area < 3000)
		{
			drawContours( gray , contours, i, Scalar(0,255,0),2);
		}
		else
		{
			drawContours( gray , contours, i, Scalar(0,0,255),2);
			continue;
		} 
		
		/*double perimeter = arcLength(contours[i],true);
		double epsilon = 0.02*perimeter ;
		vector<Point> approx;
		approxPolyDP(contours[i], approx, epsilon, true); // TODO not necessary
		for( unsigned int j = 0; j< approx.size(); j++ ){
           circle(gray, approx[j], 2, Scalar(255,0,0), -1, 8, 0 );
      	}        	
      	bool isConvex = isContourConvex(approx);*/

      	RotatedRect rr = minAreaRect(contours[i]);
      	//cout << rr.angle << "\t"<< rr.size <<"\t" << rr.size.height / rr.size.width << endl;
      	
      	double ratio = rr.size.height / rr.size.width ;
      	if( ratio < 1 ) ratio = 1/ratio;
      	if(ratio > 5)	// stick
      	{	
      		drawText(gray, "STK" , rr.center);
      	}
      	else if( ratio <= 1.2) {
      		drawText(gray, "CRC" , rr.center);
      	}
      	else if( ratio > 1.2 && ratio < 3)
      	{
      		double perim = rr.size.height + rr.size.width;
      		if(perim <= 90) drawText(gray, "GRN" , rr.center);
      		else if(perim > 90 && perim <= 120) drawText(gray, "ORG" , rr.center);
      		else drawText(gray, "RED" , rr.center);
      	}
      	
      	/*Vec4f line;
      	fitLine(contours[i],line,CV_DIST_L2,0,0.01,0.01);
      	double m = atan2(line[1], line[0])* 180 / 3.14;*/
		
	}
}

int main (int argc, char *argv[])
{
    // Parse options
    cxxopts::Options options("TUI Frobel");
    options
    .allow_unrecognised_options()
    .add_options()
    ("c", "Camera", cxxopts::value<int>(), "camera");
    cout << options.help() << endl;
    cout << "Press 'ESC' for exit" << endl << endl;
    auto result = options.parse(argc, argv);

    int camId = 0;
    if(result.count("c"))
    {
        camId = result["c"].as<int>();
    }
    cout << "Cam id = " << camId << endl;

    // windows
    namedWindow ("Camera", WINDOW_AUTOSIZE);
    namedWindow ("Snapshot", WINDOW_AUTOSIZE);
    namedWindow ("Diff", WINDOW_AUTOSIZE);
    namedWindow ("Projector", WINDOW_NORMAL);
    setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    moveWindow("Projector", 1366, 0); // 1366 width of my screen

    // projected image
    Mat projImage = imread("images/espas_011-intro.png" ,IMREAD_UNCHANGED); // TODO save in a mat
    resize(projImage,projImage,Size(1280,720));
    imshow("Projector", projImage);
    waitKey(100);

    VideoCapture cap(camId);
    Mat camImage, snapshot, diff, gray;

    while (true)
    {
        cap >> camImage;
        if(camImage.empty()){
            cerr << "No camImage" << endl;
            break;
        }
        flip(camImage, camImage, -1); // flip 180 degree

        if(MODE == 0){
            snapshot = camImage.clone();
            imshow("Snapshot", snapshot);
            MODE = 1;
        }
        else{
            absdiff(camImage, snapshot, diff); 
            extractShapes(diff, gray);
            imshow("Diff", gray);
        }
        imshow("Camera", camImage);

        char key = (char) waitKey(30);
        if(key == 'p')
        {
            key = (char) waitKey();
        }
        if (key == 'q' || key == 27)
        {
            break;
        }

    }


    return 0;
}
