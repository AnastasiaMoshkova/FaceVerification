#ifndef FACERECDLIB_H
#define FACERECDLIB_H

#include "basictypes.h"

typedef std::vector<std::string> ListString;

struct ResultVerify {
	ResultVerify() {}
	bool isVerify = false;
	ListString data;
	std::string data_t;
};

struct ResultVerify_threads {
	ResultVerify_threads() {}
	bool isVerify_t = false;
	std::string data_t;
};

class FaceRecDLib {
public:
	FaceRecDLib()
	{
		deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
		deserialize("dlib_face_recognition_resnet_model_v1.dat") >> net;

	}
	std::string face_descriptor_calc(std::string image_path);
	double descriptions_compare(std::string descriptors_from_foto, std::string descriptors_from_base);
	ResultVerify face_verification(std::string face_descriptors_string, std::string video_path, double level_verification);
	ResultVerify face_verification_threads(std::string face_descriptors_string, std::string video_path, double level_verification);
	ResultVerify compare_threads(std::vector<matrix<float, 0, 1>> face_descriptors_from_foto, std::vector<matrix<rgb_pixel>> faces, double level_verification);

private:
	shape_predictor sp;
	anet_type net;
	std::vector<matrix<float, 0, 1>> string_to_float(std::string s);
	std::string float_to_string(std::vector<matrix<float, 0, 1>> matr);
	//ResultVerify compare_threads(std::vector<matrix<float, 0, 1>> face_descriptors_from_foto, std::vector<matrix<float, 0, 1>> faces, double level_verification);
	//ResultVerify ress;
	frontal_face_detector detector = get_frontal_face_detector();
};

class C
{
public:
	void method1();

	void method2(int a, float b);
};

#endif // FACERECDLIB_H
