/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This node is
*/


#include "nodding_manager.cpp"
#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>


int main(int argc, char **argv)
{
  ros::init(argc, argv, "nodding_driver_node");
  ros::NodeHandle nh;

  ros::Rate r(50);  //50Hz system > 40Hz UST-20LX.

  nodding_manager man(nh); //= nodding_manager::nodding_manager(n,1); //step size 1.
  
  printf("Entering Control Loop \n");
  while(ros::ok())
  {
    man.loop();
    r.sleep();
  }

return 0;
}
