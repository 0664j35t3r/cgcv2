#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "FaceInfo.h"
#include "cfg_config.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;
using namespace CgcvConfig;
using namespace cv;

#define FULL_VERSION 1
#define DEBUG 0
#define PI 3.14159265

typedef struct DetectorParam
{
  float scaleFactor;
  int neighbours;
  int flag;
  Size minSize;
} DetectorParam;

typedef struct TrainingData
{
	Mat features;
	Mat labels;
	Size patch;
	string svmModelFile;
	char padding;
	char step;
	
	vector<char> landmarks;
} TrainingData;

typedef struct SvmParameter {
	CvSVMParams cvParams;
	float scale;
	int   sizeOfSet;
} SvmParameter;

typedef struct Detection {
	int  scoreNum;
	Size scale;
	Size patch;
	string svmModelFile;
} Detection;

DetectorParam face_params;
DetectorParam eyes_params;
DetectorParam mouth_params;

//==============================================================================
// getHogFeatures()
//------------------------------------------------------------------------------
// TODO:
//      - create openCV HoG descriptor object
//      - define descriptor window size using given parameter
//      - define block size: 16x16, cell size 8x8
//      - for all given patches:
//        - use your descriptor object to compute features
//        - write all features in one matrix, having one row for one feature vector
//
// vector<Mat> patches: list of patches to extract from
// Mat features: extracted feature matrix to return (per reference)
// Size win: hog descriptor window size
//================================================================================

void getHogFeatures(const vector<Mat>& patches, Mat& features, Size win)
{
  cv::HOGDescriptor hog;
  hog.winSize = win;
  hog.blockSize.height = 16;
  hog.blockSize.width  = 16;
  hog.cellSize.height  =  8;
  hog.cellSize.width   =  8;
  hog.blockStride = Size(8,8);

  vector<float> a;
  for (const auto& patch : patches)
  {
    hog.compute(patch, a);
    Mat matrix(a, true);
    transpose(matrix, matrix);
    features.push_back(matrix);
  }
}

//================================================================================
// detect()
//--------------------------------------------------------------------------------
// TODO:
// - create SVM object using cv::CvSVM and load svm model (params.svmModelFile)
// - resize grayscaled face image to size given by params.scale

// - distinguish 3 cases according to search value:
// - if 0: search in mouth region, if 1: left eye, if 2: right eye
// - search through whole image using a sliding window with window size params.patch
// - extract hog features of each ROI using getHogFeatures()
// - use SVM object for getting distance to SVM hyperplane (cv::CvSVM::predict())
// - apply sigmoid function to returned distance
// - collect a certain number of your best scored ROIs (params.scoreNum)
// - that list of should be sorted starting with highest score
// - apply post processing by intersecting ROIs
//   - start with first and second, keep resulting ROI
//   - intersect that ROI with next one and so on
//   - neglect resulting ROIs having less than 80% area of the previous
// - scale back that ROI to fit with original image size
// - center of last ROI is your detected object's position
// - HINT: use of cv::Rect::operator &() could be useful
//
// Mat& face: image to search in
// Point2f& detected: position of detected object to be returned (per reference)
// char search: defines searching area
// Detection& params: detection parameter
//================================================================================

void detect(const Mat& face, Point2f& detected, char search, const Detection& params)
{
  Mat imag = face;
  CvSVM cvsvm;
  cvsvm.load(params.svmModelFile.c_str());

  cvtColor(imag, imag, CV_BGR2GRAY);
  imag.resize(params.scale.height);

  Rect a;
  // - if 0: search in mouth region, if 1: left eye, if 2: right eye
  switch(search)
  {
    case 1: // eyes
      cvRect(0, 0, imag.cols * 0.5, imag.rows * 0.6);  // todo width, height
      break;
    case 2:
      cvRect(imag.cols * 0.5, 0, imag.cols * 0.5, imag.rows * 0.6);
      break;
    default: // Mund  // lower 40%
      cvRect(0, imag.rows * 0.6, imag.cols, imag.rows * 0.4);  // todo width, height
      break;
  }

  Mat vec_feature;
  Rect intersec;
  // - search through whole image using a sliding window with window size params.patch
  for (int y = 0; y < imag.rows - params.patch.height; y = y + params.patch.height)
  {
    for (int x = 0; x < imag.cols - params.patch.width; x = x + params.patch.width)
    {
      vector<Mat> temp;
      temp.push_back(imag(Rect(x,y, params.patch.width, params.patch.height)));

      // - extract hog features of each ROI using getHogFeatures()
      getHogFeatures(temp, vec_feature, params.patch);

      // - use SVM object for getting distance to SVM hyperplane (cv::CvSVM::predict())
      float dist = cvsvm.predict(vec_feature, true);

      // - apply sigmoid function to returned distance
      dist = 1/(1 + exp(-dist)); // todo check if -d

      // - collect a certain number of your best scored ROIs (params.scoreNum)
      vector<pair<float, Rect>> vec_score;
      vec_score.push_back(pair<float, Rect>(dist, Rect(x, y, params.patch.width, params.patch.height)));

      // - collect a certain number of your best scored ROIs (params.scoreNum)
      struct sorting : public binary_function<pair<float,Rect>, pair<float,Rect>, bool>
      {
        bool operator()(const pair<float,Rect>& __x, const pair<float,Rect>& __y) const
        {
          return __x.first > __y.first;
        }
      };

      std::sort (vec_score.begin(), vec_score.end(), sorting());
      for (auto item : vec_score)
        cout << item.first << endl;

      while(vec_score.size() > params.scoreNum)
        vec_score.pop_back();

      // - apply post processing by intersecting ROIs
      //    - start with first and second, keep resulting ROI
      //    - intersect that ROI with next one and so on
      //    - neglect resulting ROIs having less than 80% area of the previous
      intersec = vec_score[0].second;
      Rect a;
      for (int i = 1; i < vec_score.size(); i++)
      {
        a = intersec & vec_score[i].second;
        if(a.area() * 0.8 < intersec.area() * 0.8) // s12 80 prozent

        intersec = intersec & vec_score[i].second;
      }

      // - scale back that ROI to fit with original image size
      // - center of last ROI is your detected object's position
      float area_param = params.scale.height * params.scale.width;
      float area_scale = intersec.height * intersec.width;
      float new_scale = area_scale / area_param;

      intersec.height = intersec.height * new_scale;
      intersec.width  = intersec.width * new_scale;
      cout << "x " << intersec.x << "y " << intersec.y << endl;
//      detected = Point2f(intersec.x, intersec.y); // needed in function maskFace()
    }
  }
  detected = Point2f(intersec.x, intersec.y);
//  detected = Point2f(6, 7); //
}

