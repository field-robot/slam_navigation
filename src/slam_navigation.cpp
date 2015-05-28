#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <std_msgs/Float32.h>
#include <nav_msgs/OccupancyGrid.h>

#define searchDistance 11.4
#define horizontalSearch 20
#define verticalSearch 20
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
double distancecovered;							//distance covered per row
double dt;


int gridmap[horizontalSearch][verticalSearch];
int row = 1;																			 	//number of rows
int numberFreeCells= 0;

bool leftright = false;																		//false is linksom; true is rechtsom
bool searchingRow;																			//boolean for start of searching



void ticksLeft( const std_msgs::Float32& ticks)
{    
    Pos1 = (ticks.data*(WheelDiameter*PI)/39*20); //based on encoder value   
    Pos1 *= dt;
      
}

void ticksRight( const std_msgs::Float32& ticks1)
{	
   Pos2 = (ticks1.data*(WheelDiameter*PI)/39*20); //based on encoder value
   Pos2 *= dt;
  
}

void odomMsgs(const nav_msgs::Odemetry& odom)						//callback function for the position of the robot
{
	x = odom.pose.pose.point.position.x;	
	y = odom.pose.pose.point.position.y;	
	w = odom.pose.pose.point.orientation.w;
}

void costmap_grid(const nav_msgs::OccupancyGrid &costmap)
{
	int k;
	for(int i = 0;i<costmap.map.info.width;i++)
		{
			for(int j = 0;j<costmap.map.info.height; j++)
			{
			gridmap[i][j] = costmap.map.data[k];
			k++ ;
			}
		}	
	
}

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "slam_navigation");
  ros::NodeHandle nh;
  
  ros::Subscriber sub = nh.subscribe("odom", 1, &odomMsgs);					//subscribed to odometry msg
  ros::Subscriber encoder_left = nh.subscribe("pub_speed_left",1, &ticksLeft);
  ros::Subscriber encoder_right = nh.subscribe("pub_speed_right",1, &ticksRight);
  ros::Subscriber local_costmap = nh.subscribe("/move_base/local_costmap/costmap",1,&costmap_grid);
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);
  
  ros::Rate loop_rate(10);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  
  ros::Time current_time, last_time;
  
  while (ros::ok()) {
		
		current_time = ros::Time::now();
      //CALCULATIONS
		
		distancecovered += (Pos1+Pos2)/2;
		if(searchingRow != true)
		{
			if (row == 1)
			{
				goal_x = 13;
				goal_y = 0;
				goal_w = 0.5*3.1415;
			}
			else 
			{
				if (row%2>0)
				{
					goal_x = 13;
				}
				else goal_x = -1;
			}
		}
							
		
		if (distancecovered > searchDistance)
		{
			searchingRow = true;
			for (int i = 30-0.5*horizontalSearch; i <horizontalSearch+30; i++)
			{
				for(int j = 30-0.5*verticalSearch; j<verticalSearch+30; j++)
				{
					if(gridmap[i][j] == 0)
					{
						numberFreeCells++;
					}
				}
			}
			if (numberFreeCells > 380)
			{
				goal_y+=0.75;
				goal_w += 3.1415;				
			}			
		}
		
		if(searchingRow == true)
		{
			if (y == goal_y-0.1|y == goal_y+0.1)
			{
				if (w == goal_w-0.1 | w == goal_w+0.1)
				{
					searchingRow = false;
					distancecovered = 0;
					row ++;
				}
			}
		}
				
					
					
			
		
      

      //END CALCULATIONS




	  move_base_msgs::MoveBaseGoal goal;

      //we'll send a goal to the robot to move 1 meter forward
      goal.target_pose.header.frame_id = "base_link";
      goal.target_pose.header.stamp = ros::Time::now();

      goal.target_pose.pose.position.x = goal_x;
      goal.target_pose.pose.position.y = goal_y;
      goal.target_pose.pose.orientation.w = goal_w;

      ROS_INFO("Sending goal");
      ac.sendGoal(goal);

      ac.waitForResult();

      if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Succesfully reached the goal");
      else
        ROS_INFO("Did not reach the goal. Your robot is fucked up");
	  
	  dt = (current_time-last_time).toSec();
	  last_time = current_time;
	  	
	  loop_rate.sleep();
	  ros::spinOnce();
      
  }
}
