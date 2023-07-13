# lscm_deliverybot

with :robot: symbole is meant to run on the robot
with :computer: symbole is meant to run on the computer

# Installation

    sudo apt install python3-vcstool
    git clone https://github.com/JosefGst/lscm_deliverybot.git -b robot3
    cd lscm_deliverybot
    vcs import .. < my.repos
    rosdep install --from-paths src --ignore-src -r -y

<!-- ## :computer:Deliverybot Simulation

`roslaunch sim_world hospital.launch model:=deliverybot`  
![single_hospital](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/single_hospital.png)

`roslaunch deliverybot_nav navigation.launch model:=deliverybot`  
![single_deliverybot](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/single_deliverybot.png) -->

<!-- ## Multiple deliverybots Simulation

`roslaunch sim_world multi_hospital.launch`
![alt text](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/multi_hospital.png)

`roslaunch deliverybot_nav multi_nav_bringup.launch`
![alt text](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/multi_deliverybot.png) -->

# Real Robot :robot:

username: lscm  
pwd: 1

## :robot: Bringup

Start all sensors and motors on the robot.

    roslaunch deliverybot_bringup bringup_multi.launch

Ps: why is the launch file called "bringup_multi"? Because it'll add a prefix to all topics of the robot, e.g. "robot1". This way later multiple robots can be controlled in the same ros-network.

## :robot: Mapping

Still with the bringup running, open a new terminal and launch gmapping

    roslaunch deliverybot_slam gmappping.launch

:computer: you can also open rviz on your computer if the ros network is setup propperly

    rosrun rviz rviz -d src/lscm_deliverybot/deliverybot_slam/rviz/gmapping.rviz

![gmapping_real](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/gmapping_real.png)

:computer: use the keyboard to drive the robot around and create the map. The **\_\_ns** argument adds the namespace for the robot. e.g. "robot1 or robot3"

    rosrun teleop_twist_keyboard teleop_twist_keyboard.py __ns:=robot3 _speed:=.3 _turn:=.5

:robot: Change "map_name" to your liking and save the map.

    roscd deliverybot_nav/maps/
    rosrun map_server map_saver -f map_name __ns:=robot3

## :robot: Navigation

Close gmapping with `ctrl+c`. In case you closed the bringup, relaunch it and run the navigation launch file with your costume map "map_name".

    roslaunch deliverybot_nav multi_nav_bringup.launch map_name:=map_name

:computer:

    roscd deliverybot_nav
    rosrun rviz rviz -d rviz/multi_deliverybot_nav.rviz

![init_nav_real](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/init_nav_real.png)  
Use the third "2D Pose Estimate" button to set the initial position for robot3 on the map. The other Pose Estimate buttons are for robot1 and robot2.  
![nav_real](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/nav_real.png)  
With the "2D nav goal" button you can send a goal to the robot. It'll find a path and try to reach it.

# :robot: Send goals through a Python program

open the send_goal.py file:  
`/home/lscm/deliverybot_ws/src/lscm_deliverybot/medical_task/scripts/send_goal.py`  
the most important functions are:

    def movebase_client(prefix,x, y, a):	
    def door_control(prefix, msg):

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

    if __name__ == "__main__":
        rospy.init_node("movebase_client_py")
        clear_costmap = rospy.ServiceProxy('/robot3/move_base/clear_costmaps', Empty)

        result = execute_delivery()


Befor you run the python script you should launch the bringup and navigation.<br><br>
Run `python send_goal.py` in the terminal. Inside the main "execute_delivery" will be called. With `door_control('robot3',"3,2,8000,70,10")` and "2" at the third arg we close the door. Send a goal with movebase_client('robot3',1, -1, 0). The first argument is the prefix, followed by the x, y coordinates. For the orientation, set the last arg in radians corresponding to the map frame. Open the door with "1" as the third argument `door_control('robot3',"3,1,8000,70,10")`. Don't change the other parameters for the door command they are specific for the communication only and not important to understand.

<!-- ## Multiple deliverybots real
### Laptop

`roscore`
![](https://github.com/JosefGst/lscm_deliverybot/blob/robot3/images/split_screen_delivery.gif)

### Robot1

use branch **robot1**
source deliverybot_ws
`roslaunch deliverybot_bringup bringup_multi.launch`

### Robot2

use branch **robot2** "TODO"
source medical_ws
`roslaunch whbot_bringup bringup_multi.launch`

### Laptop

source deliverybot_ws
`roslaunch deliverybot_nav multi_nav_bringup.launch map_name:=ros_map12_mod` -->
