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
double speed_x_prev = -1;
double speed_omega = 0;
double speed_omega_prev = -1;
bool cornering;

bool startright = true;

int rowcount = 1;
int direction = 0;
int direction_old = 0;


int row = 1;																			 	//number of rows
int numberFreeCells= 0;
int freeCell = 0;
int freeCell_prev = -1;
double angle_start = 0;
bool leftright = false;																		//false is linksom; true is rechtsom
bool searchingRow;																			//boolean for start of searching



void odomMsgs(const nav_msgs::Odometry& odom)						//callback function for the position of the robot
{
	x = odom.pose.pose.position.x;	
	y = odom.pose.pose.position.y;	
	w = odom.pose.pose.orientation.z;
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
	for (int j = 40; j<45;j++)
	{
		if (costmap.data[i]>127)
		{
			maxdistancer = i;
			}
	}
	
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
  if (rowcount == 1)
		{
			freeCell_prev = freeCell;
		}
  while (ros::ok()) {
		
		current_time = ros::Time::now();
		if ((freeCell) > 85)
		{
			cornering = true;
			angle_start = w;
			ROS_INFO("Free cells %i", freeCell);
		}
	 
	 	freeCell_prev = freeCell;
	 
	 while (cornering == true)
	 {
	 	if (rowcount%2 == 1)
	 	{
	 		direction = 1;
	 	}
	 	else direction = -1;
	 	speed_x = 0.5;
	 	speed_omega = direction;
	 	if (abs(angle_start -w)<0.3)
	 	{
	 		speed_omega = 0;
	 		rowcount++;
	 		cornering= false;
	 	}
	  }
	  if (rowcount == 15)
	  {
	  	speed_omega = 0;
	  	speed_x = 0;
	  }
	  if (speed_x != speed_x_prev | speed_omega != speed_omega_prev)
	  {
	  	geometry_msgs::Twist vel;
	  	 vel.linear.x = speed_x;
	 	 vel.angular.z = speed_omega;
	 	 speed_omega_prev = speed_omega;
	 	 speed_x_prev = speed_x;
	 	 cmdvel.publish(vel);
	  }
	  
	  
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
