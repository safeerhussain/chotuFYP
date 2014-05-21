#include "ros/ros.h"
#include <ros/ros.h>
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/BumperEvent.h>
#include  <kobuki_msgs/Led.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>

using namespace std;
/////////////////////////////////////////////////////////
//Bumper Code Working Fine. Don't Change.
/////////////////////////////////////////////////////////

ros::Publisher pubb;
ros::Publisher pubb1;
int bump_n = 5;

std_msgs::String msg1;
std::stringstream ss;


std_msgs::String msg11;
std::stringstream ss1;


static const std::string OPENCV_WINDOW = "Image window";
namespace enc = sensor_msgs::image_encodings;

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame );

String face_cascade_name = "/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

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
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      //cv::imshow(OPENCV_WINDOW, cv_ptr->image);
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
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return; };

    detectAndDisplay(cv_ptr->image);

    cv::waitKey(3);

       image_pub_.publish(cv_ptr->toImageMsg());
  }
};

void detectAndDisplay( Mat frame )
{
  ros::NodeHandle nh;
  pubb = nh.advertise<std_msgs::String>("facePointsMessage", 10);
  pubb1 = nh.advertise<std_msgs::String>("faceCountMessage", 10);
  std::vector<Rect> faces;

  Mat frame_gray;
  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  String f_c;
    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    f_c = static_cast<ostringstream*>( &(ostringstream() << faces.size()) )->str();
    ss1 << f_c;
   msg11.data = ss1.str();
   pubb1.publish(msg11);
   ss1.str("");
  //for( int i = 1; i <= faces.size(); i++ )
  int i = 0;
  //ROS_INFO("%d ",  faces.size());
  Point p1( 210, 0 );
   Point p2(210,480);
   line(frame, p1, p2, Scalar( 0, 0, 255 ),2, 8, 0 );

   Point p3( 425, 0 );
  Point p4(425,480);
  line(frame, p3, p4, Scalar( 0, 0, 255 ),2, 8, 0 );

  if(faces.size() == 0)
  {
	  	 ss << "n";
		 msg1.data = ss.str();

		 ROS_INFO("%s", msg1.data.c_str());
		 pubb.publish(msg1);
		 ss.str("");
  }

  while(faces.size() != 0 && i<faces.size())
  {
  //while(faces.size()){
  Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
  if(faces.size() == 1)
  {
	 ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 0, 255, 0 ),4, 8, 0 );
	 //ROS_INFO("x: %d y: %d",faces[0].x, faces[0].y);
	 int myx,myy = 0;
	 string StringY;
	 string StringX;

	 myx = faces[0].x;
	 StringX = static_cast<ostringstream*>( &(ostringstream() << myx) )->str();
	 myy = faces[0].y;
	 StringY = static_cast<ostringstream*>( &(ostringstream() << myy) )->str();

	 ss << "s "+StringX+" "+StringY;
	 msg1.data = ss.str();
	 pubb.publish(msg1);
	 ss.str("");
	  ROS_INFO("SINGLE FACE");
  }

  else
  {
	  ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 0, 0, 255 ),4, 8, 0 );
	//  ROS_INFO("%d %d",faces[0].x, faces[0].y);
	  ROS_INFO("M0RE THAN ONE ");
	  //ROS_INFO("%d ",  faces.size());

	  ss << "m";
	  msg1.data = ss.str();
	  ss.str("");
	//	 ROS_INFO("%s", msg1.data.c_str());
   	  pubb.publish(msg1);

  }

  Mat faceROI = frame_gray( faces[i] );
  std::vector<Rect> eyes;

  //-- In each face, detect eyes
  eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );


  	/*for( int j = 0; j < eyes.size(); j++ )
    {
      Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
      int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
      //circle( frame, center, radius, Scalar( 255, 0, 0 ), 2, 8, 0 );
    } // Inner For*/

  i++;
    }//loop


  //-- Show what you got
  imshow( window_name, frame );

  while (!faces.empty())
    {
       faces.pop_back();
    }
} // Method


int main(int argc, char **argv)
{

	ros::init(argc, argv, "bump_node");
	ros::NodeHandle nh;
	  ROS_INFO("OK");
	  ImageConverter ic;

	//ros::Subscriber bumper = nh.subscribe("/mobile_base/events/bumper",1, callback);
  ros::spin();

  return 0;
}