bool fExists(const std::string& filename) {
  ifstream ifile(filename.c_str());
  return ifile;
}

//================================================================================
// readLandmarks()
//--------------------------------------------------------------------------------
// DO NOT CHANGE ANYTHING HERE!
//  - just use it!
//
// string fileName: file name of image the landmarks accord to
// vector<Point2f>& landmarks: returned list of landmark points (per reference)
//================================================================================

void readLandmarks(string fileName, vector<Point2f>& landmarks) {
	fileName = fileName + ".txt";
	ifstream file;
	file.open(fileName.c_str());
	
	string line;
	string xStr, yStr;
	Point2f pt;
	
	while(getline(file, line)) {
		
		size_t pos = line.find(' ');
		line.erase(0, pos+1);
		
		pos = line.find(' ');
		xStr = line.substr(0, pos);
		
		line.erase(0, pos+1);
		yStr = line;
		
		pt = Point2f(atof(xStr.c_str()), atof(yStr.c_str()));
		landmarks.push_back(pt);
	}
}

void readImageList(string fileName, vector<string>& fileList, vector<string>& maskList) {
	ifstream file;
	file.open(fileName.c_str());
	
	string line;
	
	while(getline(file, line)) {
		fileList.push_back("data/training_data/" + line);
		maskList.push_back("data/training_data/masks/" + line);
	}
}


//================================================================================
// trainSVM()
//--------------------------------------------------------------------------------
// TODO: 
// - read in training images (number is given with svmParams.sizeOfSet)
// - read in fitting masks
// - read in fitting landmarks (see function for info how to use)
// - for all images
//   - resize (factor svmParams.scale) grayscaled image and mask
//   - extract landmarks: indices for needed marks stored in TrainingData::landmarks (see below)
//   - calculate eye centers and mouth center scaled according to image scale
//   - use centers to define ROIs around according to size TrainingData::patch

// - collect all negative and positive training features for both mouth and eyes:
//   - positive: eye and mouth ROIs using getHogFeatures()
//   - negative: TrainingData::patch sized ROIs
//   - negative: extract hog features of patch with step size TrainingData::steps
//   - negative: use mask to only extract features of patches in totally valid areas
//     - HINT: use of cv::countNonZero could be very useful
//   - negative: do not enter the area of the positive ROIs totally just within TrainingData::padding pixels
// - make matrix of training features for both eyes and mouth including all collected data
// - make label matrix with value 1 for positive and 2 for negative features
// - create two SVM objects using cv::CvSVM
// - train both SVMs
// - store both trained models - use TrainingData::svmModelFile as file names
//
// landmarks[0]; Index for left eye left corner OR mouth
// landmarks[1]; Index for left eye right corner
// landmarks[2]; Index for right eye left corner
// landmarks[3]; Index for right eye right corner
//
// parameters:
// TrainingData Eyes: parameters for eye training
// TrainingData Mouth: parameters for mouth training
// vector<string>& imageList: list of training file names
// vector<string>& imageList: list of mask file names
// MSvmParameter svmParams: parameters for svm learning
//
// return: void
//================================================================================

