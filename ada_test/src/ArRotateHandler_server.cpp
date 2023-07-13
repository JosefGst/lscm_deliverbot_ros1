#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <ada_test/ArRotateHandlerAction.h>

class ArRotateHandlerAction
{
protected:

  ros::NodeHandle nh_;
  actionlib::SimpleActionServer<ada_test::ArRotateHandlerAction> as_; // NodeHandle instance must be created before this line. Otherwise strange error occurs.
  std::string action_name_;
  // create messages that are used to published feedback/result
  ada_test::ArRotateHandlerFeedback feedback_;
  ada_test::ArRotateHandlerResult result_;

public:

  ArRotateHandlerAction(std::string name) :
    as_(nh_, name, boost::bind(&ArRotateHandlerAction::executeCB, this, _1), false),
    action_name_(name)
  {
    as_.start();
  }

  ~ArRotateHandlerAction(void)
  {
  }

  void executeCB(const ada_test::ArRotateHandlerGoalConstPtr &goal)
  {
    // helper variables
    ros::Rate r(2);
    bool success = true;
    //-----------------------THIS IS TUTORIAL SAMPLE---------------------//
    // push_back the seeds for the fibonacci sequence
    feedback_.test_sequence.clear();
    feedback_.test_sequence.push_back(0);
    feedback_.test_sequence.push_back(1);

    // publish info to the console for the user
    ROS_INFO("%s: Executing, creating counting sequence of order 20 with seeds %i, %i", action_name_.c_str(), feedback_.test_sequence[0], feedback_.test_sequence[1]);

    // start executing the action  REPLACE THESE LINES
    for(int i=2; i<=20; i++)
    {
      // check that preempt has not been requested by the client
      if (as_.isPreemptRequested() || !ros::ok())
      {
        ROS_INFO("%s: Preempted", action_name_.c_str());
        // set the action state to preempted
        as_.setPreempted();
        success = false;
        break;
      }
      feedback_.test_sequence.push_back(i);
      // publish the feedback
      as_.publishFeedback(feedback_);
      // this sleep is not necessary, the sequence is computed at 1 Hz for demonstration purposes
      r.sleep();
    }
    //------------------------LOOP ENDING---------------------------------//
    //STEP1 run arcode reading
    //STEP2 run robot rotating
    //STEP3 get intialpose

    
    if(success)
    {
      result_.pose.push_back(0.01); //TODO:to be edited, assuming this is initial pose.
      result_.pose.push_back(0.02);
      result_.pose.push_back(0.03);
      result_.pose.push_back(0.04);
      result_.pose.push_back(0.05);
      result_.pose.push_back(0.06);
      result_.pose.push_back(0.07);
      ROS_INFO("%s: Succeeded", action_name_.c_str());
      // set the action state to succeeded
      as_.setSucceeded(result_);
    }
  }


};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "arrotatehandler");

  ArRotateHandlerAction arrotatehandler("arrotatehandler");
  ros::spin();

  return 0;
}
