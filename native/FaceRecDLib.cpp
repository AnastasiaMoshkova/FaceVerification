#include "FaceRecDLib.h"
  
std::string FaceRecDLib::face_descriptor_calc(std::string image_path)
{
	//frontal_face_detector detector = get_frontal_face_detector();
	cv::Mat image_opencv;
	image_opencv = cv::imread(image_path);

	if ((image_opencv.rows < 150) || (image_opencv.cols < 150))
	{
		return "";
	}

	//matrix<rgb_pixel> img;

	//dlib::matrix<dlib::rgb_pixel> img(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(image_opencv)));
	matrix<rgb_pixel> img;
	load_image(img, image_path);

	// load image from image_path
	/*load_image(img, image_path);

	if ((img.nr() < 150) || (img.nc() < 150))
	{
		return "";
	}
*/

	std::vector<matrix<rgb_pixel>> faces;

	for (auto face : detector(img))
	{
		auto shape = sp(img, face);
		matrix<rgb_pixel> face_chip;
		extract_image_chip(img, get_face_chip_details(shape, 150, 0.15), face_chip);
		faces.push_back(std::move(face_chip));
	}

	if (faces.size() > 0)
	{
		std::vector<matrix<float, 0, 1>> face_descriptors = net(faces);

		//string for descriptors
		std::string descriptors_string;

		std::string str;
		//float fVal;
		//	str = toString(fVal);

		for (int i = 0; i < face_descriptors[0].size(); i++)
		{
			str = toString(face_descriptors[0](i));
			descriptors_string += str + ';';
			str.clear();
		}

		return descriptors_string;
	}
	else
	{
		return "";
	}
}

std::vector<matrix<float, 0, 1>> FaceRecDLib::string_to_float(std::string s) {

	matrix<float, 0, 1> matr;
	std::vector<matrix<float, 0, 1>> matr_vect;
	matr.set_size(128);
	int j = 0;
	std::string s1;
	std::vector<std::string> sv;
	std::vector<float> sf;
	float calc;

	for (int i = 0;i < s.size();i++)
	{
		if (s[i] != ';')
		{
			s1 = s1 + s[i];
		}
		if (s[i] == ';')
		{
			//sv.push_back(s1);
			//calc = fromString(s1);

			calc = fromString<float>(s1);

			sf.push_back(calc);
			matr(j) = calc;
			j++;

			s1.clear();
		}
	}

	matr_vect.push_back(matr);

	return matr_vect;
	//return sf;
}

double FaceRecDLib::descriptions_compare(std::string descriptors_from_foto, std::string descriptors_from_base) {


	auto face_descriptors_from_foto = string_to_float(descriptors_from_foto);
	auto face_descriptors_from_base = string_to_float(descriptors_from_base);

	double level;
	level = length(face_descriptors_from_foto[0] - face_descriptors_from_base[0]);
	
	return level;
}

std::string FaceRecDLib::float_to_string(std::vector<matrix<float, 0, 1>> face_descriptors)
{
	std::string descriptors_string;

	std::string str;
	//float fVal;
	//	str = toString(fVal);

	for (int i = 0; i < face_descriptors[0].size(); i++)
	{
		str = toString(face_descriptors[0](i));
		descriptors_string += str + ';';
		str.clear();
	}

	return descriptors_string;
}

