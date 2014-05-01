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
#include <stdlib.h>
#include <unistd.h>

/////////////////////////////////////////////////////////
//Bumper Code Working Fine. Don't Change.
/////////////////////////////////////////////////////////
ros::Publisher pubb;
int bump_n = 5;

std_msgs::String msg1;
std::stringstream ss;

void callback(const kobuki_msgs::BumperEventConstPtr msg)
{
	  ros::NodeHandle nh;
	  pubb = nh.advertise<std_msgs::String>("bumperMessage", 10);

	  if (msg->state == kobuki_msgs::BumperEvent::PRESSED  && msg->bumper == kobuki_msgs::BumperEvent::CENTER )
	  {
		  ROS_INFO("Pressed Center");
 		  bump_n = 1;
          ss << "center";
	  }
	  else if (msg->state == kobuki_msgs::BumperEvent::PRESSED  && msg->bumper == kobuki_msgs::BumperEvent::RIGHT )
	  {
		  ROS_INFO("Pressed Right");
		  bump_n = 2;
 		  ss << "right";
	  }

	  else if (msg->state == kobuki_msgs::BumperEvent::PRESSED  && msg->bumper == 0 )
	  {
		  ROS_INFO("Pressed Left");
		  bump_n = 0;
		  ss << "left";
	  }

  	  else if(msg->state == kobuki_msgs::BumperEvent::RELEASED)
	  {
  		 ROS_INFO("Released");
  		 bump_n = 4;
  		 ss << "released";
	  }

	 msg1.data = ss.str();
//	 ROS_INFO("%s", msg1.data.c_str());
	 pubb.publish(msg1);
	 ss.str("");
}

int main(int argc, char **argv)
{ 
  ros::init(argc, argv, "bump_node");
  ros::NodeHandle nh;
  ros::Subscriber bumper = nh.subscribe("/mobile_base/events/bumper",1, callback);
  ros::spin();

  return 0;
}