void trainSVM(TrainingData Eyes, TrainingData Mouth, vector<string>& imageList, vector<string>& maskList, const SvmParameter svmParams) {
  cout << "trainSVM" << endl;
  for (int i = 0; i < svmParams.sizeOfSet; i++)
  {
    Mat imag = imread(imageList.at(i));

    cv::cvtColor(imag, imag, CV_BGR2GRAY);
    resize(imag, imag, Size(svmParams.scale * imag.cols, svmParams.scale * imag.rows));

    //  readLandmarks()
    Mat mask = imread(maskList.at(i));
    cv::cvtColor(mask, mask, CV_BGR2GRAY);
    resize(mask, mask, Size(svmParams.scale * mask.cols, svmParams.scale * mask.rows));

    vector<Point2f> landmarks;
    readLandmarks(imageList.at(i), landmarks);

    Point center_mouth           = landmarks.at(Mouth.landmarks.at(0)) * svmParams.scale;
    Point left_eye_left_corner   = landmarks.at(Eyes.landmarks.at(0));
    Point left_eye_right_corner  = landmarks.at(Eyes.landmarks.at(1));
    Point right_eye_left_corner  = landmarks.at(Eyes.landmarks.at(2));
    Point right_eye_right_corner = landmarks.at(Eyes.landmarks.at(3));

    Point center_left_eye  = (left_eye_left_corner + left_eye_right_corner) * 0.5 * svmParams.scale;
    Point center_right_eye = (right_eye_left_corner + right_eye_right_corner) * 0.5 * svmParams.scale;

    Point top_left_left_eye;
    top_left_left_eye.x = center_left_eye.x - (Eyes.patch.width / 2);
    top_left_left_eye.y = center_left_eye.y - (Eyes.patch.height / 2);

    Point top_left_right_eye;
    top_left_right_eye.x = center_right_eye.x - (Eyes.patch.width / 2);
    top_left_right_eye.y = center_right_eye.y - (Eyes.patch.height / 2);

//    cout << imag.size() << endl;
    Point top_left_mouth;
    top_left_mouth.x = center_mouth.x - (Mouth.patch.width / 2);
    top_left_mouth.y = center_mouth.y + (Mouth.patch.height / 2);

    // left corner and size
    Rect ROI_eyes_left (top_left_left_eye, Eyes.patch);
    Rect ROI_eyes_right (top_left_right_eye, Eyes.patch);
    Rect ROI_mouth (top_left_mouth, Mouth.patch);

    cout << center_left_eye << endl;
    circle(imag, center_left_eye , 3, Scalar(255,255,255));
    circle(imag, center_right_eye, 3, Scalar(255,255,255));
    circle(imag, center_mouth    , 3, Scalar(255,255,255));
    imshow("hallo", imag);
    //    waitKey(0);
    //    ROIs sind Rects, die Bereiche deines Bildes definieren sollen das eine Mat ist. Die HOG-Features
    //    sind aus den ROIs der Bilder zu berechnen, also wie du sagst: Bilausschnitte herauskopieren und getHOGFeatures füttern.

    //    Die positiven Trainingsdaten sind die HOG-Features jener Bereiche von denen du weißt das sie
    //    die gewünschten Informationen enthalten. Beim Auge also beispielsweise jene Bereiche die Augen enthalten
    //    (welche das sind kannst du ja aus dem text file auslesen). Selbes gilt für den Mund.
    //    Damit kannst du zwei separate SVM
    //    's trainieren die zwischen Auge und nicht Auge, bzw. Mund und nicht Mund unterscheiden können (basierend auf den
    //    HOG-Features).
    //    Zu den wenigen positiven Beispielen generierst du noch viele Negativbeispiele, die ALLE übrigen sich überlappenden
    //    Bereiche des Bildes darstellen. Dabei sind Bereiche in jeweils step_size Abstand (sowohl vertikal, als auch horizontal)
    //    zu nehmen. Also einfach mit einem Fenster über das gesamte Bild sweepen (wobei sich der Bereich immer nur um
    //    step_size in eine Richtung weiterbewegt), und nimmst alle Bereiche DIE WEDER DAS GESUCHTE OBJEKT beinhalten
    //    NOCH AUSSERHALB DES BILDBEREICHES liegen (definiert durch die Maske). ZUSAETZLICH ist noch der Bereich um die
    //    Augen um padding erweitert, sowie um den Mund um padding vermindert Tabu. Wenn dein Fenster beim sweepen
    //    irgendwie in so einen Bereich kommt, ist er zu ignorieren. Von den damit übrigen negativen Beispielen berechnest
    //    du dir ebenfalls die HOG-Features um deine negativen Beispiele zu erhalten.
    //    Damit die SVM beim trainieren weiß was positiv und was negativ ist, gibst du ihre diese Information
    //    noch mit den Labels mit. Das ist einfach ein Vektor der 1 für positive und 2 für negative Beispiele enthaltet.
    //    Habe auch ewig und drei Tage gebraucht um zu verstehen was genau zu tun ist...

    //  - positive: eye and mouth ROIs using getHogFeatures()
    vector<Mat> eye;
    vector<Mat> mouth;
    eye.push_back(imag(ROI_eyes_left)); // subpic
    eye.push_back(imag(ROI_eyes_right));
    mouth.push_back(imag(ROI_mouth));
    getHogFeatures(eye, Eyes.features, Eyes.patch);
    Eyes.labels.push_back(1);
    Eyes.labels.push_back(1);
    getHogFeatures(mouth, Mouth.features, Mouth.patch);
    Mouth.labels.push_back(1);

    //  - negative: TrainingData::patch sized ROIs
    Rect intersec;
    for(int i; i < imag.cols - Eyes.patch.width; i += Eyes.step)
    {
      for (int j = 0; j < imag.rows - Eyes.patch.height; j += Eyes.step)
      {
        Rect tmp(Point(i,j), Eyes.patch);
        intersec = tmp & ROI_eyes_left;
        if(intersec.area() > 0)
          continue;
        intersec = tmp & ROI_eyes_right;
        if(intersec.area() > 0)
          continue;

        if(countNonZero(mask(tmp)) != tmp.area()) // ob die maske weiß ist
          continue;
        std::vector<Mat> vec_tmp;
        vec_tmp.push_back(imag(tmp));
        getHogFeatures(vec_tmp, Eyes.features, Eyes.patch);
        Eyes.labels.push_back(2);
      }
    }
    for(int i; i < imag.cols - Mouth.patch.width; i += Mouth.step)
    {
      for (int j = 0; j < imag.rows - Mouth.patch.height; j += Mouth.step)
      {
        Rect tmp(Point(i,j), Mouth.patch);
        intersec = tmp & ROI_mouth;
        if(intersec.area() > 0)
          continue;
//        cout << "6 loop" << endl;
        if(countNonZero(mask(tmp)) != tmp.area()) // ob die maske weiß ist
          continue;
//        cout << "7 loop" << endl;
        std::vector<Mat> vec_tmp;
        vec_tmp.push_back(imag(tmp));
        getHogFeatures(vec_tmp, Mouth.features, Mouth.patch);
        Mouth.labels.push_back(2);
      }
    }
  }

  // - create two SVM objects using cv::CvSVM
  CvSVM eye;
  CvSVM mouth;

  // - train both SVMs
  eye.train(Eyes.features, Eyes.labels);
  mouth.train(Mouth.features, Mouth.labels);

  // - store both trained models - use TrainingData::svmModelFile as file names
  eye.save(Eyes.svmModelFile.c_str());
  mouth.save(Mouth.svmModelFile.c_str());
}

//================================================================================
// detectFace()
//--------------------------------------------------------------------------------
// TODO:
//      - define a classifier (CascadeClassifier)
//      - convert the input image to a gray image (cvtColor)
//      - use the function equalizeHist to equalize the colors and brightness of
//        image
//      - use the first classifier to detect the face in the image
//      - from the found faces take first face
//      - store a ROI containing the found face to cv::Rect faceROI
//
// Mat& frame: the current image to work on
// Rect& faceROI: the ROI containing the face to store
//================================================================================

