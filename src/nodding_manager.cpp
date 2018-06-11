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
:myNodeHandler(n),stepSize(stepsize), error(999)
{
  this->myClient = myNodeHandler.serviceClient<dynamixel_workbench_msgs::JointCommand>("joint_command");
  //Initialize location to horizontal position.
  this->sendJointCommand(); 
 
  while(this->error > 2)
  {
    //std::this_thread::sleep_for(std::chrono::milliseconds(200)); //wait for intitial horizontal position.
  }

};

//Recieves the state of the dynamixel.
void nodding_manager::recieveDynamixelState(dynamixel_workbench_msgs::DynamixelStateList &msg)
{  

  int pos = msg.dynamixel_state[0].present_position; //array would have multiple elements if there were multiple servos in use at one time.
  
  if(this->cw_CCWb) //ensure that a positive error always indicates a lag.
  {
    this->error=this->position - pos;
  }
  else
  {
    this->error=pos-this->position;
  }


  return;
};

bool nodding_manager::loop()
{
  this->updateTF();
  this->updatePosition();

  if(std::abs(this->error > 2))
  {
  std::cout<<"Error:"<<this->error<<std::endl;
  }

  return true;
};

//Update the the TF based on the current (estimated) position.
void nodding_manager::updateTF()
{
  double angle = 0.29*(this->position-512);  //512 is the horizontal offset. Resolution is 0.29 degrees.
  angle = angles::from_degrees(angle); //convert to radians.

  tf::Transform transform;
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.02) );
  tf::Quaternion q;
  q.setRPY(0, angle, 0);
  transform.setRotation(q);
  this->myBroadcaster.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "servo", "laser"));

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


