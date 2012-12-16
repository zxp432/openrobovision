//
// тестирование видеозахвата OpenCV через интерфейс C++
//
// http://robocraft.ru
//

#include "orv/orv.h"

#include "opencv2/opencv.hpp"

int main(int argc, char* argv[])
{
	cv::VideoCapture cap(0); // откроем камеру по-умолчанию
	if(!cap.isOpened())		 // успешно?
	{
		std::cout<<"[!] Error: cant open camera!"<<std::endl;
		return -1;
	}
	cv::Mat gray;
	cv::Mat edges;
	cv::namedWindow("frame");
	cv::namedWindow("edges");
	while(1)
	{
		cv::Mat frame;
		// у VideoCapture перегружен оператор >> ,который можно использовать для считывания кадра
		cap >> frame; // получаем новый кадр
		cv::imshow("frame", frame);		// показываем картинку
		cv::cvtColor(frame, gray, CV_BGR2GRAY); // конвертируем в градации серого
		cv::GaussianBlur(gray, gray, cv::Size(7,7), 1.5, 1.5); // размытие (фильтром Гаусса)
		cv::Canny(gray, edges, 0, 30);	// детектор границ Кенни
		cv::imshow("edges", edges);		// показываем картинку
		int key = cv::waitKey(33);
		if(key == 27) //ESC
			break;
		else if(key == 13) // Enter
		{
			// сохраним кадр в файл
			cv::imwrite("frame.jpg", frame);
			cv::imwrite("edges.jpg", edges);
			std::cout<<"[i] Saved."<<std::endl;
		}

	}
	// камера будет автоматически освобождена в деструкторе VideoCapture
	return 0;
}
