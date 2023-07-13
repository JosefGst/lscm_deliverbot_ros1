/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is to munally reset odom to 0.
 */

#include "ros/ros.h"
#include "std_msgs/String.h"

int main(int argc, char **argv) {
  ros::init(argc, argv, "reset_odom");
  ros::NodeHandle n;
  // Advertize the publisher on the topic you like
  ros::Publisher pub = n.advertise<std_msgs::String>("/pc_to_stm32", 100);

  std_msgs::String msg;
  msg.data = "clear";
  ros::Time beginTime = ros::Time::now();

  while (ros::ok()) {
    // ros::Duration secondsIWantToSendMessagesFor = ros::Duration(3);
    ros::Time endTime = beginTime + ros::Duration(5);
    while (ros::Time::now() < endTime) {
        pub.publish(msg);
        ros::Duration(0.5).sleep();
    }
    return 0;
  }
}
