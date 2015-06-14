/*
 * FaceInfo.h
 *
 *  Created on: 22 Jan 2015
 *      Author: rprimas
 */

#ifndef CV_TASK2_FACEINFO_H_
#define CV_TASK2_FACEINFO_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "cfg_config.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace CgcvConfig;
using namespace cv;

class FaceInfo {
public:
	Mat face;
	Mat face_trans;
	Mat face_trimmed;
	Mat mask;

	Point left_eye;
	Point right_eye;
	Point mouth;

	String in_face_filename;
	String out_face_detect_filename;
	String out_face_mask_filename;
	String out_face_trans_filename;
	String out_face_trimmed_filename;

	FaceInfo(Mat mat, String iff,String ofdf, String ofmf,String oftf,String oftrif){
		face = mat;
		in_face_filename = iff;
		out_face_detect_filename = ofdf;
		out_face_mask_filename = ofmf;
		out_face_trans_filename = oftf;
		out_face_trimmed_filename = oftrif;
	}
        
        
        FaceInfo(Mat mat){
		FaceInfo(mat,"","","","","");
	}

	~FaceInfo();

	void setEyes(Point l,Point r){
		left_eye = l;
		right_eye = r;
	}

	void setMouth(Point m){
		mouth = m;
	}
};

#endif /* CV_TASK2_FACEINFO_H_ */
