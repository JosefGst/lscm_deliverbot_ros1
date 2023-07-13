#!/usr/bin/env python2
from Tkinter import *
from send_goal import *
import rospy
from threading import *
from play_sound import play_music
from time import sleep

rospy.set_param('/robot3/loc_ind', 0)

start_pos = [0, 0, 0]
# goal_pos = [21.5, 34, -1.5]

robot_state = 0 # 0 ros/ 1 joystick

rospy.init_node("gui_send_goal")

# creating tkinter window
root = Tk()
root.geometry("700x350")
root.configure(background="white")



# Adding widgets to the root window
Label(root, text = 'LSCM Deliverybot',bg="white", font =('Verdana', 32)).pack(side = TOP, pady = 10)
emergancy_label = Label(root, text = '',bg="white", fg="red", font =('Verdana', 24))
emergancy_label.pack()

# Creating a photoimage object to use image
photo = PhotoImage(file = "/home/lscm/deliverybot_ws/src/lscm_deliverybot/medical_task/include/startnext.png")
photoimage = photo.subsample(1,1)

# add Clear costmap service
clear_costmap1 = rospy.ServiceProxy('/robot1/move_base/clear_costmaps', Empty)
clear_costmap3 = rospy.ServiceProxy('/robot3/move_base/clear_costmaps', Empty)

def threading():
    # Call work function
    # t1=Thread(target=send_goal1)
    # t1.start()
    t3=Thread(target=send_goal3)
    
    t3.start()
    

def close_window():
    cancle_move_base()
    rospy.sleep(1)
    cancle_move_base()
    root.destroy()
    exit()


    




def send_goal3():
    rospy.loginfo("started robot3")
    movebase_client('robot3', start_pos[0], start_pos[1], start_pos[2]) # rotate to goal

def robot_state_callback(data):
    # rospy.loginfo("I heard %s", data.data)
    state = data.data.split(",")
    # print(state[1] + state[3])
    global robot_state
    if int(state[1]) == 5 and int(state[3]) == 0:
        robot_state = 0
        print('Currently in ROS Mode')
        mode_btn['text']='Currently in ROS Mode'

    elif int(state[1]) == 5 and int(state[3]) == 1:
        robot_state = 1
        print('Currently in Joystick Mode')
        mode_btn['text']='Currently in Joystick Mode'

    elif (int(state[1]) == 7 or int(state[1]) ==6) and int(state[3]) == 1:
        robot_state = 1
        print('Emergancy button is pressed!!!')
        emergancy_label.config(text = 'Emergancy button is pressed!!!\n Relaunch ROS after release or set initial position in Rviz')

    elif (int(state[1]) == 7 or int(state[1]) ==6) and int(state[3]) == 0:
        robot_state = 1
        print('Emergancy button is released!')
        emergancy_label.config(text = '')
    
 

def get_robot_state():
	rospy.Subscriber("/robot3/stm32_to_pc", String, robot_state_callback)
	return robot_state_callback

def set_state(state):
    set_robot_state = rospy.Publisher("/robot3/pc_to_stm32", String, queue_size=2)
    
    set_robot_state.publish("0,3," + str(state))
    rate = rospy.Rate(50)
    while not rospy.is_shutdown():
        for i in range(2):
            # print("0,3," + str(state))
            set_robot_state.publish("0,3," + str(state))
            rate.sleep()
        break

def req_emergency_state():
    set_robot_state = rospy.Publisher("/robot3/pc_to_stm32", String, queue_size=2)
    
    set_robot_state.publish("0,6")
    rate = rospy.Rate(50)
    while not rospy.is_shutdown():
        for i in range(2):
            set_robot_state.publish("0,6")
            rate.sleep()
        break

def toggle_joystick_mode():
    global robot_state
    robot_state = 1 - robot_state #toggle between 1 and 0
    set_state(robot_state)
    if robot_state:
        mode_btn['text']='Currently in Joystick Mode'
    else:
        mode_btn['text']='Currently in ROS Mode'
    
    
def init():
    print("init")
    set_state(robot_state)
    req_emergency_state()
    sleep(2) # wait some time till rospublisher is set up
    req_emergency_state()
    set_state(robot_state)


t0=Thread(target=init)
t0.start()
    

# Joystick Mode Button
mode_btn = Button(root, text = 'Current Mode: ROS', font =('none', 32),command=toggle_joystick_mode)
mode_btn.pack()
# Start Button
Button(root, image=photoimage,command=threading,relief=FLAT,borderwidth=0, background="white").pack()
# Exit Button
Button(root, text="Exit", font= "none 32",command=close_window, background="white").pack(side = BOTTOM)




root.attributes('-fullscreen',True)
get_robot_state()
root.mainloop()