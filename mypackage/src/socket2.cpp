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

void socket_client_talker();
void *thread_socket2(void* ptr);

#define Fn_STOP 1
#define Fn_FORWARD 2
#define Fn_BACKWARD 3
#define Fn_RIGHT 4
#define Fn_LEFT 5
#define Fn_WAKEUP 6
#define Fn_SLEEP 7
#define Fn_FACECOUNT 8
#define Fn_FACEFOLLOW 9

#define MAX_LEN 1024
int port_1 = 2062;
int port_2 = 2063;
int socket_reply = 9;

char faceCount_msg[10];
int faceCount_main;
int faceCount_flag = 1;
char sendingBuffer[2];
char input[9];
ros::Publisher move_pub;
geometry_msgs::Twist tw3;

void FaceCountNodeCallback(const std_msgs::String::ConstPtr& msg2)
{
	strcpy(faceCount_msg, msg2->data.c_str());
	faceCount_flag = 1;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "move_node");
	ros::NodeHandle nh;

	move_pub = nh.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 10);

	ros::Subscriber faceCount = nh.subscribe("faceCountMessage",1, FaceCountNodeCallback);

	char* message = "a";
	pthread_t tester;
	//pthread_create(&tester, NULL, thread_socket2, (void*)message);

	socket_client_talker();
	ros::spin();

	return 0;
}

void* thread_socket2(void* ptr)
{
	socket_client_talker();
}
void socket_client_talker()
{

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
    serverAddress.sin_port = htons(port_2);
    if (bind(socketFileDescriptor, (struct sockaddr *) &serverAddress,
             sizeof (serverAddress)) < 0) {
    	ROS_ERROR("ERROR, no port provided");

    }
    while(1){
    listen(socketFileDescriptor, 5);
    clilen = sizeof (clientAddress);
    newSocketFileDescriptor = accept(socketFileDescriptor,
                                  (struct sockaddr *) &clientAddress,
                                  &clilen);

    int pd = fork();

    if(pd !=0)
    {
    if (newSocketFileDescriptor < 0)
    	ROS_ERROR("ERROR on accept");

    close(newSocketFileDescriptor);
    }


    if(pd == 0)
    {
    bzero(buffer, 256);

    ROS_INFO("Child Created");
    // while (1)
    {
		ROS_INFO("Please enter your command:");
		char fd = read(STDIN_FILENO, input, 9);
		input[fd] = '/0';

		if(faceCount_flag == 1)
    	{
//			write(STDOUT_FILENO, input, fd);

    		numBytes = write(newSocketFileDescriptor,input , fd);
    		ROS_INFO("FLAG ONE");
    	}
    	else
    	{

    	}

        if (numBytes < 0)
        {
        	ROS_ERROR("ERROR writing to socket");
        }
        else
        {

        }

    }
    ROS_ERROR("DIE");
    close(newSocketFileDescriptor);
    close(socketFileDescriptor);

    }
    }
}
