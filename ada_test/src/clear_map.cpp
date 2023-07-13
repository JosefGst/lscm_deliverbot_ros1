/* Copyright (C) 2021 Logistics and Supply Chain MultiTech R&D Centre <https://www.lscm.hk>
 * All Rights Reserved   
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ben Liu <bjliu@lscm.hk>, December 2021

 * This code is to maually clear costmap. 
 * Set /clear_map/clearing_time to change clear frequency.
 * Default is 5.0s per clearance.
 */

#include "ros/ros.h"
#include "std_srvs/Empty.h"

class clearCostMapObject {
 private:
  ros::NodeHandle _nh;
  ros::ServiceClient _clearcostmap_client;
  ros::Timer _clearcostmap_timer;
  ros::Duration _clearing_time;

  void ClearCostMapInit() {
    std_srvs::Empty clear_costmap;
    if (_clearcostmap_client.call(clear_costmap))
      ROS_INFO("Initial clear costmap");
    else
      ROS_ERROR("fail to clear costmap");
  }

  void ClearCostMapTimerCallBack(const ros::TimerEvent &event) {
    std_srvs::Empty clear_costmap;
    if (_clearcostmap_client.call(clear_costmap))
      ROS_INFO("clear costmap success in %f seconds", clearing_time);
    else
      ROS_ERROR("fail to clear costmap");
  }

 public:
  double clearing_time;
  clearCostMapObject() {
    _clearcostmap_client = _nh.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
    // clear the costmap once.
    ClearCostMapInit();

    // get clearing time from parameter server
    if (_nh.getParam("/clear_map/clearing_time", clearing_time)) {
      _clearing_time = ros::Duration(clearing_time);
      ROS_INFO("clearing_time param is %f", clearing_time);
    } else {
      ROS_INFO("clearing_time param not found!!");
      // default value is 5.0s.
      _clearing_time = ros::Duration(5.0d);
    }

    // create timer to clear costmap in every specific seconds.
    _clearcostmap_timer = _nh.createTimer(_clearing_time,
        &clearCostMapObject::ClearCostMapTimerCallBack, this);
  }
  ~clearCostMapObject() {
  }
};  // End of class clearCostMapObject

int main(int argc, char** argv) {
  ros::init(argc, argv, "clear_map");
  clearCostMapObject ccmo;
  ros::spin();
  return 0;
}
