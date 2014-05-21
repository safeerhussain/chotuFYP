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

#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
using namespace std;
void socket_server_listener();
void manual_listener();

#define MAX_LEN 1024
int port_1 = 2034;
int socket_reply = 7;
ros::Publisher sock_pub;

std_msgs::String msg1;
std::stringstream ss;
std::string value;

char input[9];

int main(int argc, char **argv)
{
	ros::init(argc, argv, "socket1_node");
	ros::NodeHandle nh;

	sock_pub = nh.advertise<std_msgs::String>("socket1Message", 1);

	while(ros::ok)
	{
		manual_listener();
		//socket_server_listener();
	}	
	ros::spin();

	return 0;
}
void manual_listener(){
	bzero(input, 2);
	ROS_INFO("Please enter your command:");
	int fd = read(STDIN_FILENO, input, 9); 
	input[fd] = '/0';
	write(STDOUT_FILENO, input, fd);

	socket_reply = atoi(input);

	value = static_cast<ostringstream*>( &(ostringstream() << socket_reply) )->str();
	ss << value;
	msg1.data = ss.str();

	//ROS_INFO("sending ----> %s", msg1.data.c_str());
	sock_pub.publish(msg1);
	ss.str("");

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
    serverAddress.sin_port = htons(port_1);
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

            if(atoi(buffer) > -1 && atoi(buffer) < 15)
            {
            	socket_reply = atoi(buffer);

            	value = static_cast<ostringstream*>( &(ostringstream() << socket_reply) )->str();
				ss << value;
				msg1.data = ss.str();

				 //ROS_INFO("sending ----> %s", msg1.data.c_str());
				 sock_pub.publish(msg1);
				 ss.str("");
            	//ROS_INFO("Socet is %d",socket_reply );

            }//flush ??
            //break;
        }
    }//numBytes > 0

    close(newSocketFileDescriptor);
    close(socketFileDescriptor);

}
