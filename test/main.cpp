#include <iostream>
#include "windows.h"
#include "interface.h"
#include <opencv2/opencv.hpp>

using namespace std;

void __stdcall getResult(int* px, int* py, size_t size, void* context)
{
	for (size_t i = 0; i < size; i++){ 
		cout << "x: " << *px << ", y:" << *py << endl;
	}
}


int main(int argc, char* argv[])
{
	cout << "hello, vs" << endl;

	SetCurrentDirectory(L"D:\\code\\MotionDetection\\release");
	HINSTANCE m_hMotionDetectorDll;
	m_hMotionDetectorDll = LoadLibrary(L"D:\\code\\MotionDetection\\release\\MotionDetection.dll");
	if (m_hMotionDetectorDll != NULL){
		cout << "Load dll success!" << endl;
	}
	else{
		cout << "Load dll failed! " << GetLastError() << endl;
		return -1;
	}

	_createMotionDetector	m_pfnCreateMotionDetector;
	_deleteMotionDetector	m_pfnDestroyMotionDetector;
	_setRoomSize			m_pfnSetRoomSize;
	_setStudentRegion		m_pfnSetStudentRegion;
	_setCaffemodelPath		m_pfnCaffemodelPath;
	_setCallback			m_pfnSetCallback;
	_setShowWindows			m_pfnSetShowWindows;
	_setFrame				m_pfnSetFrame;

	m_pfnCreateMotionDetector = (_createMotionDetector)GetProcAddress(m_hMotionDetectorDll, "createMotionDetector");
	m_pfnDestroyMotionDetector = (_deleteMotionDetector)GetProcAddress(m_hMotionDetectorDll, "deleteMotionDetector");
	m_pfnSetRoomSize = (_setRoomSize)GetProcAddress(m_hMotionDetectorDll, "setRoomSize");
	m_pfnSetStudentRegion = (_setStudentRegion)GetProcAddress(m_hMotionDetectorDll, "setStudentRegion");
	m_pfnCaffemodelPath = (_setCaffemodelPath)GetProcAddress(m_hMotionDetectorDll, "setCaffemodelPath");
	m_pfnSetCallback = (_setCallback)GetProcAddress(m_hMotionDetectorDll, "setCallback");
	m_pfnSetShowWindows = (_setShowWindows)GetProcAddress(m_hMotionDetectorDll, "setShowWindows");
	m_pfnSetFrame = (_setFrame)GetProcAddress(m_hMotionDetectorDll, "setFrame");

	LPVOID					m_lpMotionDetection;
	m_lpMotionDetection = m_pfnCreateMotionDetector();

	if (!m_pfnCreateMotionDetector)
	{
		FreeLibrary(m_hMotionDetectorDll);
		m_hMotionDetectorDll = NULL;
	}
	else {
		m_pfnSetRoomSize(m_lpMotionDetection, 100, 100);
		m_pfnSetShowWindows(m_lpMotionDetection, false, true);
		m_pfnSetCallback(m_lpMotionDetection, getResult, NULL);

		char szCaffemodelPath[1000] = { 0 };
		sprintf(szCaffemodelPath, "D:\\code\\mtcnn\\mtcnn\\model");
		m_pfnCaffemodelPath(m_lpMotionDetection, szCaffemodelPath);

		string videoPath = "D:\\video\\4.mp4";
		//if (argc == 2) videoPath = argv[1];
		cv::VideoCapture cam;
		cv::Mat frame;
		if (cam.open(0)){
			cam >> frame;
			m_pfnSetStudentRegion(m_lpMotionDetection, 0, 0, 0, frame.cols, frame.rows, frame.cols, frame.rows, 0);
			while (frame.isContinuous() && !frame.empty()){
				resize(frame, frame, cv::Size(frame.cols/3, frame.rows/3));
				uchar *data = frame.data;
				int height = frame.rows;
				int width = frame.cols;
				int length = height*width*frame.channels();
				m_pfnSetFrame(m_lpMotionDetection, data, height, width, length);
				
				//cv::imshow("aaa", frame); 
				cv::waitKey(20);
				cam >> frame;
			}
		}
		else{ 
			cout << "invalid video" << endl;
		}
		FreeLibrary(m_hMotionDetectorDll);
	}

	int a;
	cin >> a;
	return 0;
}
