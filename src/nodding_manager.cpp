/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This class is responsible for driving the Dynamixel position and for updating the corresponding TF frames.
*/

#include "nodding_manager.h"
#include <dynamixel_workbench_msgs/JointCommand.h>
#include <angles/angles.h>
#include <math.h>


nodding_manager::nodding_manager(ros::NodeHandle n)
:myNodeHandler(n), position(512), myDynamixel()
{

  myDynamixel.begin(device_name.c_str(), dxl_baud_rate);
   
  if (!myDynamixel.scan(dxl_id_, &dxl_cnt_, scan_range))
  {
    ROS_ERROR("Not found Motors, Please check scan range or baud rate");
    ros::shutdown();
  }
  //myDynamixel.addSyncWrite("Goal_Position");
  //Initlialize location to horizontal position.
  //this->sendJointCommand(); 
  //std::cout<<"STARTING!"<<std::endl;
 // sleep(1);
  //ros::spinOnce();
  /*while(std::abs(this->error) > 2 and ros::ok())
  {
	sleep(1);
    //ros::spinOnce();
	std::cout<<"Error Margin: "<<this->error<<". Waiting..."<<std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(200)); //wait for intitial horizontal position.
  }
  */
  std::string node_namespace = myNodeHandler.getNamespace();
  std::cout<<node_namespace<<std::endl;
  int speed;
  if(!myNodeHandler.getParam(("/nodding_manager_node/goal_speed"), speed)) //could use n.param("goal_speed",speed,100) and remove if statement.
  {
	speed=100; //default value if somehow not set in launch file.
  } 
  std::cout<<"Speed: "<<speed<<std::endl;
  myDynamixel.goalSpeed(1, speed);

  std::cout<<"Reseting Position!"<<std::endl; 
  myDynamixel.goalPosition(1, 512);
  
  //do some other stuff while sleeping otherwise.

  //get max & min positions
  int maxp;
  //if(!myNodeHandler.getParam("max_position", maxp))
  //{
  //	std::cout<<"Couldn't find parameter"<<std::endl;
  //	maxp=649; //originally 649
  //}

  myNodeHandler.param(("/nodding_manager_node/max_position"), maxp, 649);

  maxPosition = maxp;
  std::cout<<"maxPosition: "<<maxPosition<<std::endl;
  int minp;
  myNodeHandler.param<int>(("/nodding_manager_node/min_position"), minp, 374);
  minPosition=minp;
  std::cout<<"minPosition: "<<minPosition<<std::endl;
  sleep(5);

  myDynamixel.goalPosition(1, maxPosition);

  /*dynamixel_workbench_msgs::JointCommand srv;
  srv.request.unit = "raw"; //raw position encoding, as opposed to an angle.
  srv.request.id =1;  //Servo ID=1. 
  srv.request.goal_position = 512;
  while(!myClient.call(srv))
  {
  std::cout<<"Connecting..."<<std::endl;
  }   
  sleep(5);
   */   

  std::cout<<"Startup Complete!"<<std::endl;
};

bool nodding_manager::updatePosition()
{
	if(cw_CCWb)
	{
		if(position >= maxPosition-3)
		{
			cw_CCWb = false;
			return myDynamixel.goalPosition(1, minPosition);
		}
	}
	else
	{
		if(position <= minPosition+3)
		{	
			cw_CCWb = true;
			return myDynamixel.goalPosition(1, maxPosition);
		}
	}
return true; //still between goals
};


void nodding_manager::readPosition()
{
	position= myDynamixel.itemRead(1, "Present_Position");
};


bool nodding_manager::loop()
{
  readPosition();
  updateTF();
  //std::cout<<"Current Position: "<<position<<std::endl;
  return updatePosition();
};

//Update the the TF based on the current (estimated) position.
void nodding_manager::updateTF()
{
  double angle = 0.29*(position-512);  //512 is the horizontal offset. Resolution is 0.29 degrees.
  angle = angles::from_degrees(angle); //convert to radians.

  tf::Transform transform;
									//X ,y , Z
  transform.setOrigin( tf::Vector3(0.0254*sin(angle), 0.0, 0.0254*cos(angle)) ); //calibrate
  tf::Quaternion q;
  q.setRPY(0, angle, 0);
  transform.setRotation(q);
  myBroadcaster.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "camera", "laser"));

  return;
};





