#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include "std_msgs/String.h"
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <locale>

using namespace cv;
using namespace std;

static const std::string OPENCV_WINDOW = "Image window";

Mat object = imread("/home/turtlebot/Pictures/book.jpg", CV_LOAD_IMAGE_GRAYSCALE );
int minHessian = 500;
SurfDescriptorExtractor extractor;
Mat des_object;
FlannBasedMatcher matcher;
std::vector<Point2f> obj_corners(4);
ros::Publisher pubb1;
std_msgs::String msg11;
std::stringstream ss1;


SurfFeatureDetector detector( minHessian );
std::vector<KeyPoint> kp_object;

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
	  pubb1 = nh.advertise<std_msgs::String>("siftPointsMessage", 10);
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);


    //Calculate descriptors (feature vectors)

    namedWindow("Good Matches");



    //Get the corners from the object
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( object.cols, 0 );
    obj_corners[2] = cvPoint( object.cols, object.rows );
    obj_corners[3] = cvPoint( 0, object.rows );

    char key = 'a';
    int framecount = 0;

    Mat des_image, img_matches;
    std::vector<KeyPoint> kp_image;
    std::vector<vector<DMatch > > matches;
    std::vector<DMatch > good_matches;
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    std::vector<Point2f> scene_corners(4);
    Mat H;
    Mat image;

 //   cvtColor(frame, image, CV_RGB2GRAY);

    detector.detect( cv_ptr->image, kp_image );
    extractor.compute( cv_ptr->image, kp_image, des_image );

    matcher.knnMatch(des_object, des_image, matches, 2);

    for(int i = 0; i < min(des_image.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
    {
        if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
        {
            good_matches.push_back(matches[i][0]);
        }
    }

    //Draw only "good" matches
    drawMatches( object, kp_object, cv_ptr->image, kp_image, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    if (good_matches.size() >= 4)
    {
        for( int i = 0; i < good_matches.size(); i++ )
        {
            //Get the keypoints from the good matches
            obj.push_back( kp_object[ good_matches[i].queryIdx ].pt );
            scene.push_back( kp_image[ good_matches[i].trainIdx ].pt );
        }

        H = findHomography( obj, scene, CV_RANSAC );

        perspectiveTransform( obj_corners, scene_corners, H);

        int xCo1 = (int)Point2f(object.cols, 0).x + scene_corners[0].x;
        int xCo2 = (int)Point2f(object.cols, 0).x + scene_corners[1].x;
        int xCo3 = (int)Point2f(object.cols, 0).x + scene_corners[2].x;
        int xCo4 = (int)Point2f(object.cols, 0).x + scene_corners[3].x;
        int xCo = (xCo1 + xCo2 + xCo3 + xCo4)/4;

        int yCo1 = (int)Point2f(object.cols, 0).y + scene_corners[0].y;
        int yCo2 = (int)Point2f(object.cols, 0).y + scene_corners[1].y;
        int yCo3 = (int)Point2f(object.cols, 0).y + scene_corners[2].y;
        int yCo4 = (int)Point2f(object.cols, 0).y + scene_corners[3].y;
        int yCo = (yCo1 + yCo2 + yCo3 + yCo4)/4;


//        cv::circle(img_matches, cv::Point(500,500),2,CV_RGB(255,0,0));


        //ROS_INFO("The Coordinates are: %d, %d", xCo,yCo);

        string StringY;
       	 string StringX;
       	 StringX = static_cast<ostringstream*>( &(ostringstream() << xCo) )->str();
       	 StringY = static_cast<ostringstream*>( &(ostringstream() << yCo) )->str();

       	 if(xCo > 640 && xCo < 1280)
       	 {
			 ss1 << "s "+StringX+" "+StringY;
			 msg11.data = ss1.str();
       	 }
       	 else
       	 {
			 ss1 << "n";
			 msg11.data = ss1.str();
       	 }
		 pubb1.publish(msg11);
		 ss1.str("");

        //Draw lines between the corners (the mapped object in the scene image )
        line( img_matches, scene_corners[0] + Point2f( object.cols, 0), scene_corners[1] + Point2f( object.cols, 0), Scalar(0, 255, 0), 4 );
        line( img_matches, scene_corners[1] + Point2f( object.cols, 0), scene_corners[2] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[2] + Point2f( object.cols, 0), scene_corners[3] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[3] + Point2f( object.cols, 0), scene_corners[0] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
    }


    imshow( "Good Matches", img_matches );
    // Update GUI Window
   // cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};

void initialSetup(){
    detector.detect( object, kp_object );
}
void secondIntial(){
    extractor.compute( object, kp_object, des_object );

}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "sift_node");
  ROS_INFO("OK");
  initialSetup();
  secondIntial();
  ImageConverter ic;
  ros::spin();
  return 0;
}