void detectFace(const Mat& frame, Rect& faceROI)
{
  if (frame.empty())
  {
    cout << "No frame_grayture!" << endl;
    return;
  }

  CascadeClassifier face_cascade;
  Mat frame_gray;
  faceROI = Rect(0, 0, 15, 15);
  String face_cascade_name = "haarcascade_frontalface_alt.xml";

  if( !face_cascade.load( face_cascade_name ) )
  {
    std::cout << "--(!)Error loading" << std::endl;
    return;
  }

  std::vector<Rect> faces;
  cv::cvtColor(frame, frame_gray, CV_RGB2GRAY);
  cv::equalizeHist(frame_gray, frame_gray);

  face_cascade.detectMultiScale(frame_gray, faces, face_params.scaleFactor, face_params.neighbours, face_params.flag, face_params.minSize); // todo refactor

//  for(ulong iter = 0; iter < faces.size(); iter++)
//  {
//    Point center( faces[iter].x + faces[iter].width*0.5, faces[iter].y + faces[iter].height*0.5 );
//    cv::ellipse( (Mat &) frame, center, Size( faces[iter].width*0.5, faces[iter].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
//    Mat faceROI = frame_gray( faces[iter] );
//  }
}

void detectEyesMouth(const Mat& frame, const Rect& face, Point& left_eye, Point& right_eye, Point& mouth, const Detection& eyes_data, const Detection& mouth_data) {

  cout << "detect Eyes Mouth " << endl;
  Mat frameClone = frame.clone();

  Point2f eye1, eye2, mouth1;
	detect(frame(face), eye2, 2, eyes_data); // right eye
	detect(frame(face), mouth1, 0, mouth_data); // mouth
	detect(frame(face), eye1, 1, eyes_data); // left eye

	left_eye = Point(face.x + floor(eye1.x), face.y + floor(eye1.y));
	right_eye = Point(face.x + floor(eye2.x), face.y + floor(eye2.y));
	mouth = Point(face.x + floor(mouth1.x),  face.y + floor(mouth1.y));
}


//================================================================================
// maskFace()
//--------------------------------------------------------------------------------
// TODO: 
// 1) get eyes_center point by calculating the mean value of left/right eye's x/y coordinates
// 2) get eyes_distance by calculating the norm of a vector pointing from left eye to right eye
// 3) get eyescenter_mouth_distance by calculating the norm of a vector pointing from mouth to eyes_center
// 4) get rotation of face by calculating angle between vector pointing from left eye to right eye and horizontal line
// 5) draw ellipses with opencv and set these parameters accordingly: color=Scalar(255, 255, 255), thickness=-1, lineType=8, shift=0
//
// NOTE: use PI as value for pi when converting radians to degrees
//
// Mat& mask: the black image where you should draw the ellipses
// const Point left_eye: the coordinates of the left eye
// const Point right_eye: the coordinates of the right eye
// const Point mouth_center: the coordinates of the mouth
//================================================================================

void maskFace(Mat& mask, const Point left_eye, const Point right_eye, const Point mouth_center, float S_1, float S_2)
{
  //todo
  Point le = left_eye;
  Point re = right_eye;
  Point mc = mouth_center;
//  le.x = 1;
//  le.y = 2;
//  re.x = 3;
//  re.y = 4;
//  mc.x = 5;
//  mc.y = 6;

  // 1) get eyes_center point by calculating the mean value of left/right eye's x/y coordinates
  Point mean;
  mean.x = (le.x + re.x) / 2;
  mean.y = (le.y + re.y) / 2;

  // 2) get eyes_distance by calculating the norm of a vector pointing from left eye to right eye
  float dist_eyes = sqrt( pow(le.x - re.x, 2) + pow(le.y - re.y,2));

  // 3) get eyescenter_mouth_distance by calculating the norm of a vector pointing from mouth to eyes_center
  // hessesche normalform
  Point vec_eye_mean;
  vec_eye_mean.x = (le.x - re.x) / 2;
  vec_eye_mean.y = (le.y - re.y) / 2;

  float dist_mouth_eyes = sqrt( pow(vec_eye_mean.x - mouth_center.x, 2) + pow(
          vec_eye_mean.y - mc.y,2) );

  // 4) get rotation of face by calculating angle between vector pointing from left eye to right eye and v_horizontal line
  Vec2f v_horizontal (1, 0);

  Vec2f v_eyes;
  v_eyes[0] = re.x - le.x;
  cout << "l_e" << re << endl;
  cout << "l_e" << le << endl;
  v_eyes[1] = re.y - le.y;
  float norm = sqrt(v_eyes[0] * v_eyes[0] + v_eyes[1] * v_eyes[1]);

  normalize(v_horizontal, v_horizontal);
  normalize(v_eyes, v_eyes);

  float theta = acos(v_horizontal.dot(v_eyes) / norm) * 180 / PI; // todo radians
  cout << "theta " << theta << endl;

  //  cout << "mask " << mask.size() << endl;

  // 5) draw ellipses with opencv and set these parameters accordingly: color=Scalar(255, 255, 255), thickness=-1, lineType=8, shift=0
  // color=Scalar(255, 255, 255), thickness=-1, lineType=8, shift=0
  ellipse(mask, mc, Size(dist_eyes,dist_mouth_eyes), theta, 0, 0, Scalar(255,255,255), -1, 8, 0); // todo
  imshow("ellipse", mask);
//  waitKey(0);
  cout << " leave mask" << endl;
}


//================================================================================
// affineTransform()
//--------------------------------------------------------------------------------
// TODO: 
// 1) use opencv to calculate a transformation matrix that maps the three given points from one image into another image
// 2) apply the calculated transformation on imageToTransform. The size of the image must not change
// 3) write the calculated transformation matrix to T
//
// parameters:
// Mat imageToTransform: the image to transform
// Point left_eye_one: left eye coords of imageToTransform
// Point right_eye_one: right eye coords of imageToTransform
// Point mouth_one: mouth coords of imageToTransform
// Point left_eye_two: left eye coords of second image
// Point right_eye_two: right eye coords of second image
// Point mouth_two: mouth coords of second image
// Mat& T: save the calculated transformation matrix to this variable
//
// return: Mat: the transformed image
//================================================================================

