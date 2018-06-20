/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This node is
*/


#include "nodding_manager.cpp"
#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

nodding_manager* manG;

void rDS(const dynamixel_workbench_msgs::DynamixelStateList &msg)
{
    printf("Recieving dynamixel state \n");
	int pos = msg.dynamixel_state[0].present_position;
	manG->recieveDynamixelState(pos);
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "nodding_driver_node");
  ros::NodeHandle nh;

  ros::Rate r(50);  //40Hz system.

  //subscirber to current position
  ros::Subscriber position_sub = nh.subscribe("/dynamixel_state", 10, &rDS);

  nodding_manager man(nh,1); //= nodding_manager::nodding_manager(n,1); //step size 1.
  manG = &man;
  
  /*std::string device_name   = "/dev/ttyUSB0";
  uint32_t dxl_baud_rate    = 100000;

  uint8_t scan_range        = 10;

  uint32_t profile_velocity     =  200;
  uint32_t profile_acceleration =  50;

  myDynamixel = new DynamixelWorkbench;
  */

  printf("Entering Control Loop \n");
  while(ros::ok())
  {
    man.loop();
    r.sleep();
  }

return 0;
}
