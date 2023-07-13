/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is to add timestamp for odom data.
 * As the odom messages from stm32 don't contain time. 
 */

#include <ros/ros.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <nav_msgs/Odometry.h>

class SubscribeAndPublish {
 public:
  SubscribeAndPublish() {
    pub_ = n_.advertise<nav_msgs::Odometry>("odom", 1);
    sub_ = n_.subscribe("stm32_odom", 1,
      &SubscribeAndPublish::posecallback, this);
  }

  void posecallback(const nav_msgs::Odometry::ConstPtr& msg) {
    nav_msgs::Odometry odom;
    odom = *msg;
    odom.header.stamp = ros::Time::now();
    odom.header.frame_id = "robot3/odom";
    odom.child_frame_id = "robot3/base_link";
    // odom.pose = msg->pose;
    // odom.twist = msg->twist;
    odom.pose.covariance[0] = 1e-5;
    odom.pose.covariance[7] = 1e-5;
    odom.pose.covariance[14] = 1e6;
    odom.pose.covariance[21] = 1e6;
    odom.pose.covariance[28] = 1e6;
    odom.pose.covariance[35] = 1e-3;
    odom.twist.covariance = odom.pose.covariance;
    odom.twist.covariance[0] = 0.0001;
    odom.twist.covariance[7] = 0.0001;
    odom.twist.covariance[35] = 1e-3;

    // if(odom_pub)  odom_pub.publish(odom);
    pub_.publish(odom);
  }
  ~SubscribeAndPublish() {
  }

 private:
  ros::NodeHandle n_;
  ros::Publisher pub_;
  ros::Subscriber sub_;
};  // End of class SubscribeAndPublish

int main(int argc, char **argv) {
  // Initiate ROS
  ros::init(argc, argv, "odom_add_time");
  SubscribeAndPublish SAPObject;

  ros::spin();
  return 0;
}
