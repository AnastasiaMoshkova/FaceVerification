#ifndef BASICTYPES_H
#define BASICTYPES_H

#define _CRT_SECURE_NO_WARNINGS

#define DLIB_USE_BLAS
#define DLIB_USE_LAPACK
#define DLIB_JPEG_SUPPORT
#define DLIB_PNG_SUPPORT

#include <fstream>
#include <string>
#include <iostream>
#include <future>
#include <thread>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <dlib/assert.h>
#include <dlib/image_processing.h>
#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/image_transforms.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/matrix/lapack/gesvd.h>
#include <dlib/matrix/lapack/gesdd.h>

// скорее всего это не нужно
//#include <windows.h>
//#include <stdio.h>

using namespace dlib;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = dlib::add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = dlib::add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET> using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128, avg_pool_everything<
    alevel0<
    alevel1<
    alevel2<
    alevel3<
    alevel4<
    max_pool<3, 3, 2, 2, relu<affine<con<32, 7, 7, 2, 2,
    input_rgb_image_sized<150>
    >> >> >> >> >> >>;

template <typename T>
inline std::string toString(T val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

template<typename T>
inline T fromString(const std::string& s)
{
    std::istringstream iss(s);
    T res;
    iss >> res;
    return res;
}

template<typename T>
inline void getLogLine(std::stringstream& stream, const T& a){
    stream << a;
}

template<typename T, typename ... Types>
inline void getLogLine(std::stringstream& stream, const T& a, Types... args){
    stream << a;
    getLogLine(stream, args...);
}

template<typename ... Types>
inline std::string getLogString(Types... args){
    std::stringstream s;
    getLogLine(s, args...);
    return s.str();
}

inline std::string getTimeStamp()
{
    time_t t = time(0);   // get time now
    struct tm now;
    localtime_s(&now, &t);

    return getLogString(now.tm_year + 1900, "/", now.tm_mon + 1, "/", now.tm_mday, " ", now.tm_hour, ":", now.tm_min, ":", now.tm_sec);
}

#endif // BASICTYPES_H
