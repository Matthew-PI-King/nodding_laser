/*
*Author: Matthew King
*Date: July 11th, 2018
*
*This node converts incoming laser scans into pointcloud2 format and waits for a transform into the map frame.
*/

#include "ros/ros.h"
#include <laser_geometry/laser_geometry.h>
#include <tf/transform_listener.h>
#include <tf/message_filter.h>

#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/LaserScan.h>

class mycloud_converter{
	public:
	mycloud_converter(int i)
	:nh(), tf_listener(), projector()
	{
	sub= nh.subscribe("scan", 5, &mycloud_converter::callback, this);
	pub= nh.advertise<sensor_msgs::PointCloud2>("pointcloud", 400);

	nh.param<std::string>(("/scan_converter_node/target_frame"), destination_frame, "laser_link"); //get the target frame!

	std::cout<<"Converter Initialized!"<<std::endl;
	};

	void callback(const sensor_msgs::LaserScan::ConstPtr &scan)
	{
	if(!tf_listener.waitForTransform(scan->header.frame_id, destination_frame, scan->header.stamp + ros::Duration().fromSec(scan->ranges.size()*scan->time_increment), ros::Duration(0.1)))
		return;

	sensor_msgs::PointCloud2 cloud;
	projector.transformLaserScanToPointCloud(destination_frame, *scan, cloud, tf_listener);

	pub.publish(cloud);

	}

	private:

	ros::NodeHandle nh;
	tf::TransformListener tf_listener;
	laser_geometry::LaserProjection projector;
	ros::Subscriber sub;
	ros::Publisher pub;
	std::string destination_frame;

};


int main(int argc, char** argv)
{
	ros::init(argc, argv, "scan_converter_node");

	std::cout<<"Initializing converter!"<<std::endl;
	mycloud_converter converter(1);
	std::cout<<"TESTING!!!!!!!!!"<<std::endl;
	ros::spin();

	return 0;
}

