#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <std_msgs/Float32.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>

#define WheelDiameter 0.15
#define PI 3.1415

double x;
double y;
double w;
double goal_x;
double goal_y;
double goal_w;
double Pos1;
double Pos2;
double distancecovered;																		//distance covered per row
double dt;
double speed_x = 0;
double speed_omega = 0;
bool cornering;




int row = 1;																			 	//number of rows
int numberFreeCells= 0;
int freeCell = 0;

bool leftright = false;																		//false is linksom; true is rechtsom
bool searchingRow;																			//boolean for start of searching



void odomMsgs(const nav_msgs::Odometry& odom)						//callback function for the position of the robot
{
	x = odom.pose.pose.position.x;	
	y = odom.pose.pose.position.y;	
	w = odom.pose.pose.orientation.w;
}

void costmap_grid(const nav_msgs::OccupancyGrid &costmap)
{
	for (int i = 0;i<costmap.info.width*costmap.info.height;i++)
	{
	
			if (costmap.data[i]==0) numberFreeCells++;
			
	}
	ROS_INFO("NumberOfFreeCells %i",numberFreeCells);
	freeCell = numberFreeCells;
	numberFreeCells = 0;
	
}

//typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "slam_navigation");
  ros::NodeHandle nh;
   ros::Publisher cmdvel = nh.advertise<geometry_msgs::Twist>("cmd_vel",1);
  ros::Subscriber sub = nh.subscribe("odom", 1, &odomMsgs);					//subscribed to odometry msg
  ros::Subscriber local_costmap = nh.subscribe("/move_base/local_costmap/costmap",1,&costmap_grid);
  //tell the action client that we want to spin a thread by default
 // MoveBaseClient ac("move_base", true);
  
  ros::Rate loop_rate(10);

  //wait for the action server to come up
 // while(!ac.waitForServer(ros::Duration(5.0))){
  //  ROS_INFO("Waiting for the move_base action server to come up");
  //}
  
  ros::Time current_time, last_time;
  
  while (ros::ok()) {
		
		current_time = ros::Time::now();
		if (freeCell > 3000)
		{
			cornering = true;
			ROS_INFO("Free cells %i", freeCell);
		}

	 //while (cornering == true);
	  geometry_msgs::Twist vel;
	  
	  vel.linear.x = speed_x;
	  vel.angular.z = speed_omega;
	  
	  cmdvel.publish(vel);
/*
      //we'll send a goal to the robot to move 1 meter forward
      goal.target_pose.header.frame_id = "base_link";
      goal.target_pose.header.stamp = ros::Time::now();

      goal.target_pose.pose.position.x = goal_x;
      goal.target_pose.pose.position.y = goal_y;
      goal.target_pose.pose.orientation.w = goal_w;
*/
 //     ROS_INFO("Sending goal");
 //     ac.sendGoal(goal);

   //   ac.waitForResult();

   //   if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
     //   ROS_INFO("Succesfully reached the goal");
   //   else
    //    ROS_INFO("Did not reach the goal. Your robot is fucked up");
	  
	  dt = (current_time-last_time).toSec();
	  last_time = current_time;
	  	
	  loop_rate.sleep();
	  ros::spinOnce();
     
  }
}
