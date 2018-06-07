#include<tf/transform_broadcaster.h>
#include<ros/ros.h>
//#include<dynamixel_msgs/JointState.h>
#include<dynamixel_workbench_msgs/DynamixelStateList.h>
#include<dynamixel_workbench_msgs/DynamixelState.h>
#include<angles/angles.h>

/* This node publishes the tf between the laser scan and the servo.  This is based on the angle published by the servo. */

//Module that applies transform to laser scan of tilting hokuyo laser
using namespace std;



//Recieves position values from dynamixel servo and uses angle to apply transform to laser scan
//void obtainValues(const dynamixel_msgs::JointState &msg) 
void obtainValues(const dynamixel_workbench_msgs::DynamixelStateList &msg)
{
    //gets position from message
    int pos = msg.dynamixel_state[0].present_position;
	double angle = 0.29*(pos-512);
    angle = angles::from_degrees(angle);
    //perform transform
    static tf::TransformBroadcaster br;
    tf::Transform transform;
    transform.setOrigin( tf::Vector3(0.0, 0.0, 0.02) );
    tf::Quaternion q;
    q.setRPY(0, angle, 0);
    transform.setRotation(q);
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "servo", "laser"));
}

//main
int main(int argc, char **argv) 
{
    //initialize
    ros::init(argc, argv, "tilt_transform");
    ros::NodeHandle nh;
  
    //subscirber to current position
    ros::Subscriber position_sub = nh.subscribe("/dynamixel_state", 5, &obtainValues);

    //wait for updates in position
    ros::spin();
}
