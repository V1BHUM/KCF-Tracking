#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "src/kcftracker.hpp"
#include "src/fhog.hpp"
#include <vector>
#include <string>

int main() {

	// Create video capturing object
	// 0 opens default camera, otherwise filename as argument
	cv::VideoCapture video(0);

	// Check that video is opened
	if (!video.isOpened()) return -1;

	// For saving the frame
	cv::Mat frame;
	// Get video resolution
	int frameWidth = video.get(cv::CAP_PROP_FRAME_WIDTH);
	int frameHeigth = video.get(cv::CAP_PROP_FRAME_HEIGHT);

	
	// Create tracker, select region-of-interest (ROI) and initialize the tracker

    /*
        KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

        tracker.init( Rect(xMin, yMin, width, height), frame );

        result = tracker.update(frame);
    */

	bool hog,fix,multi;
	std::string s;
	std::cout<<"Enter Bitmask (HOG, FIXEDWINDOW, MULTISCALE, LAB) : ";
	std::cin>>s;
	if(s.size()!=3)
	{
		std::cout<<"Wrong input\n";
		return -1;
	}
	hog = s[0] == '1',fix = s[1] == '1',multi = s[2] == '1';
	
	KCFTracker tracker(hog, fix, multi, false);
	video.read(frame);
	cv::Rect2d trackingBox;
	
	std::cout<<"Enter Padding : ";
	double dpadd;
	std::cin>>dpadd;
	tracker.padding = dpadd;	
	
	
	
	trackingBox = cv::selectROI(frame, false);
	cv::Rect box = cv::Rect(trackingBox.x,trackingBox.y,trackingBox.width,trackingBox.height);
	tracker.init(box,frame);
	// Loop through available frames
	std::cout<<std::fixed;
	std::cout<<std::setprecision(3);
	clock_t current_ticks, delta_ticks;
	double fps = 0;

	std::vector<double> frs;

	int counter = 0;
	while (true) {
		// Update the tracker and draw the rectangle around target if update was successful
		video.read(frame);
		current_ticks = clock();

		box = tracker.update(frame);
        
		 //std::cout<<"Frame Rate : "<<fps<<"fps\t\tFrame Time : "<<1/fps<<"s"<<std::endl;

		cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2, 8);

		// Display the frame
		cv::imshow("Video feed", frame);
		delta_ticks = clock() - current_ticks; //the time, in ms, that took to render the scene
		if(delta_ticks > 0)
		{
			fps = (CLOCKS_PER_SEC*1.0) / delta_ticks;
			frs.push_back(fps);
		}
		

		// For breaking the loop
		if (cv::waitKey(25) >= 0) break;

	} // end while (video.read(frame))

	  // Release video capture and writer
	video.release();

	// Destroy all windows
	cv::destroyAllWindows();
	double total = 0;
	for(auto &x:frs)
	total += x;
	total /= frs.size();
	std::cout<<"\n"<<total<<std::endl;
	//int ar[] = {tracker.ROI_WIDTH,tracker.ROI_WIDTH,tracker.padding,total};
	FILE *f = fopen("output.csv","a");
	//fprintf(f,"%d,%d,%f,%d,%d,%d,%f\n",tracker.ROI_WIDTH,tracker.ROI_HEIGHT,tracker.padding,hog,fix,multi,total);
	fclose(f);
	return 0;

}

/*
g++ temp.cpp src/kcftracker.cpp src/fhog.cpp -o output `pkg-config --cflags --libs opencv4`

*/
