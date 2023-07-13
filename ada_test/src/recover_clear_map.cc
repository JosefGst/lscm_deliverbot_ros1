/* Copyright (C) 2022 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, January 2022

 * This code is to clear costmap when robot is navigating but get stuck for a while. 
 * Set /clear_map/clearing_time to change clear frequency.
 * Default is 5.0s per clearance.
 */

#include <atomic>

#include "ros/ros.h"
#include "std_srvs/Empty.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Path.h"
#include "actionlib_msgs/GoalStatusArray.h"


class clearCostMapObject {
 private:
  ros::NodeHandle nh_;
  ros::ServiceClient clearcostmap_client_;
  ros::Subscriber sub_speed_;
  ros::Subscriber sub_status_;
  ros::Subscriber sub_path_;
  bool speed_flag_;
  bool status_flag_;
  bool path_flag_;
  ros::Duration clearing_time_;
  ros::Time zero_speed_start_time_;

  int counter_speed_;
  std::atomic<int> counter_path_;

  void statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg) {
    // if status == 1, means navigating.
    if (!msg->status_list.empty() && msg->status_list.back().status == 1) {
      status_flag_ = true;
    } else {
      status_flag_ = false;
    }
  }
  void pathCallback(const nav_msgs::Path::ConstPtr& msg) {
    // if poses is empty, means fail to find a path.
    if (status_flag_ && msg->poses.size() == 0) {
      ++counter_path_;
      if (counter_path_ > 25) {
        path_flag_ = true;
        counter_path_ = 0;
      }
    } else {
      counter_path_ = 0;
      path_flag_ = false;
    }
  }

  void speedCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    // If speed remains 0 for servaral seconds, it will execute clear map.
    // Speed is prior than path planning.
    if (status_flag_ && abs(msg->linear.x) < 0.001
        && abs(msg->angular.z) < 0.001) {
      // If speed_flag is false, start the counter
      ++counter_speed_;
      if (counter_speed_ > 20) {
        speed_flag_ = true;
        counter_speed_ = 0;
      }
    } else {
      counter_path_ = 0;  // Note that counter_path is modified here.
      counter_speed_ = 0;
      speed_flag_ = false;
    }
  }

 public:
  double clearing_time;
  clearCostMapObject() {
    // Subscribe the speed. About 5 Hz.
    sub_speed_ = nh_.subscribe("cmd_vel", 2,
                                &clearCostMapObject::speedCallback, this);
    // Subscribe the status. About 5 Hz.
    sub_status_ = nh_.subscribe("move_base/status", 2,
                                &clearCostMapObject::statusCallback, this);
    // Subscribe the path contents. About 5 Hz.
    sub_path_ = nh_.subscribe("move_base/GlobalPlanner/plan", 2,
                              &clearCostMapObject::pathCallback, this);
    // Clear costmap service.
    clearcostmap_client_ = nh_.serviceClient<std_srvs::Empty>(
                            "/move_base/clear_costmaps");

    // Set main loop 5Hz.
    ros::Rate rate(5);
    // Message to send for clearing costmap.
    std_srvs::Empty clear_costmap_msg;
    // Get clearing time from parameter server, or default 5s.
    if (nh_.getParam("/clear_map/clearing_time", clearing_time)) {
      clearing_time_ = ros::Duration(clearing_time);
      ROS_INFO("clearing_time param is %f", clearing_time);
    } else {
      ROS_INFO("clearing_time param not found.");
      // Default value is 5.0s.
      clearing_time_ = ros::Duration(5.0);
    }
    ROS_INFO("clearing costmap setup finished.");
    zero_speed_start_time_ = ros::Time::now();

    // Main loop
    while (ros::ok) {
      // The clear map will work only if status is ACTIVE.
      if (!status_flag_) {
        zero_speed_start_time_ = ros::Time::now();
        ros::spinOnce();
        rate.sleep();
        continue;
      }
      // If the cmd_vel is 0 or path is empty, clear map.
      if (!path_flag_ && !speed_flag_) {
        zero_speed_start_time_ = ros::Time::now();
        ros::spinOnce();
        rate.sleep();
        continue;
      }
      // If the cmd_vel keeps 0 in a few seconds, try clear map
      if (ros::Time::now() - zero_speed_start_time_ >
          ros::Duration(clearing_time_)) {
        if (path_flag_) ROS_INFO("path flag true");
        if (speed_flag_) ROS_INFO("speed flag true");
        if (clearcostmap_client_.call(clear_costmap_msg)) {
          ROS_INFO("Clearing costmap");
        } else {
          ROS_ERROR("Fail to clear costmap");
        }
        zero_speed_start_time_ = ros::Time::now();
        speed_flag_ = false;
        path_flag_ = false;
      }
      ros::spinOnce();
      rate.sleep();
    }
  }
  ~clearCostMapObject() {
    ros::shutdown();
  }
};  // End of class clearCostMapObject

int main(int argc, char** argv) {
  ros::init(argc, argv, "recover_clear_map");
  clearCostMapObject ccmo;
  return 0;
}
