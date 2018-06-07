#include "ros/ros.h"
#include <dynamixel_workbench_msgs/JointCommand.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "nodding_driver_node");
  ros::NodeHandle n;

  ros::Rate r(0.2);

  ros::ServiceClient client = n.serviceClient<dynamixel_workbench_msgs::JointCommand>("joint_command");

	dynamixel_workbench_msgs::JointCommand srv;
	srv.request.unit = "raw";
	srv.request.id =1;
	srv.request.goal_position = 512;

  while(ros::ok())
  {
	dynamixel_workbench_msgs::JointCommand srv;
	srv.request.unit = "raw";
	srv.request.id =1;
	srv.request.goal_position = 378;
	if(client.call(srv))
	{
		//std::cout << srv.response.result <<std::endl;
	}

	r.sleep();

	dynamixel_workbench_msgs::JointCommand srv2;
	srv2.request.unit = "raw";
	srv2.request.id =1;
	srv2.request.goal_position = 649;
	if(client.call(srv2))
	{
		//std::cout << srv2.response.result <<std::endl;
	}

	r.sleep();
	
  }

}
