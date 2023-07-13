#!/usr/bin/env python2

import  vlc
import time

def play_music(filename, times=1):
    for i in range(times):
        p = vlc.MediaPlayer(filename)
        p.play()
        # time.sleep(4)
        p.stop()
