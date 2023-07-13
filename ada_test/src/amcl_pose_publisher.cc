/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is to publish the pose via /amcl_tf_pose, which is estimated by AMCL in 10Hz.
 *
 * NOTED: 
 * The pose is not realtime updated, as AMCL will not update unless the robot moves.
 * Its only purpose is to make sure the pose keeps publishing. 
 */

#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

int main(int argc, char** argv) {
  ros::init(argc, argv, "amcl_pose_publisher");
  ros::NodeHandle node;
  ros::Publisher amcl_pose_publisher =
    node.advertise<geometry_msgs::PoseWithCovarianceStamped>("amcl_tf_pose", 10);

  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);

  ros::Rate rate(10.0);
  while (node.ok()) {
    geometry_msgs::TransformStamped transformStamped;
    try {
      transformStamped = tfBuffer.lookupTransform("map", "odom",
                               ros::Time::now());
    }
    catch (tf2::TransformException &ex) {
      ROS_WARN("%s", ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }

    geometry_msgs::PoseWithCovarianceStamped amcl_pose;
    amcl_pose.pose.pose.position.x = transformStamped.transform.translation.x;
    amcl_pose.pose.pose.position.y = transformStamped.transform.translation.y;
    amcl_pose.pose.pose.position.z = transformStamped.transform.translation.z;
    // tf::Quaternion q(0, 0, pose.pose.pose.orientation.z, pose.pose.pose.orientation.w);
    // tf::Matrix3x3 m(q);
    // double roll, pitch, yaw;
    // m.getRPY(roll, pitch, yaw);
    amcl_pose.pose.pose.orientation.x = transformStamped.transform.rotation.x;
    amcl_pose.pose.pose.orientation.y = transformStamped.transform.rotation.y;
    amcl_pose.pose.pose.orientation.z = transformStamped.transform.rotation.z;
    amcl_pose.pose.pose.orientation.w = transformStamped.transform.rotation.w;

    amcl_pose_publisher.publish(amcl_pose);

    rate.sleep();
  }
  return 0;
}