Mat affineTransform(Mat imageToTransform, Point left_eye_one, Point right_eye_one, Point mouth_one, Point left_eye_two, Point right_eye_two, Point mouth_two, Mat& T)
{
  cout << " Affine Transformat" << endl;
  // 1) use opencv to calculate a transformation matrix that maps the three given points from one image into another image
  // http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/warp_affine/warp_affine.html
  Mat src, warp_dst;

  Point2f one[3];
  one[0] = left_eye_one;
  one[1] = right_eye_one;
  one[2] = mouth_one;

  Point2f two[3];
  two[0] = left_eye_two;
  two[1] = right_eye_two;
  two[2] = mouth_two;

  src = getAffineTransform(one, two);
//  cout << " Affine Transformat4" << src.size() << endl;
//  cout << " Affine Transformat4" << imageToTransform.size() << endl;

  // 2) apply the calculated transformation on imageToTransform. The size of the image must not change
  warpAffine(imageToTransform, warp_dst, src, warp_dst.size());

  // 3) write the calculated transformation matrix to T
  T = src;
  return warp_dst;
}

//================================================================================
// affineTransformVertices()
//--------------------------------------------------------------------------------
// TODO:
// 1) transform the four vertices of the given image by using the previously calculated transformation matrix T.
// 2) return a vector containing the four transformed vertices in the following order:
//    up/left, up/right, bottom/left, bottom/right
//
// parameters:
// Mat image: the image whose vertices should be transformed
// Mat T: the transformation matrix
//
// return: vector<Point2f>: the transformed image vertices
//================================================================================
vector<Point2f> affineTransformVertices(Mat image, Mat& T)
{
  // 1) transform the four vertices of the given image by using the previously calculated transformation matrix T.
  T.convertTo(T, CV_32FC1);
  int width = image.cols;
  int height = image.rows;

  Mat E(3,4, DataType<float>::type);

  E.at<float>(0,0) =  0;//ey1
  E.at<float>(1,0) =  0;//ex1
  E.at<float>(2,0) =  1;//
  E.at<float>(0,1) =  0;//ey2
  E.at<float>(1,1) =  width;//ex2
  E.at<float>(2,1) =  1;//
  E.at<float>(0,2) =  height;//ey3
  E.at<float>(1,2) =  0;//ex3
  E.at<float>(2,2) =  1;//
  E.at<float>(0,3) =  height;//ey4
  E.at<float>(1,3) =  width;//ex4
  E.at<float>(2,3) =  1;//

  //  cout << "a  >>>" <<  E.size() << endl;
  //  cout << "a  >>>" <<  E.cols << endl;
  //  cout << "T  >>>" <<  T.size() << endl;
  //  cout << "T  >>>" <<  T.cols << endl;
  E.mul(T);

  vector<Point2f> tmp;
  tmp.push_back(Point2f(E.at<float>(1,0), E.at<float>(0,0)));
  tmp.push_back(Point2f(E.at<float>(1,1), E.at<float>(0,1)));
  tmp.push_back(Point2f(E.at<float>(1,2), E.at<float>(0,2)));
  tmp.push_back(Point2f(E.at<float>(1,3), E.at<float>(0,3)));

  // 2) return a vector containing the four transformed vertices in the following order:
  //    up/left, up/right, bottom/left, bottom/right
  return tmp; // return vector<Point2f>(4, Point(1,2));
}

//================================================================================
// calculateRect()
//--------------------------------------------------------------------------------
// DO NOT CHANGE ANYTHING HERE!
//================================================================================
vector<Point2f> calculateRect(vector<Point2f> points)
{
  cout << " calculate Rect" << endl;
    float left_border = max(points[0].x, points[2].x);
    float right_border = min(points[1].x, points[3].x);
    float top_border = max(points[0].y, points[1].y);
    float bottom_border = min(points[2].y, points[3].y);
    points[0] = Point2f(left_border, top_border);
    points[1] = Point2f(right_border, top_border);
    points[2] = Point2f(left_border, bottom_border);
    points[3] = Point2f(right_border, bottom_border);
    return points;
}

//================================================================================
// distTransform()
//--------------------------------------------------------------------------------
// TODO: 
// - create a structuring element of size 3x3; openCV constant: MORPH_ELLIPSE
// - dilate the inverted mask 5 times
// - apply distance transform algorithm
//   - erode mask as long there are white elements
//   - count number of erosion for each element that is not black yet
//   - HINT: use of cv::countNonZero could be useful
// - normalize resulting mask
// - invert again
// - calculate squared pixel values
// - normalize again
//
// parameters:
// Mat face_one: source mask
// Mat mask_one: final mask to be returned (per reference)
//
// return: void
//================================================================================

void distTransform(const Mat& src, Mat& dest) {

  cout << " distransform" << endl;
	// comment out or delete the next line if you want to do the bonus:
//	cv::distanceTransform(src, dest, CV_DIST_C, CV_DIST_MASK_PRECISE);
	cv::distanceTransform(src, dest, CV_DIST_C, 3);
	// --- your BONUS code here:
}

