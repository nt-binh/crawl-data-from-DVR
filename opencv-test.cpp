#include<iostream>
#include<float.h>
#include<cmath>
#include<opencv2/opencv.hpp>
#include<time.h>
#include<string>
#include<stdlib.h>
#include<chrono>
#include<ctime>
#include<thread>
#include<typeinfo>
#include <X11/Xlib.h>

using namespace std;
using namespace cv;

static int captureFrame(string URL, int capture, string filename, string cameraName) {
	cv::VideoCapture cap;
	cap.open(URL + cameraName);
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); 					//get the width of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
	std::cout << "Size: " << dWidth << " x " << dHeight << endl;
	auto start = std::chrono::system_clock::now();
	
	while (true) {
		if (!cap.isOpened()) {
			std::cout << "Camera is not available yet!" << endl;
			std::cin.get();
			break;
		}
		Mat frame;
		cap.retrieve(frame);
		bool bSuccess = cap.read(frame);
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video file" << endl;
			break;
		}
		//cv::namedWindow("Demo", WINDOW_AUTOSIZE);
		//cv::imshow("Demo", frame);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_time = end - start;

		std::cout << int(elapsed_time.count()) << std::endl;
		if (int(elapsed_time.count()) >= capture){
			std::time_t t = std::time(0);   // get time now
    		std::tm* now = std::localtime(&t);
			std::string captureTime = std::to_string(now->tm_year + 1900) + std::to_string(now->tm_mon + 1) + std::to_string(now->tm_mday) + "_" + std::to_string(now->tm_hour) + std::to_string(now->tm_min) + ".jpg";
			std::string saveFile = "image/" + filename + "_" + cameraName + "_" + captureTime;
			cv::imwrite(saveFile, frame);
			start = std::chrono::system_clock::now();
			break;
		}
		//OLD CAPTURE METHOD
		/*
		if (count % capture == 0) {
			std::time_t t = std::time(0);   // get time now
    		std::tm* now = std::localtime(&t);
			std::string captureTime = std::to_string(now->tm_year + 1900) + std::to_string(now->tm_mon + 1) + std::to_string(now->tm_mday) + "_" + std::to_string(now->tm_hour) + std::to_string(now->tm_min) + ".jpg";
			std::string saveFile = "image/" + filename + "_" + cameraName + "_" + std::to_string(count / capture) + "_" + captureTime;
			cv::imwrite(saveFile, frame);
		}
		
		if (timer%27 == 0){
			count++;
			std::cout << "Counting: " << count << std::endl;
		}
		timer++;
		
		if (cv::waitKey(10) == 27) {
			break;
		}
	*/
	}
	//cv::destroyAllWindows();
	return 0;
}

int main(int argc, char** argv)
{
	int linkIndex, parkNameIndex;
	for (int i = 0; i < argc; i++){
		if (std::string(argv[i]) == "-t"){
			linkIndex = i+1;
		}
		if (std::string(argv[i]) == "-n"){
			parkNameIndex = i+1;
		}
	}
	//Webcam
	//cap.open(0);
	//External IP 117.102.196.48:18080/
	//External IP 120.143.48.172

	//cap.open("http://ADMIN:itd-hd2015@120.143.48.172:9000/1");
	//"rtsp://admin:itd-hd2015@124.41.73.7:554/10"
	string parkName = argv[parkNameIndex];
	string link = argv[linkIndex];
	std::cout << " ... Starting ..." << endl;
	std::thread cam1 (captureFrame, link, 3, parkName, "00");
	std::thread cam2 (captureFrame, link, 3, parkName, "10");
	std::thread cam3 (captureFrame, link, 3, parkName, "20");
	std::thread cam4 (captureFrame, link, 3, parkName, "30");
	cam1.join();
	cam2.join();
	cam3.join();
	cam4.join();
	/*
	std::cout << "Switching to Python execution file" << std::endl;
	std::time_t time = std::time(0);   // get time now
    std::tm* now = std::localtime(&time);
	std::string uploadTime = "python uploadDrive.py " + std::to_string(now->tm_year + 1900) + "/" + std::to_string(now->tm_mon + 1) + "/" + std::to_string(now->tm_mday);
	const char *command = uploadTime.c_str();
	system(command);								//upload to Google Drive once streaming ended
	*/
}
