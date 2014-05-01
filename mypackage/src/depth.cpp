#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "std_msgs/String.h"


void imgcb(const sensor_msgs::Image::ConstPtr& msg);

static const uint32_t MY_ROS_QUEUE_SIZE = 1000;
ros::Publisher pubb;
std_msgs::String msgg;
std::stringstream ss;

void imgcb(const sensor_msgs::Image::ConstPtr& msg)
{
    // The message's data is a raw buffer. While the type is uint8_t*, the
    // data itself is an array of floats (for depth data), the value of
    // the float being the distance in meters.

      float depth_v = *reinterpret_cast<const float*>(&msg->data[0]);
      std::string s = boost::lexical_cast<std::string>(depth_v);
      ss << s;
  	  ros::NodeHandle nh;
  	  pubb = nh.advertise<std_msgs::String>("depth_data", 10);
      msgg.data = ss.str();
      pubb.publish(msgg);
	  ss.str("");



}

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "depth_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("camera/depth/image", MY_ROS_QUEUE_SIZE, imgcb);
    ros::spin();

    return 0;
}