//================================================================================
// blendFaceSequence()
//--------------------------------------------------------------------------------
// TODO:
// 1) calculate imageCount and alpha
// 2) calculate the alpha blended mask and transition image
// 3) do a distance transformation with openCV
//    - set distanceType = CV_DIST_C
//    - set maskSize = 3
// 4) calculate alpha-blended mask with gauss weighting (format: CV_32FC1)
//    - sigma = 11
// 5) blure the calculated image
// 6) calculate the final image using the transition image, the blured image
//    and the alpha-blended mask
//
// parameters:
// vector<FaceInfo*> faces: all information about the image
// int transitionTime: duration of transition from one image in faces to the next image.
// int fps: frames per second
//
// return:
// vector<Mat> - Vector of all calculated transition Images
//================================================================================
vector<Mat> blendFaceSequence(vector<FaceInfo*> faces, int transitionTime, int fps)
{
  cout << " blendFaceSequence" << endl;

 // 1) calculate imageCount and alpha
  int imagecount = (transitionTime/1000) * fps;

//  cout << faces[0]->face << endl;
//  cout << faces[0]->face_trans << endl;
//  cout << faces[0]->face_trimmed << endl;

  vector<Mat> r;
  Mat final = Mat(faces[1]->face.rows,faces[1]->face.cols, CV_32FC1);
  Mat blur = Mat(faces[1]->face.rows,faces[1]->face.cols, CV_32FC1);
  // 2) calculate the alpha blended mask and transition image
  for (int i = 0; i < faces.size() - 1; i++)
  {
    // 2) calculate the alpha blended mask and transition image
    for (int j = 1; j <= 4; j++)
    {
      float alpha = j / imagecount;
      //  faces[i]->face_trans = (1 - alpha) * faces[i]->face + alpha * faces[i + 1]->face;

      // 3) do a distance transformation with openCV
      //    - set distanceType = CV_DIST_C
      //    - set maskSize = 3
      cout << "before" << endl;
      distTransform(faces[i]->mask, faces[i]->mask);
      cout << "after" << endl;

      // 4) calculate alpha-blended mask with gauss weighting (format: CV_32FC1)
      //    - sigma = 11
      for (int k = 0; k < faces[i]->mask.cols; k++)
      {
        for (int l = 0; l < faces[i]->mask.rows; l++)
        {
          faces[i]->mask.at<int>(k,l) = 1.f - exp(-((faces[i]->mask.at<int>(k,l),2) / (2 * (11 * 11))));
        }
      }
      cout << " aaaaaaaaaaaaaaaaaaaa" << endl;
      // 6) calculate the final image using the transition image, the blured image
      //    and the alpha-blended mask
      GaussianBlur(faces[i]->face_trans, blur,Size(11, 11), 11, 11);
      // Mat blur = GaussianBlur(faces[i]->face_trans.at<int>(k,l), faces[i]->face_trans.at<int>(k,l),Size(11, 11), 11, 11);
      for (int k = 0; k < faces[i]->mask.cols; k++)
      {
        for (int l = 0; l < faces[i]->mask.rows; l++)
        { //cout << " ber" << endl;
          final.at<int>(k, l) = faces[i]->mask.at<int>(k, l) * faces[i]->face_trans.at<int>(k,l) + (1 - faces[i]->mask.at<int>(k, l)) * blur.at<int>(k,l);
        }
      }
    }
    r.push_back(final);
  }



  return r;
}

//================================================================================
// createVideo()
//--------------------------------------------------------------------------------
// TODO:
// 1) create VideoWriter
// 		- FORCC = PIM1
// 2) write the images in frames to the VideoWriter
// 3) release the VideoWriter
//
// parameters:
// vector<Mat> frames: all images of the video
// int fps: frames per second
// string video_dir: directory to store the video
// Size size: size of the video
//
//================================================================================
void createVideo(vector<Mat> frames, int fps, string video_dir, Size size)
{
  cout << " create video " << endl;
  VideoWriter videoWriter(video_dir, CV_FOURCC('P', 'I','M', '1'), fps, size);
  for(const auto& frame : frames)
    videoWriter.write(frame);
  videoWriter.release();
}


//================================================================================
// main()
//--------------------------------------------------------------------------------
// TODO:
//	- Nothing!
//	- Do not change anything here
//================================================================================

