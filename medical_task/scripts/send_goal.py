#!/usr/bin/env python

import rospy
import sys
import tf_conversions
import actionlib
from std_srvs.srv import Empty
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
from std_msgs.msg import String
from actionlib_msgs.msg import GoalID
import geometry_msgs.msg

global dbot_deliver, change_map, el_open_door, el_close_door, el_change_floor, clear_costmap

def movebase_client(prefix,x, y, a):
	
	client = actionlib.SimpleActionClient('/' + prefix + '/move_base', MoveBaseAction)
	client.wait_for_server()

	goal = MoveBaseGoal()
	goal.target_pose.header.frame_id = "map"
	goal.target_pose.header.stamp = rospy.Time.now()

	goal.target_pose.pose.position.x = x
	goal.target_pose.pose.position.y = y
	goal.target_pose.pose.position.z = 0.0

	quat = tf_conversions.transformations.quaternion_from_euler(
		0.0,
		0.0,
		a
	)

	goal.target_pose.pose.orientation.x = quat[0]
	goal.target_pose.pose.orientation.y = quat[1]
	goal.target_pose.pose.orientation.z = quat[2]
	goal.target_pose.pose.orientation.w = quat[3]
	
	client.send_goal(goal)
	wait = client.wait_for_result()
	rospy.loginfo("Sent Goal")
	if not wait:
		client.cancel_goal()
		rospy.logerr("Action server not available!")
		rospy.signal_shutdown("Action server not available!")
	else:
		return client.get_result()


def door_control(prefix, msg):
	pub = rospy.Publisher('/' + prefix + '/stm32_door_control', String, queue_size=1)
	rate = rospy.Rate(10)
	while not rospy.is_shutdown():
		for i in range(2):
			# rospy.loginfo(msg)
			pub.publish(msg)
			rate.sleep()
		break


def execute_delivery():
	# clear_costmap.call()
	while not rospy.is_shutdown():
		rospy.sleep(5)	
		door_control('robot3',"3,2,8000,70,10") # close
		movebase_client('robot3',1, -1, 0) # EL out
		door_control('robot3',"3,1,8000,70,10") #open
		rospy.sleep(5)
		movebase_client('robot3',1, -4, 1.5) # EL out
	return True


def cancle_move_base():
	cancel_pub = rospy.Publisher("/robot3/move_base/cancel", GoalID, queue_size=2)
	cancel_msg = GoalID()
	cancel_msg.id = ""
	cancel_pub.publish(cancel_msg)

def robot_state_callback(data):
    rospy.loginfo("I heard %s", data.data)

def get_robot_state():
	rospy.Subscriber("/robot3/stm32_to_pc", String, robot_state_callback)
	rospy.spin()

def cmd2stm():
	set_robot_state = rospy.Publisher("/robot3/pc_to_stm32", String, queue_size=2)
	rate = rospy.Rate(10)
	while not rospy.is_shutdown():
		for i in range(2):
			# rospy.loginfo("set to ros mode=1")
			set_robot_state.publish("0,3,0,1") 
			rate.sleep()
		break
	

		
if __name__ == "__main__":
	rospy.init_node("movebase_client_py")
	clear_costmap = rospy.ServiceProxy('/robot3/move_base/clear_costmaps', Empty)
	
	# cmd2stm()
	# print("published")
	# get_robot_state()
	result = execute_delivery()
