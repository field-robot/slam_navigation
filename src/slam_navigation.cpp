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
double _row_width_;

int _turn_1;
int _row_1;


bool cornering;

bool startright = true;

int rowcount = 1;
int direction = 0;
int direction_old = 0;
int _free_cells_;
int _field_length_;
int init_ = 0;

int row = 1;																			 	//number of rows
int _num_rows_;
int numberFreeCells= 0;
int freeCell = 0;
int freeCell_prev = -1;
int _obstaclecells_;
int _rowcells_;

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
	
	
}

bool Cornering();
bool Begin_Row();
bool Next_Goal();
bool End_Row();
void sendGoal(double goal_x, double goal_y, double goal_w);


bool Cornering()
{
		goal_y += _row_width_;
		goal_x = x;
		goal_w += PI;
		sendGoal(goal_x, goal_y ,goal_w);
		while (!Begin_Row());
		return true;
}

bool Begin_Row()
{
	if (freeCell < _free_cells_){		
		ROS_INFO("Begin of row has been detected");
		Next_Goal();
		row++;
		return true;
		}		
}
			
bool Next_Goal()
{
	if (sin(w)>0){
		goal_x = _field_length_;
		}
	if (sin(w)<0){
		goal_x = 0;
		}
	sendGoal(goal_x, goal_y ,goal_w);
	return true;
}

bool End_Row()
{
	if (freeCell > _free_cells_){
		ROS_INFO("End of row has been detected");
		if(row<(_num_rows_-1)){
		Cornering();
		}
	}
}



void sendGoal(double goal_x, double goal_y, double goal_w)
{
      
	typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
	MoveBaseClient ac("move_base", true);
	move_base_msgs::MoveBaseGoal goal;
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

}

//typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "slam_navigation");
  ros::NodeHandle nh;
  ros::NodeHandle nh1("~"); 				//subscribing to a private parameter server
  ros::Subscriber sub = nh.subscribe("odom", 1, &odomMsgs);				//subscribed to odometry msg
  ros::Subscriber local_costmap = nh.subscribe("/move_base/local_costmap/costmap",1,&costmap_grid);
 
  nh1.param("free_cells_", _free_cells_,80);
  nh1.param("field_length", _field_length_, 13);
  nh1.param("row_width", _row_width_, 0.75);
  nh1.param("number_of_rows", _num_rows_, 20);
  nh1.param("obstaclecells", _obstaclecells_, 20);
  nh1.param("rowcells", _rowcells_, 20);

  //tell the action client that we want to spin a thread by default
  //MoveBaseClient ac("move_base", true);
  
  ros::Rate loop_rate(10);

  //wait for the action server to come up
    
	
	sendGoal(_field_length_, 0 ,0);			//sending robot to its first goal
	ros::Time current_time, last_time;

  while (ros::ok()) {
		
		current_time = ros::Time::now();
		while(!End_Row());
	   
	  
	 	 dt = (current_time-last_time).toSec();
	  	last_time = current_time;
	  	
	  	loop_rate.sleep();
	  	ros::spinOnce();
     
  }
}
