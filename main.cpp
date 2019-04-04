#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <cxxopts.hpp>

using namespace std;
using namespace cv;

int MODE = 0; // 0: homography, 1:levels

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
	namedWindow ("Projector", WINDOW_NORMAL);
	setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	moveWindow("Projector", 1366, 0); // 1366 width of my screen
	
	// projected image
	Mat projImage;
	vector<Point2f> pCorners, cCorners;
	Size patternSize(9,6); 
	if(MODE == 0 )
	{
		projImage = imread("images/pattern.png", CV_LOAD_IMAGE_GRAYSCALE);
		resize(projImage,projImage,Size(1280,720));
		bool found = findChessboardCorners(projImage, patternSize, pCorners);
		if(found)
		{
 			cornerSubPix(projImage, pCorners, Size(11, 11), Size(-1, -1), 
 							TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
 			cout << "Chessboard corners are detected in projected image" << endl;
		}
		else
		{
			cerr << "Cannot found chessboard in projected image!" << endl;
		}
		imshow("Projector", projImage);
	}
	
	Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();
	detectorParams->cornerRefinementMethod = aruco::CORNER_REFINE_SUBPIX;
	detectorParams->adaptiveThreshWinSizeStep = 2;	// 10
	detectorParams->adaptiveThreshWinSizeMax = 10;	// 23
	detectorParams->minMarkerPerimeterRate = 0.002; // 0.03
	detectorParams->maxMarkerPerimeterRate = 0.1;		// 4
	detectorParams->perspectiveRemovePixelPerCell = 10; 		// 4;
	detectorParams->perspectiveRemoveIgnoredMarginPerCell = 0.3;// 0.13 

	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(1));
	

	VideoCapture cap(camId);
	Mat camImage, gray;
	Mat homography;
	int frame = 0;
	int has = 0;
	double totalTime = 0;
	
	while (true) 
	{
        cap >> camImage;
        if(camImage.empty()){
        	cerr << "No camImage" << endl;
        	break;
        }
        flip(camImage, camImage, -1); // flip 180 degree       
        
        if( MODE == 0 )
        {
        	cvtColor(camImage, gray, COLOR_RGB2GRAY);
        	bool found = findChessboardCorners(gray, patternSize, cCorners);
        	if(found)
			{
				cout << "Chessboard corners are detected in camera image" << endl;
	 			cornerSubPix(gray, cCorners, Size(11, 11), Size(-1, -1), 
	 							TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	 			drawChessboardCorners(camImage, patternSize, Mat(cCorners), found);
	 			homography = findHomography( cCorners, pCorners, CV_RANSAC );
	 			imshow("Camera", camImage);
	 			 // for debug, remove later
	 			cout << homography << endl; 
	 			//destroyWindow("Projector");
	 			waitKey(0);
	 			// end debug
	 			MODE = 1;
	 			projImage = imread("images/espas_011-intro.png" ,IMREAD_UNCHANGED);
				resize(projImage,projImage,Size(1280,720));
				imshow("Projector", projImage);
			}
			else
			{
				cerr << "Cannot found chessboard in camera image!" << endl;
				imshow("Camera", camImage); 
			}		
        }
        else
        {	
        	double tick = (double)getTickCount();
        	vector< int > ids;
		    vector< vector< Point2f > > corners, rejected;
		    vector< Vec3d > rvecs, tvecs;
		    aruco::detectMarkers(camImage, dictionary, corners, ids, detectorParams, rejected);
		    if(ids.size() > 0) {
		        aruco::drawDetectedMarkers(camImage, corners, ids);
		        has++;
		    }
		    double currentTime = ((double)getTickCount() - tick) / getTickFrequency();
        	totalTime += currentTime;
        	frame++;
			if(frame % 100 == 0){
				printf("Accuracy: %.2f \t Time : %.2f ms\n", 
						(double)has/frame*100, 1000 * totalTime / double(frame));
				totalTime = 0;
				frame = 0;
				has = 0;
			}
			
			/*if(!homography.empty()){
				Mat debug = Mat::zeros(camImage.size(), CV_8UC1);
				for(int i=0; i < (int)corners.size(); i++)
				{
					Scalar center = mean(corners[i]);
					circle(debug, Point(center[0], center[1]), 20, 255,-1);  
				} 
				warpPerspective(debug, projImage, homography, Size(1280,720));
				imshow("Projector", projImage); 			
			}*/
			
			if(!homography.empty()){
				vector<Point2f> original;
				vector<Point2f> warped;
				projImage = imread("images/espas_011-intro.png" ,IMREAD_UNCHANGED); // TODO save in a mat
				resize(projImage,projImage,Size(1280,720));
				for(int i=0; i < (int)corners.size(); i++)
				{
					Scalar center = mean(corners[i]);
					original.push_back(Point2f(center[0], center[1])); 
				} 
				//warpPerspective(debug, projImage, homography, Size(1280,720));
				if(corners.size()) perspectiveTransform(original, warped, homography);
				for(int i=0; i < (int)warped.size(); i++)
				{
					circle(projImage, warped[i], 30, Scalar(255,255,255), 3);
				}
				imshow("Projector", projImage); 			
			}
			
        	imshow("Camera", camImage); 
        }
        
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
