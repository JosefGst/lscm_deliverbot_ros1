/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is to excute multiple goal navigation.
 * 
 */
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseArray.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Int8.h>
#include <string>
#include <map>
#include <cmath>
#include <vector>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

struct Location {
  int id;
  // std::string frame;
  move_base_msgs::MoveBaseGoal coordinate;
};

class Multigoal {
 private:
    int goal_id_, goal_count_, goal_stat_;
    const int gas_start_id_ = 1, gas_cancel_id_ = 2;
    std::vector<Location> locations_;
    ros::Publisher gas_pub_;
    ros::Subscriber sub_;
    // move_base_msgs::MoveBaseGoal goal;
    // void sendGoal(move_base_msgs::MoveBaseGoal &goal);
    void checkGasPoint(const int &goal_id);
    void getGoals();
    void check_status(const actionlib::SimpleClientGoalState &goal_stat);

 public:
    bool flag_loop = true;
    bool flag_reached = false;
    bool flag_sended = false;
    explicit Multigoal(ros::NodeHandle nh);
    ~Multigoal();
};
Multigoal::Multigoal(ros::NodeHandle nh) :goal_id_(0),
        goal_count_(0), goal_stat_(0) {
  // goal_id_ = 0;
  // goal_count_ = 0;
  // goal_stat_ = 0;
  ros::Rate loop_rate(1);
  // odom_sub = nh.subscribe("/husky_velocity_controller/odom",
      // 1, &Multigoal::odomCallback,this);
  gas_pub_ = nh.advertise<std_msgs::Int8>("stm_disinfector_control", 10);
  getGoals();
  MoveBaseClient ac("/move_base", true);
  while (ros::ok() && !ac.waitForServer(ros::Duration(5.0))) {
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  while (ros::ok()) {
    check_status(ac.getState());
    ros::spinOnce();
    move_base_msgs::MoveBaseGoal goal;
    goal = locations_[goal_id_].coordinate;
    goal.target_pose.header.stamp = ros::Time::now();
    ROS_INFO("Move to goal %i", goal_id_);
    ac.sendGoal(goal);
    ac.waitForResult();
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("Hooray, the base moved 1 meter forward");
    else
      ROS_INFO("The base failed to move forward 1 meter for some reason");
    ros::spinOnce();
    loop_rate.sleep();
  }
}

Multigoal::~Multigoal() {
}

void Multigoal::getGoals() {
  Location location0, location1, location2, location3;
  geometry_msgs::PoseStamped temp_pose;
  temp_pose.header.frame_id = "map";
  location0.id = 0;
  location0.coordinate.target_pose = temp_pose;
  location0.coordinate.target_pose.pose.position.x = -0.11;
  location0.coordinate.target_pose.pose.position.y = -1.00;
  location0.coordinate.target_pose.pose.orientation.z = 0.15;
  location0.coordinate.target_pose.pose.orientation.w = 0.99;

  location1.id = 1;
  location1.coordinate.target_pose = temp_pose;
  location1.coordinate.target_pose.pose.position.x = 2.05;
  location1.coordinate.target_pose.pose.position.y = -0.21;
  location1.coordinate.target_pose.pose.orientation.z = 0.15;
  location1.coordinate.target_pose.pose.orientation.w = 0.99;

  location2.id = 2;
  location2.coordinate.target_pose = temp_pose;
  location2.coordinate.target_pose.pose.position.x = 2.53;
  location2.coordinate.target_pose.pose.position.y = 1.29;
  location2.coordinate.target_pose.pose.orientation.z = -0.64;
  location2.coordinate.target_pose.pose.orientation.w = 0.77;

  location3.id = 3;
  location3.coordinate.target_pose = temp_pose;
  location3.coordinate.target_pose.pose.position.x = 6.22;
  location3.coordinate.target_pose.pose.position.y = -0.54;
  location3.coordinate.target_pose.pose.orientation.z = 0.99;
  location3.coordinate.target_pose.pose.orientation.w = -0.12;

  locations_.push_back(location0);
  locations_.push_back(location1);
  locations_.push_back(location2);
  locations_.push_back(location3);
}

void Multigoal::checkGasPoint(const int &goal_id) {
  std_msgs::Int8 gas_start_signal, gas_cancel_signal;
  gas_start_signal.data = 1;
  gas_cancel_signal.data = 0;
  if (goal_id == gas_start_id_) {
    gas_pub_.publish(gas_start_signal);
  }
  if (goal_id == gas_cancel_id_) {
    gas_pub_.publish(gas_cancel_signal);
  }
}

void Multigoal::check_status(const actionlib::SimpleClientGoalState &goal_stat) {
  // ROS_INFO("goal_status %i",goal_status);
  // ACTIVE=1, SUCCEEDED=3, ABORTED=4
  if (goal_stat == actionlib::SimpleClientGoalState::ACTIVE) {
    ROS_INFO("goal_sended ");
    flag_reached = false;
    flag_sended = false;
  } else if (goal_stat == actionlib::SimpleClientGoalState::SUCCEEDED) {
    ROS_INFO("goal reached");
    flag_reached = true;
    flag_sended = false;
    // starting from location1
    goal_count_ += 1;
    goal_id_ += 1;
    // add gas emitting judge
    checkGasPoint(goal_id_);
    if (goal_id_ >= locations_.size() && flag_loop) goal_id_ = 0;
  } else if (goal_stat == actionlib::SimpleClientGoalState::ABORTED) {
    ROS_INFO("goal failed! ");
    flag_reached = false;
    flag_sended = false;
  }
}
int main(int argc, char** argv) {
    ros::init(argc, argv, "bing_multi");
    ros::NodeHandle nh;
    Multigoal Multigoal(nh);
    // ros::Rate rate(5);
    // ros::spin();
    return 0;
}

