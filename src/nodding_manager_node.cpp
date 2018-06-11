/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This node is
*/


#include "nodding_manager.cpp"


void rDS(dynamixel_workbench_msgs::DynamixelStateList &msg){};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "nodding_driver_node");
  ros::NodeHandle nh;

  ros::Rate r(40);  //40Hz system.

  nodding_manager man(nh,1); //= nodding_manager::nodding_manager(n,1); //step size 1.

  //subscirber to current position
  //ros::Subscriber position_sub = nh.subscribe("/dynamixel_state", 5, &rDS);

  while(ros::ok())
  {
    man.loop();
    r.sleep();
  }

return 0;
}
