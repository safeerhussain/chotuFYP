#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <iostream>
#include <stdio.h>

#include <fstream>
#include <sstream>

#include "std_msgs/String.h"
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <locale>

using namespace std;
using namespace cv;

ros::Publisher pubb;





static const std::string OPENCV_WINDOW = "Image window";
namespace enc = sensor_msgs::image_encodings;



void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels);

String face_cascade_name = "/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
string fn_csv = "/home/turtlebot/Downloads/DetectsAndRecognizeFaces/DetectsAndRecognizeFaces/csv.ext";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
vector<Mat> images;
vector<int> labels;



string g_listname_t[]=
{
	"Tran Dang Bao",
	"Arnold Schwarzenegger"
};

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;



public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1, &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
	  ros::NodeHandle nh;
	  std_msgs::String msg1;
	  std::stringstream ss;
	  string f_c;
	pubb = nh.advertise<std_msgs::String>("faceRecogMessage", 10);
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ))
    { printf("--(!)Error loading First One\n"); return;
    };
//    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return; };

    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
       model->train(images, labels);
       // That's it for learning the Face Recognition model. You now
       // need to create the classifier for the task of Face Detection.
       // We are going to use the haar cascade you have specified in the
       // command line arguments:
       //
       CascadeClassifier haar_cascade;
       haar_cascade.load(face_cascade_name);

       int im_width = images[0].cols;
           int im_height = images[0].rows;
       // ------------------------------------------------------
         //  Mat frame;
           // Clone the current frame:
           Mat original = cv_ptr->image.clone();
           // Convert the current frame to grayscale:
           Mat gray;
           cvtColor(original, gray, CV_BGR2GRAY);
           // Find the faces in the frame:
           vector< Rect_<int> > faces;
           haar_cascade.detectMultiScale(gray, faces);
           // At this point you have the position of the faces in
           // faces. Now we'll get the faces, make a prediction and
           // annotate it in the video. Cool or what?
           for(int i = 0; i < faces.size(); i++) {
               // Process face by face:
               Rect face_i = faces[i];
               // Crop the face from the image. So simple with OpenCV C++:
               Mat face = gray(face_i);
               // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
               // verify this, by reading through the face recognition tutorial coming with OpenCV.
               // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
               // input data really depends on the algorithm used.
               //
               // I strongly encourage you to play around with the algorithms. See which work best
               // in your scenario, LBPH should always be a contender for robust face recognition.
               //
               // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
               // face you have just found:
               Mat face_resized;
               cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
               // Now perform the prediction, see how easy that is:
               int prediction = model->predict(face_resized);
               // And finally write all we've found out to the original image!
               // First of all draw a green rectangle around the detected face:
               rectangle(original, face_i, CV_RGB(0, 255,0), 1);
               // Create the text we will annotate the box with:
               string box_text;

   			if (prediction == 12) {
   				f_c = static_cast<ostringstream*>( &(ostringstream() << prediction) )->str();
   				    ss << f_c;
			 msg1.data = ss.str();
			 pubb.publish(msg1);
			 ss.str("");
   				box_text.append(" Imran Hemani" );
   			}
   			else if(prediction == 10){
   				f_c = static_cast<ostringstream*>( &(ostringstream() << prediction) )->str();
				ss << f_c;
				 msg1.data = ss.str();
				 pubb.publish(msg1);
				 ss.str("");
   				box_text.append(" Safeer e Hussain" );
   			}
   			else
   			{
   				f_c = static_cast<ostringstream*>( &(ostringstream() << 99) )->str();
				ss << f_c;
				 msg1.data = ss.str();
				 pubb.publish(msg1);
				 ss.str("");
   				box_text.append( " Unknown Person" );
   			}
               // Calculate the position for annotated text (make sure we don't
               // put illegal values in there):
               int pos_x = std::max(face_i.tl().x - 10, 0);
               int pos_y = std::max(face_i.tl().y - 10, 0);
               // And now put it into the image:
               putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
           }
           // Show the result:
           imshow("face_recognizer", original);
           // And display it:



    // Update GUI Window
   // cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};

int main(int argc, char** argv)
{
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }

  ros::init(argc, argv, "face_node");
  ROS_INFO("OK");
  ImageConverter ic;
  ros::spin();
  return 0;
}

void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels) {

	char separator = ';';
	std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }

    else {printf ("Read Successfully!");}


    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}
