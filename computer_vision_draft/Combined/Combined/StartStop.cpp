#include "StartStop.h"

StartStop::StartStop(std::mutex* mutex_var) {
	colorLimitsHSV redLimsTemp, greenLims;
	redLimsTemp.iLowH = 0;
	redLimsTemp.iHighH = 10;
	redLimsTemp.iLowS = 70;
	redLimsTemp.iHighS = 255;
	redLimsTemp.iLowV = 50;
	redLimsTemp.iHighV = 255;

	this->redLims = redLimsTemp;
	this->greenLims = redLimsTemp;

	this->mutex_var = mutex_var;

}


int StartStop::start() {
	cout << "Circle detector started" << endl;
	int iLowH = 56;
	int iHighH = 88;

	int iLowS = 75;
	int iHighS = 255;


	int iLowV = 78;
	int iHighV = 255;	//Sets the hue values to only detect green

	int i = startStop(iLowH, iHighH, iLowS, iHighS, iLowV, iHighV, false); //start the loop with the Hue values
	if (i == 1) {
		//TCPclient(0, 0);
		cout << "Found green" << endl;
		return red();	//if successfull start search for red
	}
}


int StartStop::red() {
	int iLowH = 0;
	int iHighH = 20;

	int iLowS = 70;
	int iHighS = 255;

	int iLowV = 50;
	int iHighV = 255;	//Sets the hue values to only detect red

	int i = startStop(iLowH, iHighH, iLowS, iHighS, iLowV, iHighV, true);
	if (i == 1) {
		//TCPclient(1, 0
		cout << "Found red" << endl;
	}
	return -1;
}


int StartStop::startStop(int LowH, int HighH, int LowS, int HighS, int LowV, int HighV, bool isRed) {
	while (true) {
		Mat  imgHSV, imgThreshold, imgLap, abs_Lap; //array to store picture data


		mutex_var->lock();
		if (hsvImage.empty()) {
			mutex_var->unlock();
			std::this_thread::sleep_for(100ms);
			cout << "WAITING FOR IMAGE(CIRCLE)" << endl;
			continue;
		}
		imgHSV = hsvImage.clone();
		mutex_var->unlock();


		if (isRed == true) {
			Mat1b mask1, mask2;
			inRange(imgHSV, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), mask1); //Filter out colors
			inRange(imgHSV, Scalar(170, 70, 50), Scalar(180, 255, 255), mask2);
			Mat1b mask = mask1 | mask2;
			imgThreshold = mask;
			cout << "red" << endl;
		}
		else {
			inRange(imgHSV, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), imgThreshold); //Filter out colors
			cout << "green" << endl;
		}

		GaussianBlur(imgThreshold, imgThreshold, Size(9, 9), 2, 2);	//Doesn't work without
		Laplacian(imgThreshold, imgLap, CV_16S, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(imgLap, abs_Lap);


		

		vector<Vec3f> circles;
		HoughCircles(abs_Lap, circles, HOUGH_GRADIENT, 2, abs_Lap.rows, 250, 250, 25, 0); //detect the circle
		if (!circles.empty()) {
			return 1;
		}

		#ifndef RACE
				imshow("image", imgThreshold);
				imshow("Lap", abs_Lap);
				waitKey(10);
		#endif

	}
}