int main(int argc, char *argv[])
{
    printf("CV/task2 framework version 1.0\n"); // DO NOT REMOVE THIS LINE!!!

    if (argc != 2)
    {
      cout << "Usage: ./cvtask2 <config-file>\n" << endl;
      return -1;
    }

    try
    {
        cout << "reading config..." << endl;
        cout << "OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << endl;
		
        // load config
        Config* cfg = Config::open(argv[1]);

        //load parameters
        string output_dir = cfg->getAttribute<string>("output_dir");
        string video_out = cfg->getAttribute<string>("video_out");
        string s_fps = cfg->getAttribute<string>("fps");
        string s_transform_time = cfg->getAttribute<string>("transform_time");
        string imagesequence = cfg->getAttribute<string>("face_sequence_filename");
        string outlier_file = cfg->getAttribute<string>("outliers");
        string detect_face_string = cfg->getAttribute<string>("detect_face");
        
        #if defined(_WIN32)
            _mkdir("./output/");
            _mkdir(output_dir.c_str());
        #else 
            mkdir("./output/", 0777); 
            mkdir(output_dir.c_str(), 0777); 
        #endif

        int fps = atoi(s_fps.c_str());
        int transform_time = atoi(s_transform_time.c_str());


		// load svm parameters
		Container* cfgSvm = cfg->getContainer("svm_training", 0);
		string training_dir = cfgSvm->getAttribute<string>("dir");
		float training_scale = cfgSvm->getContainer("img", 0)->getAttribute<float>("scale");
		int training_set_size = cfgSvm->getContainer("img", 0)->getAttribute<int>("number");
		
		// detection parameters
		int num_of_scores = cfg->getAttribute<int>("best_score_num");
		
		// see more output
		int test_svm = cfg->getAttribute<int>("test_svm");
		int test_hog = cfg->getAttribute<int>("test_hog");
		int test_bonus = cfg->getAttribute<int>("test_bonus");

        // load images
        vector<string> iff = cfg->getMultiAttribute<string>("in_face_filename");
        vector<string> ofdf = cfg->getMultiAttribute<string>("out_face_dedect_filename");
        vector<string> ofmf = cfg->getMultiAttribute<string>("out_face_mask_filename");
        vector<string> oftf = cfg->getMultiAttribute<string>("out_face_trans_filename");
        vector<string> oftrif = cfg->getMultiAttribute<string>("out_face_trimmed_filename");

        // create face infos
        vector<FaceInfo*> faces;
        
        for (int i = 0; i < iff.size(); i++)
        {
            Mat face = imread(iff[i]);
            faces.push_back(new FaceInfo(face, 
                                         iff[i], 
                                         ofdf[i], 
                                         ofmf[i], 
                                         oftf[i], 
                                         oftrif[i]));
        }

        face_params.scaleFactor = 1.1;
        face_params.neighbours = 2;
        face_params.flag = CV_HAAR_SCALE_IMAGE;
        face_params.minSize = Size(30, 30);
        eyes_params.scaleFactor = 1.1;
        eyes_params.neighbours = 2;
        eyes_params.flag = CV_HAAR_DO_CANNY_PRUNING;
        eyes_params.minSize = Size(30, 30);
        mouth_params.scaleFactor = 1.1;
        mouth_params.neighbours = 10;
        mouth_params.flag = CV_HAAR_FIND_BIGGEST_OBJECT;
        mouth_params.minSize = Size(30, 30);

        cout << "===================================" << endl;
        cout << "               CV Task 2           " << endl;
        cout << "===================================" << endl;

		// SVM
		// file list
		vector<string> trainingDataList, trainingMaskList;
		readImageList("data/training_data/list.txt", trainingDataList, trainingMaskList);
		
		CvSVMParams cvSvmParameter;
	
		cvSvmParameter.svm_type = CvSVM::C_SVC;
		cvSvmParameter.kernel_type = CvSVM::LINEAR;
		cvSvmParameter.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1e4, 1e-6);
		
		SvmParameter svmParams;
		svmParams.cvParams = cvSvmParameter;
		svmParams.scale = training_scale;
		svmParams.sizeOfSet = training_set_size;
		TrainingData EyeTraining;
		EyeTraining.patch = Size(24, 16);
		EyeTraining.svmModelFile = "trained_eyes.txt";
		char eylm[] = {6, 8, 9, 11};
		EyeTraining.landmarks = vector<char>(eylm, eylm + sizeof(eylm) / sizeof(char));
		EyeTraining.padding = 5;
		EyeTraining.step = 3;
		TrainingData MouthTraining;
		MouthTraining.patch = Size(40, 24);
		MouthTraining.svmModelFile = "trained_mouth.txt";
		MouthTraining.landmarks.push_back(18);
		MouthTraining.padding = 10;
		MouthTraining.step = 3;
//      cout << "hallo 3" << endl;
		Detection det_eyes;
		det_eyes.scale = Size(120, 120);
		det_eyes.scoreNum = num_of_scores;
		det_eyes.svmModelFile = EyeTraining.svmModelFile;
		det_eyes.patch = EyeTraining.patch;
		Detection det_mouth;
		det_mouth.scale = Size(120, 120);
		det_mouth.scoreNum = num_of_scores;
		det_mouth.svmModelFile = MouthTraining.svmModelFile;
		det_mouth.patch = MouthTraining.patch;

//      cout << "hallo 4" << endl;
		// test DistanceTransf
		if(test_bonus) {
			string bonus_test_out_name = cfg->getAttribute<string>("test_bonus_out");
			Mat dtTest = Mat::zeros(300, 300, CV_8UC1);
			Rect inRect = Rect(100, 50, 100, 100);
			rectangle(dtTest, inRect, 255, -1, 1);
//      cout << "hallo 5" << endl;
			distTransform(dtTest, dtTest);
			
			normalize(dtTest, dtTest, 0, 255, NORM_MINMAX, CV_8UC1);
			if(bonus_test_out_name != "")
				imwrite(bonus_test_out_name, dtTest);
		}
//      cout << "hallo 7" << endl;
		// test HoG
		if( true)// test_hog)
		{
      cout << "hallo 8" << endl;
			string hog_test_out_name = cfg->getAttribute<string>("test_hog_out");
			string file = trainingDataList[0];
			Mat im = imread(file);
			vector<Mat> testPatches;
			Mat hogMat;
			Size testWin = Size(40, 24);
			Rect testRect;
//      cout << "hallo 9" << endl;
			for(int t = 0; t < 5; t++) {
				testRect = Rect(t * 5, t * 5, testWin.width, testWin.height);
//        cout << "hallo 10" << endl;
				testPatches.push_back(im(testRect));
			}
      cout << "get Hog Features" << endl;
			getHogFeatures(testPatches, hogMat, testWin);

			if(hogMat.cols > 0 && hogMat.rows > 0) {
				resize(hogMat, hogMat, Size(hogMat.cols, hogMat.rows * 40));
				normalize(hogMat, hogMat, 0, 255, NORM_MINMAX, CV_8UC1);
				
			}
			else {
				hogMat = Mat::zeros(300, 300, CV_8UC1);
			}
				if(hog_test_out_name != "")
					imwrite(hog_test_out_name, hogMat);
		}
		
		// train the SVM
		trainSVM(EyeTraining, MouthTraining, trainingDataList, trainingMaskList, svmParams);
		
		// test SVM
		if(test_svm) {
			string svm_test_out_name = cfg->getAttribute<string>("test_svm_out");
			CvSVM svmTester1, svmTester2;
			Mat svImOut;
			if(fExists(EyeTraining.svmModelFile) && fExists(MouthTraining.svmModelFile)) {
				svmTester1.load(EyeTraining.svmModelFile.c_str());
				svmTester2.load(MouthTraining.svmModelFile.c_str());
				int svc1 = svmTester1.get_support_vector_count();
				int desc_size1 = svmTester1.get_var_count();
				int svc2 = svmTester2.get_support_vector_count();
				int desc_size2 = svmTester2.get_var_count();
				int desc_size = max(desc_size1, desc_size2);
				
				Mat svMat1 = Mat::zeros(1, desc_size, CV_32FC1);	
				Mat svMat2 = Mat::zeros(1, desc_size, CV_32FC1);
				if(svc1 > 0) {
					const float* sv1 = svmTester1.get_support_vector(0);
					for(int c = 0; c < desc_size; c++)
						svMat1.at<float>(0, c) = sv1[c % desc_size1];
				}
				if(svc2 > 0) {
					const float* sv2 = svmTester2.get_support_vector(0);
					for(int c = 0; c < desc_size; c++)
						svMat2.at<float>(0, c) = sv2[c % desc_size2];
				}
				
				svImOut = Mat::zeros(1, desc_size, CV_32FC1);
				for(int c = 0; c < desc_size1; c++)
						svImOut.push_back(svMat1);
				for(int c = 0; c < desc_size2; c++)
						svImOut.push_back(svMat2);
				
				svImOut.convertTo(svImOut, CV_8UC1, 255, 255);
				
			}
			else {
				svImOut = Mat::zeros(300, 300, CV_8UC1);
			}
			if(svm_test_out_name != "")
				imwrite(svm_test_out_name, svImOut);
			
		}

        // detect faces and create mask
                Mat last_face;
                Rect roi;
        for (std::vector<FaceInfo*>::iterator it = faces.begin(); it != faces.end(); ++it)
        {
            Point l, r, m;
            Rect face_roi;

            detectFace((*it)->face, face_roi);
            last_face = (*it)->face.clone();
            roi = face_roi;
            detectEyesMouth((*it)->face, face_roi, l, r, m, det_eyes, det_mouth);

            (*it)->setEyes(l, r);
            (*it)->setMouth(m);

            Mat mask = Mat::zeros((*it)->face.size(),CV_8UC1);
            (*it)->mask = mask;
            
            maskFace((*it)->mask, l, r, m, 1.8f, 1.4f);
        }
                
        rectangle(last_face,roi,Scalar(0, 0, 255));
        imwrite(detect_face_string,last_face);
        // draw face detection results
        for (std::vector<FaceInfo*>::iterator it = faces.begin(); it != faces.end(); ++it)
        {
            Mat face_to_draw = (*it)->face.clone();
            Point l, r, m;

            l = (*it)->left_eye;
            r = (*it)->right_eye;
            m = (*it)->mouth;

            if (l.x && l.y) circle(face_to_draw, l, 10, Scalar(0, 0, 255), 2, 8, 0); // left eye is red
            if (r.x && r.y) circle(face_to_draw, r, 10, Scalar(255, 0, 0), 2, 8, 0); // right eye is blue
            if (m.x && m.y) circle(face_to_draw, m, 30, Scalar(0, 255, 0), 2, 8, 0); // mouth is green

            imwrite((*it)->out_face_detect_filename, face_to_draw);
            imwrite((*it)->out_face_mask_filename, (*it)->mask);
        }

        // apply affine transformation
        FaceInfo* first_face = faces[0];
        Mat f1 = first_face->face;
        Point l1 = first_face->left_eye;
        Point r1 = first_face->right_eye;
        Point m1 = first_face->mouth;

        // find outliers
        vector<Point2f> outliers;
        outliers.push_back(Point2f(0, 0));
        outliers.push_back(Point2f(f1.cols - 1, 0));
        outliers.push_back(Point2f(0, f1.rows - 1));
        outliers.push_back(Point2f(f1.cols, f1.rows));
        
        for (std::vector<FaceInfo*>::iterator it = faces.begin(); it != faces.end(); ++it)
        {
            Mat f2 = (*it)->face;
            Mat mask2 = (*it)->mask;
            Point l2 = (*it)->left_eye;
            Point r2 = (*it)->right_eye;
            Point m2 = (*it)->mouth;

            Mat T = Mat::eye(2, 3, 6);
            (*it)->face_trans = affineTransform(f2, l2, r2, m2, l1, r1, m1, T);
            (*it)->mask = affineTransform(mask2, l2, r2, m2, l1, r1, m1, T);
            imwrite((*it)->out_face_trans_filename, (*it)->face_trans);
            
            vector<Point2f> cur_outliers = affineTransformVertices(f2, T);
            
            (cur_outliers[2].x > f2.cols) ? cur_outliers[2].x = f2.cols - 1 : cur_outliers[2].x = cur_outliers[2].x;
            (cur_outliers[2].y > f2.rows) ? cur_outliers[2].y = f2.rows - 1 : cur_outliers[2].y = cur_outliers[2].y;
            (cur_outliers[3].x > f2.cols) ? cur_outliers[3].x = f2.cols - 1 : cur_outliers[3].x = cur_outliers[3].x;
            (cur_outliers[3].y > f2.rows) ? cur_outliers[3].y = f2.rows - 1 : cur_outliers[3].y = cur_outliers[3].y;
            
            outliers[0] = Point2f(max(outliers[0].x, cur_outliers[0].x), max(outliers[0].y, cur_outliers[0].y));
            outliers[1] = Point2f(min(outliers[1].x, cur_outliers[1].x), max(outliers[1].y, cur_outliers[1].y));
            outliers[2] = Point2f(max(outliers[2].x, cur_outliers[2].x), min(outliers[2].y, cur_outliers[2].y));
            outliers[3] = Point2f(min(outliers[3].x, cur_outliers[3].x), min(outliers[3].y, cur_outliers[3].y));
        }

        Mat outlier = Mat::zeros(f1.size(), CV_32FC3);
        for (int i = 0; i < outliers.size(); i++)
        {
            circle(outlier, outliers[i], 10, Scalar(0, 0, 255), 2, 8, 0);
        }

        outliers = calculateRect(outliers);

        for (int i = 0; i < outliers.size(); i++)
        {
            circle(outlier, outliers[i], 3, Scalar(0, 255, 0), 2, 8, 0);
        }
        imwrite(outlier_file, outlier);

        // trim images
        Rect trimArea = Rect(outliers[0], outliers[3]);
        
        for (std::vector<FaceInfo*>::iterator it = faces.begin(); it != faces.end(); ++it)
        {
            Mat face = (*it)->face_trans;
            if(trimArea.width == 0 && trimArea.height == 0)
				trimArea = Rect(0, 0, face.cols, face.rows);
            Mat trimmed1 = face(trimArea);
            trimmed1.copyTo((*it)->face_trimmed);

            Mat mask = (*it)->mask;
            Mat trimmed2 = mask(trimArea);
            trimmed2.copyTo((*it)->mask);

            imwrite((*it)->out_face_trimmed_filename, (*it)->face_trimmed);
            imwrite((*it)->out_face_mask_filename, (*it)->mask);
        }


        vector<Mat> frames = blendFaceSequence(faces, transform_time, fps);
	if(!frames.empty()){
		
		imwrite(imagesequence,frames[fps]);
		createVideo(frames, fps, video_out, Size(frames.at(1).cols,frames.at(1).rows));
	}
    }
    catch (std::exception &ex) {
      cout << ex.what() << endl;
      cout << "Program exited with errors!" << endl;
      return -1;

      cout << "Program exited normally!" << endl;
    }
}
