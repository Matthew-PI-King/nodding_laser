/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This class is responsible for driving the Dynamixel position controller and for updating the corresponding TF frames.
*/
#include "ros/ros.h"
#include<tf/transform_broadcaster.h>
#include<dynamixel_workbench_msgs/DynamixelStateList.h>
#include<dynamixel_workbench_msgs/DynamixelState.h>
#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>



class nodding_manager
{
public:
nodding_manager(ros::NodeHandle n); //to try - 4, 11.
//~nodding_manager();

/*
*Loop calls the other private functions. Returns true unless something has gone wrong.
*/
bool loop();

private:
//Functions
void readPosition();
void updateTF();
bool updatePosition();

//Variables
ros::NodeHandle myNodeHandler;
tf::TransformBroadcaster myBroadcaster;


int minPosition; //40 degrees from vertical = 378
int maxPosition; //-40 degrees from vertical = 649

int position;

bool cw_CCWb=true; //clockwise or (not) counterclockwise. Clockwise herein defined to be increasing numerical value.

//Dynamixel toolbox stuff

  std::string device_name   = "/dev/ttyACM0";
  uint32_t dxl_baud_rate    = 1000000;

  uint8_t scan_range        = 10;

  uint32_t profile_velocity     =  200;
  uint32_t profile_acceleration =  50;

  DynamixelWorkbench myDynamixel; //*=new DynamixelWorkbench;
 
  uint8_t dxl_cnt_;
  uint8_t dxl_id_[12];
  


};
