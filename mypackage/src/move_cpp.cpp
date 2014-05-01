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


void socket_server_listener();
void *thread_socket(void* ptr);
void *thread_move(void* ptr);

#define Fn_STOP 1
#define Fn_FORWARD 2
#define Fn_BACKWARD 3
#define Fn_RIGHT 4
#define Fn_LEFT 5
#define Fn_WAKEUP 6
#define Fn_SLEEP 7
#define Fn_FACECOUNT 8



#define MAX_LEN 1024
//#define PORT_NUMBER 2055
int port_number = 2055;
int socket_reply = 99;

char bump_msg[10] = "released";
char button_msg[10] = "";
char depth_msg[20] = "123456789";
float depth_main;
char faceCount_msg[10] = "12345678";
int faceCount_main;
char sendingBuffer[2];

ros::Publisher move_pub;
geometry_msgs::Twist tw3;

ros::Publisher led1_pub;
ros::Publisher led2_pub;


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
void DepthNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(depth_msg, msg2->data.c_str());
	depth_main = atof(depth_msg);
	//ROS_INFO("Depth Value: %f", depth_main);
}
int main(int argc, char **argv)
{
/*	if (argc < 1)
	{
		ROS_ERROR("Please provide Port Number");
		exit(0);
	}
	else
	{
		port_number = atoi(argv[1]);
	}*/
	ros::init(argc, argv, "move_node");
	ros::NodeHandle nh;

	move_pub = nh.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 10);
	led1_pub = nh.advertise<kobuki_msgs::Led>("/mobile_base/commands/led1", 10);
	led2_pub = nh.advertise<kobuki_msgs::Led>("/mobile_base/commands/led2", 10);

	ros::Subscriber bumper = nh.subscribe("bumperMessage",1, BumpNodeCallback);
	ros::Subscriber button = nh.subscribe("buttonMessage",1, ButtonNodeCallback);
	ros::Subscriber faceCount = nh.subscribe("faceCountMessage",1, FaceCountNodeCallback);
	ros::Subscriber depth = nh.subscribe("depth_data",1, DepthNodeCallback);


	char* message = "a";
	pthread_t tester;
	pthread_create(&tester, NULL, thread_socket, (void*)message);
	pthread_create(&tester, NULL, thread_move, (void*)message);

	ros::spin();

	return 0;
}
void* thread_move(void* ptr)
{
	//ROS_INFO("IN MOVE %d", socket_reply);
	geometry_msgs::Twist tw;
	kobuki_msgs::Led led1;
	kobuki_msgs::Led led2;

	while(ros::ok())
	{
		if(socket_reply == Fn_WAKEUP)
		{
			//ROS_INFO("REPLY 1");
			led1.value = kobuki_msgs::Led::RED;
			led2.value = kobuki_msgs::Led::RED;
			led1_pub.publish(led1);
			led2_pub.publish(led2);
		}
		if(socket_reply == Fn_SLEEP)
		{
			//ROS_INFO("REPLY 1");
			led1.value = kobuki_msgs::Led::BLACK;
			led2.value = kobuki_msgs::Led::BLACK;
			led1_pub.publish(led1);
			led2_pub.publish(led2);
		}
		if(socket_reply == Fn_STOP)
		{
			//ROS_INFO("REPLY 1");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_FORWARD)
		{
			//ROS_INFO("REPLY 2");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.linear.x = 0.3;
			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_BACKWARD)
		{
			//ROS_INFO("REPLY 3");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.linear.x = -0.3;
			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_RIGHT)
		{
			//ROS_INFO("REPLY 4");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = 0.3;
			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_LEFT)
		{
			//ROS_INFO("REPLY 5");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = -0.3;
			move_pub.publish(tw);
		}
		else if(socket_reply == Fn_FACECOUNT)
		{
			//ROS_INFO("REPLY 5");
			tw.linear.x = 0;
			tw.linear.y = 0;
			tw.angular.z = 0;
			tw.angular.z = 0.0;
			move_pub.publish(tw);
			ROS_INFO("Face Count --------------->: %d", faceCount_main);



		}
	}

}
void* thread_socket(void* ptr)
{
	socket_server_listener();
}
void socket_server_listener(){

    int socketFileDescriptor, newSocketFileDescriptor;
    socklen_t clilen;
    char buffer[MAX_LEN];
    struct sockaddr_in serverAddress, clientAddress;
    int numBytes;
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0)
        ROS_ERROR("ERROR opening socket");


    bzero((char *) &serverAddress, sizeof (serverAddress));


    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port_number);
    if (bind(socketFileDescriptor, (struct sockaddr *) &serverAddress,
             sizeof (serverAddress)) < 0) {
    	ROS_ERROR("ERROR, no port provided");

    }
    listen(socketFileDescriptor, 5);
    clilen = sizeof (clientAddress);
    newSocketFileDescriptor = accept(socketFileDescriptor,
                                  (struct sockaddr *) &clientAddress,
                                  &clilen);
    if (newSocketFileDescriptor < 0)
    	ROS_ERROR("ERROR on accept");
    bzero(buffer, 256);

    // numBytes = write(newsockfd,"I got your message",18);
    // if (numBytes < 0) fprintf(stderr, "ERROR writing to socket");
    while (1) {
    	//ROS_ERROR("Reading now");
    	//sleep(1);
        numBytes = read(newSocketFileDescriptor, buffer, MAX_LEN);

        if (numBytes < 0)
        {
        	ROS_ERROR("ERROR reading from socket");
        }
        else
        {
        	buffer[numBytes] = '\0';
            //ROS_INFO("Received from Socket: %s", buffer);

            if(atoi(buffer) > -1 && atoi(buffer) < 10)
            {
            	socket_reply = atoi(buffer);

            	ROS_INFO("Socet is %d",socket_reply );
            	if(socket_reply == 8)
            	{
            		ROS_INFO("In 8");
            		bzero(sendingBuffer,2);
            		strcpy(sendingBuffer, faceCount_msg);
            		//write(newSocketFileDescriptor,sendingBuffer,strlen(sendingBuffer));
            		ROS_INFO("Sending Info -- Faces Data %s", sendingBuffer );

            	}
            }//flush ??
            //break;
        }
    }//numBytes > 0

    close(newSocketFileDescriptor);
    close(socketFileDescriptor);

}
