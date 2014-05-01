#include "ros/ros.h"
#include <ros/ros.h>
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/ButtonEvent.h>
#include  <kobuki_msgs/Led.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/////////////////////////////////////////////////////////
//Button Code Working Fine. Don't Change.
/////////////////////////////////////////////////////////
ros::Publisher pubb;

std_msgs::String msg1;
std::stringstream ss;

void callback (const kobuki_msgs::ButtonEventConstPtr msg)
{
	  ros::NodeHandle nh;
	  pubb = nh.advertise<std_msgs::String>("buttonMessage", 10);

	if(msg->state == kobuki_msgs::ButtonEvent::PRESSED)
	{
		if(msg->button == kobuki_msgs::ButtonEvent::Button0)
		{
			//ROS_INFO("B0");
			ss << "b0";
		}
		if(msg->button == kobuki_msgs::ButtonEvent::Button1)
		{
			//ROS_INFO("B1");
			ss << "b1";
		}
		if(msg->button == kobuki_msgs::ButtonEvent::Button2)
		{
			//ROS_INFO("B2");
			ss << "b2";
		}
		 msg1.data = ss.str();
	//	 ROS_INFO("%s", msg1.data.c_str());
		 pubb.publish(msg1);
		 ss.str("");

	}

}
int main(int argc, char **argv)
{
  ros::init(argc, argv, "button_node");
  ros::NodeHandle nh;
  ros::Subscriber button = nh.subscribe("/mobile_base/events/button",1, callback);
  ros::spin();

  return 0;
}
