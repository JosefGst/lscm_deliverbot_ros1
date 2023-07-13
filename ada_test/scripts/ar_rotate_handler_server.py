#!/usr/bin/env python

import rospy
import rosgraph
import tf
from std_msgs.msg import String
from fiducial_msgs.msg import FiducialTransformArray, FiducialTransform
from geometry_msgs.msg import Transform
from ada_test.srv import ArRotateHandler, ArRotateHandlerResponse
#from __future__ import print_function


temp_buffer = [-1000, -1, -2, -3, -4, -5, -6]   #default value, if -1000 means invalid value

def handle_ar_rotate_handler(req):
    print("Starting excute reading AR actions")
    rospy.sleep(5) #sleep 5 seconds to wait for turning 360 degrees
    #TODO: turn 360 algorithm
    rospy.loginfo("Service heard %s", temp_buffer)
    return ArRotateHandlerResponse(temp_buffer)

def callback(data):
    if (data.transforms) :
        #THIS IS A REVISED VERSION, LEAVES THE roll and pitch angle to 0
        (r, p, y) = tf.transformations.euler_from_quaternion([data.transforms[0].transform.rotation.x, data.transforms[0].transform.rotation.y, data.transforms[0].transform.rotation.z, data.transforms[0].transform.rotation.w])
        quaternion = tf.transformations.quaternion_from_euler(0, 0, y)
        temp_buffer[0] = data.transforms[0].transform.translation.x
        temp_buffer[1] = data.transforms[0].transform.translation.y
        temp_buffer[2] = data.transforms[0].transform.translation.z
        temp_buffer[3] = 0 # fix the value 0 for 2D pose rotation.x
        temp_buffer[4] = 0 # fix the value 0 for 2D pose rotation.y
        temp_buffer[5] = quaternion[2] #rotation.z
        temp_buffer[6] = quaternion[3] #rotation.w
        rospy.loginfo(rospy.get_caller_id() + "I heard transform x is %f, y is %f", r, y)
    else: 
        print("I heard nothing")
        temp_buffer[0] = -1000
    


def ar_rotate_handler_server():
    rospy.init_node('ar_rotate_handler_server')
    rospy.Service('ar_rotate_handler', ArRotateHandler, handle_ar_rotate_handler)  #service for send initial pose
    rospy.Subscriber("fiducial_transforms", FiducialTransformArray, callback)  # the aruco topic to be subscribed and share the value to service
    print("Ready to receive service.")
    rospy.spin()

if __name__ == "__main__":
    if rosgraph.is_master_online():
        ar_rotate_handler_server() 
    else:
        print ("WARNING: ROS MASTER is Offline!")
