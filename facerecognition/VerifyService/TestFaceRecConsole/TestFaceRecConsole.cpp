// TestFaceRecConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "FaceRecDLib.h"
int main(int argc, char** argv)
{
	auto start3 = std::chrono::high_resolution_clock::now();
	std::cout << "load\n" << std::endl;
	FaceRecDLib FaceRec;

	std::string image_path = argv[1];
	std::string descr = FaceRec.face_descriptor_calc(image_path);
	//std::string video_path = "C:\\Users\\AnMoshkova\\Documents\\face_test.mp4";
	std::string video_path = argv[2];

	std::cout << "\n" << std::endl;
	auto finish3 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish3 - start3).count() << " ms" << std::endl;
	std::cout << "\n" << std::endl;

	std::cout << "face_verification_threads\n" << std::endl;
	auto start1 = std::chrono::high_resolution_clock::now();

	ResultVerify result = FaceRec.face_verification_threads(descr, video_path, 0.6);
	std::cout << std::boolalpha << result.isVerify << std::endl;
	std::cout << result.data.size() << std::endl;

	std::cout << "\n" << std::endl;
	auto finish1 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish1 - start1).count() << " ms" << std::endl;
	std::cout << "\n" << std::endl;

	std::cout << "face_verification\n" << std::endl;
	auto start2 = std::chrono::high_resolution_clock::now();

	auto result2 = FaceRec.face_verification(descr, video_path, 0.6);
	std::cout << std::boolalpha << result2.isVerify << std::endl;
	std::cout << result2.data.size() << std::endl;

	std::cout << "\n" << std::endl;
	auto finish2 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish2 - start2).count() << " ms" << std::endl;
	std::cout << "\n" << std::endl;

	system("pause");
	return 0;
}

