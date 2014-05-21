#include "ros/ros.h"
#include <ros/ros.h>
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/BumperEvent.h>
#include  <kobuki_msgs/Led.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

void *thread_move(void* ptr);

#define Fn_STOP 1
#define Fn_FORWARD 2
#define Fn_BACKWARD 3
#define Fn_RIGHT 4
#define Fn_LEFT 5
#define Fn_WAKEUP 6
#define Fn_SLEEP 7
#define Fn_FACECOUNT 8
#define Fn_FACEFOLLOW 9
#define Fn_SIFTFOLLOW 10

#define MAX_LEN 1024
int port_1 = 2064;
int port_2 = 2065;
int socket_reply = 7;

char bump_msg[10] = "released";
char sock_msg[2] = "7";
char button_msg[10] = "";
char depth_msg[20] = "123456789";
float depth_main = 0;
char faceCount_msg[10] = "999999999";
int faceCount_main = 0;
int faceCount_flag = 0;
char faceRecog_msg[10] = "999999999";
int faceRecog_main = 0;
char sendingBuffer[2];
char Face_Points_Data[10] = "";
char Sift_Points_Data[10] = "";
char Face_Data_Turn[3] = "";
char Sift_Data_Turn[3] = "";
char Face_turn_side[10] = "";
char Sift_turn_side[10] = "";
int myX = 0, myY = 0;
int sift_myX = 0, sift_myY = 0;


ros::Publisher move_pub;
geometry_msgs::Twist tw3;

ros::Publisher led1_pub;
ros::Publisher led2_pub;

void Socket1NodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(sock_msg, msg2->data.c_str());
	//ROS_INFO("value socket : %s", sock_msg);
	socket_reply = atoi(sock_msg);
}

void BumpNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(bump_msg, msg2->data.c_str());
	//ROS_INFO("value bump : %s", &bump_msg);
}
void ButtonNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(button_msg, msg2->data.c_str());
	//ROS_INFO("value button : %s", &button_msg);
}
void FaceCountNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(faceCount_msg, msg2->data.c_str());
	//ROS_INFO("Face Count RECEIVED: %s", faceCount_msg);
	faceCount_main = atoi(faceCount_msg);
	//ROS_INFO("Face Count RECEIVED: %d", faceCount_main);
}
void FaceRecogNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(faceRecog_msg, msg2->data.c_str());
	//ROS_INFO("Face Count RECEIVED: %s", faceCount_msg);
	faceRecog_main = atoi(faceRecog_msg);
	//ROS_INFO("Face Recog RECEIVED: %d", faceRecog_main);
}
void FacePointsNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(Face_Points_Data, msg2->data.c_str());
	//ROS_INFO("FacePoints :-%s-      \n", Face_Points_Data);

	if (strncmp(Face_Points_Data, "s" , 1)==0)
	{

		//TOKENIZING
		char *myt = strtok(Face_Points_Data," ");
		char *my2 = strtok(NULL," ");
		char *my3 = strtok(NULL," ");

		strcpy(Face_turn_side, myt);
		//ROS_INFO("--------> %s", Face_turn_side);
		myX = atoi(my2);
		myY = atoi(my3);
		if(myX < 210)
		{
			//printf("left\n");
			strcpy(Face_Data_Turn , "ll");
		}
		else if(myX > 425)
		{
			//printf("right\n");
			strcpy(Face_Data_Turn , "rr" );
		}
		else
		{
			//printf("none\n");
			strcpy(Face_Data_Turn , "nn");
		}
		//printf("the value in the turn is: -%s-\n" , Face_Data_Turn);
		//ROS_INFO("Actual Data: %s - %d - %d",myt,myX, myY);
	}
	else if(strncmp(Face_Points_Data, "m" , 1)==0)
	{
		strcpy(Face_turn_side, "m");
		//ROS_INFO("--------> %s", Face_turn_side);
		strcpy(Face_Data_Turn , " ");
	}
	else
	{
		strcpy(Face_turn_side, "n");
		//ROS_INFO("--------> %s", Face_turn_side);
		strcpy(Face_Data_Turn , " ");
	}

}
void SiftPointsNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(Sift_Points_Data, msg2->data.c_str());
	//ROS_INFO("FacePoints :-%s-      \n", Face_Points_Data);

	if (strncmp(Sift_Points_Data, "s" , 1)==0)
	{

		//TOKENIZING
		char *myt = strtok(Sift_Points_Data," ");
		char *my2 = strtok(NULL," ");
		char *my3 = strtok(NULL," ");

		strcpy(Sift_turn_side, myt);
		//ROS_INFO("--------> %s", Face_turn_side);
		sift_myX = atoi(my2);
		sift_myY = atoi(my3);
		if(sift_myX < 850)
		{
			//printf("left\n");
			strcpy(Sift_Data_Turn , "ll");
		}
		else if(sift_myX > 1065)
		{
			//printf("right\n");
			strcpy(Sift_Data_Turn , "rr" );
		}
		else
		{
			//printf("none\n");
			strcpy(Sift_Data_Turn , "nn");
		}
		//printf("the value in the turn is: -%s-\n" , Face_Data_Turn);
		ROS_INFO("Actual Data: %s - %d - %d",myt,sift_myX, sift_myY);
	}
	else
	{
		strcpy(Sift_turn_side, "n");
		//ROS_INFO("--------> %s", Face_turn_side);
		strcpy(Sift_Data_Turn , " ");
	}

}
void DepthNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(depth_msg, msg2->data.c_str());
	depth_main = atof(depth_msg);
	//ROS_INFO("Depth Value: %f", depth_main);
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "move_node");
	ros::NodeHandle nh;

	move_pub = nh.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 10);
	led1_pub = nh.advertise<kobuki_msgs::Led>("/mobile_base/commands/led1", 10);
	led2_pub = nh.advertise<kobuki_msgs::Led>("/mobile_base/commands/led2", 10);

	ros::Subscriber bumper = nh.subscribe("bumperMessage",1, BumpNodeCallback);
	ros::Subscriber socket1 = nh.subscribe("socket1Message",1, Socket1NodeCallback);
	ros::Subscriber button = nh.subscribe("buttonMessage",1, ButtonNodeCallback);
	ros::Subscriber faceCount = nh.subscribe("faceCountMessage",1, FaceCountNodeCallback);
	ros::Subscriber faceRecog = nh.subscribe("faceRecogMessage",1, FaceRecogNodeCallback);
	ros::Subscriber facePoints = nh.subscribe("facePointsMessage",1, FacePointsNodeCallback);
	ros::Subscriber siftPoints = nh.subscribe("siftPointsMessage",1, SiftPointsNodeCallback);
	ros::Subscriber depth = nh.subscribe("depth_data",1, DepthNodeCallback);



	char* message = "a";
	pthread_t tester;
	//pthread_create(&tester, NULL, thread_socket, (void*)message);
	pthread_create(&tester, NULL, thread_move, (void*)message);
	//pthread_create(&tester, NULL, thread_socket2, (void*)message);

	ros::spin();

	return 0;

}//main
void* thread_move(void* ptr)
{

	geometry_msgs::Twist tw;
	//kobuki_msgs::Led led1;
	//kobuki_msgs::Led led2;
	//led1.value = kobuki_msgs::Led::RED;
	//led2.value = kobuki_msgs::Led::RED;
	//led1_pub.publish(led1);
	//led2_pub.publish(led2);

	while(ros::ok())
	{
		//ROS_INFO("SOCKET IS----> %d", socket_reply);
		if(socket_reply == Fn_WAKEUP)
		{
			//ROS_INFO("REPLY 1");
			//led1.value = kobuki_msgs::Led::GREEN;
			//led2.value = kobuki_msgs::Led::GREEN;
			//led1_pub.publish(led1);
			//led2_pub.publish(led2);
		}
		else if(socket_reply == Fn_SLEEP)
		{
			//ROS_INFO("REPLY 1");
			//led1.value = kobuki_msgs::Led::RED;
			//led2.value = kobuki_msgs::Led::RED;
			//led1_pub.publish(led1);
			//led2_pub.publish(led2);
		}
		else if(socket_reply == Fn_STOP)
		{
			//ROS_INFO("REPLY 1");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
//			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_FORWARD)
		{
			//ROS_INFO("REPLY 2");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.linear.x = 0.3;
//			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_BACKWARD)
		{
			//ROS_INFO("REPLY 3");
			tw.linear.x = 0;
			tw.angular.z = 0;
			tw.linear.x = -0.3;
		//	move_pub.publish(tw);
		}
		else if(socket_reply == Fn_RIGHT)
		{
			//ROS_INFO("REPLY 4");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = -0.3;
	//		move_pub.publish(tw);
		}
		else if(socket_reply == Fn_LEFT)
		{
			//ROS_INFO("REPLY 5");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = 0.3;
		//	move_pub.publish(tw);
		}
		else if(socket_reply == Fn_FACECOUNT)
		{

			ROS_INFO("In face count");
			char* message = "a";
			pthread_t tester;

			//ROS_INFO("REPLY 5");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = 0.0;
		//	move_pub.publish(tw);
			//ROS_INFO("Face Count --------------->: %s", faceCount_msg);

			faceCount_flag = 1;
//			pthread_create(&tester, NULL, thread_socket2, (void*)message);

		}
		else if(socket_reply == Fn_FACEFOLLOW)
		{

//			ROS_INFO("In face follow");
			if(strcmp(Face_turn_side, "s")==0)
			{

				if(strncmp(Face_Data_Turn, "rr" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = -0.5;
					ROS_INFO("Right");

				}
				else if(strncmp(Face_Data_Turn, "ll" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = 0.5;
					ROS_INFO("Left");

				}
				else if(strncmp(Face_Data_Turn, "nn" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = 0.0;
					ROS_INFO("Centre");
				}


			//	move_pub.publish(tw);
			}
			else if(strcmp(Face_turn_side, "m")==0)
			{

				tw.angular.z = 0.0;
				tw.linear.x = 0.0;
				strcpy(Face_turn_side, " ");
				strcpy(Face_Data_Turn, " ");
			//	move_pub.publish(tw);
			}
			else
			{
				tw.angular.z = 0.0;
				tw.linear.x = 0.0;
				strcpy(Face_turn_side, " ");
				strcpy(Face_Data_Turn, " ");
				//move_pub.publish(tw);
			}

		}
		else if(socket_reply == Fn_SIFTFOLLOW)
		{

//			ROS_INFO("In face follow");
			if(strcmp(Sift_turn_side, "s")==0)
			{

				if(strncmp(Sift_Data_Turn, "rr" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = -0.5;
					//ROS_INFO("Right");

				}
				else if(strncmp(Sift_Data_Turn, "ll" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = 0.5;
					//ROS_INFO("Left");

				}
				else if(strncmp(Sift_Data_Turn, "nn" ,2)==0)
				{
					tw.linear.x = 0.2;
					tw.angular.z = 0.0;
					//ROS_INFO("Centre");
				}


			//	move_pub.publish(tw);
			}
			else
			{
				tw.angular.z = 0.0;
				tw.linear.x = 0.0;
				strcpy(Sift_turn_side, " ");
				strcpy(Sift_Data_Turn, " ");
				//move_pub.publish(tw);
			}

		}
		move_pub.publish(tw);
	}

}
