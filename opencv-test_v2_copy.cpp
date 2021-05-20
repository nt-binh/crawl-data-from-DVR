#include<iostream>
#include<float.h>
#include<cmath>
#include<opencv2/opencv.hpp>
#include<time.h>
#include<string>
//#include<stdlib.h>
#include<chrono>
#include<ctime>
#include<thread>
//#include<typeinfo>
#include<fstream>

using namespace std;
using namespace cv;

static int captureFrame(string URL, int capture, string filename, string cameraName) {
	cv::VideoCapture cap;
	cap.open(URL + cameraName);
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); 					//get the width of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
	std::cout << "Size: " << dWidth << " x " << dHeight << std::endl;
	auto start = std::chrono::system_clock::now();
	while (true) {
		if (!cap.isOpened()) {
			std::cout << "Camera is not available yet!" << std::endl;
			std::cin.get();
			return -1;
		}
		cv::Mat frame;
		cap.retrieve(frame);
		bool bSuccess = cap.read(frame);
		if (!bSuccess) //if not success, break loop
		{
			std::cout << "Cannot read a frame from video file" << std::endl;
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
			cout << "Counting: " << count << endl;
		}
		timer++;
		
		if (cv::waitKey(10) == 27) {
			break;
		}
		*/
	}
	cv::destroyAllWindows();
	return 0;
}

int main(int argc, char** argv)
{
    /*
	int linkIndex, parkNameIndex;
	for (int i = 0; i < argc; i++){
		if (std::string(argv[i]) == "-t"){
			linkIndex = i+1;
		}
		if (std::string(argv[i]) == "-n"){
			parkNameIndex = i+1;
		}
	}
    */
    string parkNames[1];
    string linkNames[1];
    ifstream file("parkList_copy.txt");
    string line;
    if (file.is_open())
    {
        int count = 0;
        std::string delimiter = ",";
        while ( file.good() )
        {
            size_t pos = 0;
            getline (file, line);
            std::string token;
            while ((pos = line.find(delimiter)) != std::string::npos) 
            {
                if (count%2 == 0){
                    parkNames[count/2] = line.substr(0, pos);
                } else{
                    linkNames[(count-1)/2] = line.substr(0, pos);
                }
                count++;
                line.erase(0, pos + delimiter.length());
            }
        }
        file.close();
    }
    else std::cout << "Unable to open file" << std::endl;
    
    //string parkNames[8] = {"19.須玉第18", "19.須玉第13", "19.須玉第1-2", "19.須玉第2-1", "19.白州第2", "20.中川2", "20.旧塩尻3", "20.旧塩尻5/6"};
    //string linkNames[8] = {"116.58.180.234:554/", "116.58.180.234:1026/", "203.181.25.68:554/", "124.41.73.7:554/", "219.117.202.65:554/", "124.41.87.157:554/", "116.58.169.35:554/", "116.58.174.46:1026/"};
	std::cout << " ... Starting ..." << std::endl;

    std::thread cam[4];
    int counter = 0;
    for (int j = 0; j < sizeof(parkNames) / sizeof(parkNames[0]); j++){
        string linkName = "rtsp://admin:itd-hd2015@" + linkNames[j];
        for (int i = 0; i < 4; i++){
            std::string camNo = std::to_string(i) + "0";
            cam[counter] = std::thread(captureFrame, linkName, 3, parkNames[j], camNo);
            counter++;
        }
    }
    for (int i = 0; i < 4; i++){
        cam[i].join();
    }
    //std::cout << "Switching to Python execution file" << std::endl;
	//std::time_t time = std::time(0);   				// get time now
    //std::tm* now = std::localtime(&time);
	//std::string uploadTime = "python uploadDrive.py " + std::to_string(now->tm_year + 1900) + "/" + std::to_string(now->tm_mon + 1) + "/" + std::to_string(now->tm_mday);
	//const char *command = uploadTime.c_str();
	//system(command);								//upload to Google Drive once streaming ended
}