# 1.Test service and actionlib procedure

### Install necessary dependencies

```sh
$ sudo apt install ros-kinetic-laser-proc
$ sudo apt install ros-kinetic-urg-c
$ sudo apt install ros-kinetic-navigation
$ sudo apt install ros-kinetic-bfl
$ sudo apt install ros-kinetic-ecl

```
### In your catkin workspace

```sh
$ cd ~/investhkrobot_ros
$ catkin_make
$ echo "source ~/investhkrobot_ros/devel/setup.bash" >> ~/.bashrc  (optional)
$ source ~/.bashrc
```

### Make the Python node executable: 

```sh
$ chmod +x scripts/ar_rotate_handler_client.py
$ chmod +x scripts/ar_rotate_handler_server.py
```

### Test the service, returning type is float64[]
- `Service` can listen to the response and make sure the result can be heard, but service will send only one response normally.

		$ roscore
		$ rosrun ada_test ar_rotate_handler_server.py
		$ rosrun ada_test ar_rotate_handler_client.py

### Test the actionlib, returning type is float64[]
- `Action` can keep watching the whole process, but you have to listen to the ACTION/result topic, and the result will publish once only.

		$ roscore
		$ rosrun ada_test ArRotateHandler_server
		$ rosrun ada_test ArRotateHandler_client
