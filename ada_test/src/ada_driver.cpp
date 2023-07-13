/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is for connecting to stm32 board and provide transform from odom -> base_link.
 * This code is deprecated.
 */
#include <ros/ros.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <nav_msgs/Odometry.h>

// std::string turtle_name;

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
  static tf2_ros::TransformBroadcaster odom_broadcaster;
  geometry_msgs::TransformStamped transformStamped;
  transformStamped.header.stamp = ros::Time::now();
  transformStamped.header.frame_id = "odom";
  transformStamped.child_frame_id = "base_link";
  transformStamped.transform.translation.x = msg->pose.pose.position.x;
  transformStamped.transform.translation.y = msg->pose.pose.position.y;
  transformStamped.transform.translation.z = msg->pose.pose.position.z;
  // tf2::Quaternion q;
  // q.setRPY(0, 0, msg->theta);
  transformStamped.transform.rotation.x = msg->pose.pose.orientation.x;
  transformStamped.transform.rotation.y = msg->pose.pose.orientation.y;
  transformStamped.transform.rotation.z = msg->pose.pose.orientation.z;
  transformStamped.transform.rotation.w = msg->pose.pose.orientation.w;

  odom_broadcaster.sendTransform(transformStamped);
  // ROS_INFO(“INFO message %d”, ros::Time::now())
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "ada_driver");
  ros::NodeHandle node;
  ros::Subscriber sub = node.subscribe("/stm32_odom", 5, &poseCallback);
  ros::spin();
  return 0;
}