ResultVerify FaceRecDLib::face_verification(std::string face_descriptors_string, std::string video_path, double level_verification)
{

	auto start3 = std::chrono::high_resolution_clock::now();

	ResultVerify res;
	matrix<rgb_pixel> img;

	//frontal_face_detector detector = get_frontal_face_detector();

	//std::vector<matrix<float, 0, 1>> face_descriptors_from_foto;
	auto face_descriptors_from_foto = FaceRecDLib::string_to_float(face_descriptors_string);

	//string line, line_img, line_mp4, foleder;
	std::string vector_for_identify;

	cv::VideoCapture cap(video_path);

	std::vector<matrix<float, 0, 1>> face_descriptors;

	int i = 0;

	while (cap.isOpened())
	{
		i = i + 1;
		cv::Mat tempIn, temp;
		if (!cap.read(tempIn))
		{
			break;
		}
		if (i % 3 == 0)
		{

			cv::resize(tempIn, temp, cv::Size(), 0.5, 0.5);

			dlib::matrix<dlib::rgb_pixel> img(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(temp)));

			std::vector<matrix<rgb_pixel>> faces;
			for (auto face : detector(img))
			{
				auto shape = sp(img, face);
				matrix<rgb_pixel> face_chip;
				extract_image_chip(img, get_face_chip_details(shape, 150, 0.15), face_chip);
				faces.push_back(std::move(face_chip));
			}

			std::cout << "faces.size() = " << faces.size() << " i = " << i << "\n" << std::endl;

			if (faces.size() > 1)
			{
				std::cout << "(faces.size() > 1) \n" << std::endl;
				res.num_face = faces.size();
				return res;
			}

			/*	if (faces.size() == 0)
			{
			std::cout << "(faces.size() == 0) \n" << std::endl;
			return res;
			}
			*/

			if (faces.size() == 1)
			{
				std::cout << "(faces.size() == 1) \n" << std::endl;
				std::vector<matrix<float, 0, 1>> face_descriptors_from_video = net(faces);

				double vector_level = length(face_descriptors_from_foto[0] - face_descriptors_from_video[0]);
				std::cout << vector_level << "\n" << std::endl;

				if (vector_level < level_verification)
				{
					res.level = vector_level;
					res.isVerify = true;

					std::cout << "time video processing verify\n" << std::endl;
					auto finish3 = std::chrono::high_resolution_clock::now();
					std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish3 - start3).count() << " ms" << std::endl;
					std::cout << "\n" << std::endl;

					return res;

				}
				else
				{
					auto vector_for_identify = FaceRecDLib::float_to_string(face_descriptors_from_video);

					//res.isVerify = false;
					res.data.push_back(vector_for_identify);
					//res.num_face = faces.size();
					//return res;
				}
			}

			//else {
			//	cout << "face not found\n" << endl;
			//}

		}


	}



	std::cout << "time video processing not verify\n" << std::endl;
	auto finish3 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish3 - start3).count() << " ms" << std::endl;
	std::cout << "\n" << std::endl;

	return res;

}

ResultVerify FaceRecDLib::compare_threads(std::vector<matrix<float, 0, 1>> face_descriptors_from_foto, std::vector<matrix<rgb_pixel>> faces, double level_verification)
{
	std::vector<matrix<float, 0, 1>> face_descriptors_from_video = net(faces);

	//ResultVerify res;
	//	ResultVerify_threads res;
	ResultVerify ress;

	double vector_level = length(face_descriptors_from_foto[0] - face_descriptors_from_video[0]);

	if (vector_level < level_verification)
	{

		ress.level = vector_level;
		ress.isVerify = true;
		//ress.data.push_back("true");
		return ress;
	}
	else
	{
		auto vector_for_identify = FaceRecDLib::float_to_string(face_descriptors_from_video);

		//res.isVerify = false;
		//res.data=vector_for_identify;
		//return res;
		//ress.data.push_back(vector_for_identify);
		ress.data_t = vector_for_identify;
	}
	return ress;
}

ResultVerify FaceRecDLib::face_verification_threads(std::string face_descriptors_string, std::string video_path, double level_ver)
{
	ResultVerify res, calc, calc_save;
	matrix<rgb_pixel> img;

	std::vector<std::future<ResultVerify>> fut_vec;

	//frontal_face_detector detector = get_frontal_face_detector();

	//std::vector<matrix<float, 0, 1>> face_descriptors_from_foto;
	auto face_descriptors_from_foto = FaceRecDLib::string_to_float(face_descriptors_string);

	//string line, line_img, line_mp4, foleder;
	std::string vector_for_identify;

	cv::VideoCapture cap(video_path);

	std::vector<matrix<float, 0, 1>> face_descriptors;

	int i = 0;

	while (cap.isOpened())
	{
		i = i + 1;
		cv::Mat tempIn, temp;
		if (!cap.read(tempIn))
		{
			break;
		}
		if (i % 3 == 0)
		{

			cv::resize(tempIn, temp, cv::Size(), 0.5, 0.5);

			dlib::matrix<dlib::rgb_pixel> img(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(temp)));

			std::vector<matrix<rgb_pixel>> faces;
			for (auto face : detector(img))
			{
				auto shape = sp(img, face);
				matrix<rgb_pixel> face_chip;
				extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip);
				faces.push_back(std::move(face_chip));
			}


			if (faces.size() > 0)
			{

				fut_vec.push_back(std::async(std::launch::async, &FaceRecDLib::compare_threads, this, face_descriptors_from_foto, faces, level_ver));

			}

			if (faces.size() > 1)
			{
				calc.num_face = faces.size();
				calc_save.isVerify == false;
				return calc;
			}

			//else {
			//	cout << "face not found\n" << endl;
			//}
		}

	}

	for (int j = 0; j < fut_vec.size(); ++j)
	{
		ResultVerify calc_save = fut_vec[j].get();

		if (calc_save.isVerify == true)
		{
			calc.isVerify = calc_save.isVerify;
			return calc;
		}
		if (calc_save.isVerify == false)
		{
			calc.isVerify = calc_save.isVerify;
			calc.data.push_back(calc_save.data_t);
		}

	}

	return calc;
}
