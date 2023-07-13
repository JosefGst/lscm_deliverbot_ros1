#!/usr/bin/env python

from __future__ import print_function

import sys
import rospy
from ada_test.srv import *
import rosgraph

def ar_rotate_handler_client():
    rospy.wait_for_service('ar_rotate_handler')
    try:
        ar_rotate_handler = rospy.ServiceProxy('ar_rotate_handler', ArRotateHandler)
        resp1 = ar_rotate_handler()
        return resp1.pose
    except rospy.ServiceException as e:
        print("Service call failed: %s"%e)



if __name__ == "__main__":
    if rosgraph.is_master_online(): # Checks the master uri and results boolean (True or False)
        print("Requesting ArRotate")
        temp_list = ar_rotate_handler_client()
        for vec in temp_list:
            print(vec)
        #print("SUCCEED! The return value is %s" %ar_rotate_handler_client())
    else:
        print ("WARNING: ROS MASTER is Offline!")
