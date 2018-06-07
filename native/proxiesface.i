/* File : example.i */
%module facerecognition

%include "std_vector.i"
%include "std_string.i"
%{
#include "FaceRecDLib.h"
%}

%include "FaceRecDLib.h"

namespace std {
	%template(ListString) vector<string>;
}
