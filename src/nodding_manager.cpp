/*
*Author: Matthew King
*Date: June 8th, 2018
*
*This class is responsible for driving the Dynamixel position controller and for updating the corresponding TF frames.
*/

#include "nodding_manager.h"
#include <dynamixel_workbench_msgs/JointCommand.h>
#include<angles/angles.h>


nodding_manager::nodding_manager(ros::NodeHandle n, int stepsize)
:myNodeHandler(n),stepSize(stepsize), error(999), position(512)
{
  this->myClient = myNodeHandler.serviceClient<dynamixel_workbench_msgs::JointCommand>("joint_command");
  
  myDynamixel->begin(device_name.c_str(), dxl_baud_rate);
   
  if (!myDynamixel->scan(dxl_id_, &dxl_cnt_, scan_range))
  {
    ROS_ERROR("Not found Motors, Please check scan range or baud rate");
    ros::shutdown();
  }
  myDynamixel->addSyncWrite("Goal_Position");
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
  std::cout<<"Reseting Position!"<<std::endl;
  sleep(5);

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

//Recieves the state of the dynamixel.
void nodding_manager::recieveDynamixelState(int pos)
{  

  //.int pos = msg.dynamixel_state[0].present_position; //array would have multiple elements if there were multiple servos in use at one time.
  
  if(this->cw_CCWb) //ensure that a positive error always indicates a lag.
  {
    this->error=this->position - pos;
  }
  else
  {
    this->error=pos-this->position;
  }
  if(std::abs(this->error > 2))
  {
  std::cout<<"Compliance Error:"<<this->error<<std::endl;
  }


  return;
};

bool nodding_manager::loop()
{
  //ros::spinOnce();
  this->updateTF();
  this->updatePosition();
  this->writePosition();

  //this->sendJointCommand();
  
  return true;
};

//Update the the TF based on the current (estimated) position.
void nodding_manager::updateTF()
{
  //int pos=this->position;
  int pos = myDynamixel->itemRead(1, "Present_Position");
  double angle = 0.29*(this->position-512);  //512 is the horizontal offset. Resolution is 0.29 degrees.
  angle = angles::from_degrees(angle); //convert to radians.

  tf::Transform transform;
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.02) );
  tf::Quaternion q;
  q.setRPY(0, angle, 0);
  transform.setRotation(q);
  this->myBroadcaster.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "servo", "laser"));

  //this->recieveDynamixelState(pos); //check error margin (Not required).

  return;
};


void nodding_manager::updatePosition()
{
  if(this->cw_CCWb) //going clockwise
  {
    if(this->position < this->maxPosition)
    {
      this->position += this->stepSize;
    }
    else //if going clockwise but if at or somehow overshot the max then switch direction.
    {
     this->position -= this->stepSize;
     this->cw_CCWb= false;
    }
  }
  else  //going counterclockwise
  {
    if(this->position > this->minPosition)
    {
      this->position -=this->stepSize;
    }
    else //switch direction again.
    {
      this->position +=this->stepSize;
      this->cw_CCWb= true;
    }
  } 
  //if(!this->sendJointCommand())
  //{
  //  printf("Failed to send command!\n");
  //}
}

//Call the position controller.
bool nodding_manager::sendJointCommand()
{
  dynamixel_workbench_msgs::JointCommand srv;
  srv.request.unit = "raw"; //raw position encoding, as opposed to an angle.
  srv.request.id =1;  //Servo ID=1. 
  srv.request.goal_position = this->position;
  return myClient.call(srv);

}

void nodding_manager::writePosition()
{

myDynamixel->syncWrite("Goal_Position", &this->position);

}

