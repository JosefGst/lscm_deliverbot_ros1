#!/usr/bin/env python2

import rospy
import vlc
from sensor_msgs.msg import LaserScan
import time
from std_msgs.msg import Int16
from actionlib_msgs.msg import GoalStatusArray
import threading

class PlaySound():
    def __init__(self):
        rospy.init_node('play_sound')
        self.player = vlc.Instance()
        self.media_list = self.player.media_list_new()
        self.media_player = self.player.media_list_player_new()
        #self.rate_status = rospy.Rate(5)
        #self.rate_lidar = rospy.Rate(10)
        self.music_file = "/home/lscm/deliverybot_ws/src/lscm_deliverybot/play_sounds/audio/Snappy R2D2.mp3"
        self.media = self.player.media_new(self.music_file)
        self.media_list.add_media(self.media)
        self.media_player.set_media_list(self.media_list)
        self.player.vlm_set_loop(self.music_file, True)
        rospy.Subscriber("scan_filtered", LaserScan, self.callback,queue_size=1)
        rospy.Subscriber("move_base/status", GoalStatusArray, self.process,queue_size=1)
        self.flag_lidar = False
        self.flag_moving = False
        self.flag_play = True
        self.th1 = threading.Thread(target=self.loop_play)
        self.th1.start()

    def loop_play(self):
        while self.flag_play:
            # print("flag_lidar is ", self.flag_lidar)
            # print("flag_moving is ", self.flag_moving)
            time.sleep(.5)
            while self.flag_lidar and self.flag_moving:
                self.media_player.play()
                time.sleep(2.5)

    def callback(self, msg):
        print("the minimum is ", min(msg.ranges))
        
        if min(msg.ranges) < 0.5:
        #if sum(sorted(msg.ranges)[:10])/10 < 0.5
            print("play")
            self.flag_lidar = True
        else:
            print("stop")
            self.flag_lidar = False
        #self.rate_lidar.sleep()

    def process(self, data):
        if len(data.status_list) != 0:
            if data.status_list[-1].status == 3:
                self.flag_moving = False
            elif data.status_list[-1].status == 1:
                self.flag_moving = True
        #self.rate_status.sleep()

    def myhook(self):
        print("shutdown time!")
        self.flag_play = False

if __name__ == '__main__':    
    print("ready to play sound")
    playsound = PlaySound()
    rospy.spin()
    rospy.on_shutdown(playsound.myhook)
