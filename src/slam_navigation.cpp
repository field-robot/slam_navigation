#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <std_msgs/Float32.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Bool.h>


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
int _dMode_;

double angle_start = 0;
bool leftright = false;																		//false is linksom; true is rechtsom
bool searchingRow;																			//boolean for start of searching

uint8_t detectionMode = 0;					//1 is count rows next to the vehicle
								//2 is detect an obstacle in the field
								//3 is detect the end of a row

bool obstacleDetected = false;
bool rowDetected = false;
  nav_msgs::Odometry odom_log;
  std_msgs::UInt16   freecells_log;
rosbag::Bag bag;

void writeBag()
{
	odom_log.pose.pose.position.x = x;	
	odom_log.pose.pose.position.y = y;	
	odom_log.pose.pose.orientation.z = w;
	freecells_log.data = numberFreeCells;
	bag.write("Odometry ", ros::Time::now(), odom_log);
	bag.write("FreeCells ", ros::Time::now(), freecells_log);
}
	

void odomMsgs(const nav_msgs::Odometry& odom)						//callback function for the position of the robot
{
	x = odom.pose.pose.position.x;	
	y = odom.pose.pose.position.y;	
	w = odom.pose.pose.orientation.z;
}

void costmap_grid(const nav_msgs::OccupancyGrid &costmap)
{
	
	if (detectionMode == 0){					//detect rows
		for(int j=0;j++;j<costmap.info.height){							//creating a for loop which knows the height of the costmap
			for(int i=0.5*costmap.info.width;i++;i<costmap.info.width){	//creating a second for loop which knows the width of the costmamp
				if (costmap.data[(i)+(j*costmap.info.height)] == 0){	//checking for empty cells
					numberFreeCells++;
				}	
			}
		}
		if (numberFreeCells > _rowcells_) rowDetected = true;			
	}
	if (detectionMode == 1){					//detect obstacle
	int a = floor(0.25*costmap.info.width);		//searching a narrow strip in front of the robot
		for(int j=0;j++;j<costmap.info.height){		
			for(int i = a; i++; i < (costmap.info.width-a)){
				if (costmap.data[(i)+(j*costmap.info.height)] == 0){		//checking for empty cells
					numberFreeCells++;
				}
			}		
		}
		if (numberFreeCells > _obstaclecells_) obstacleDetected = true;
	}
	if (detectionMode == 2){					//detect end of row
		for (int i = 0;i<costmap.info.width*costmap.info.height;i++){	//checking all available cells
			if (costmap.data[i]==0) numberFreeCells++;					//checking for empty cells	
		}
	}
	ROS_INFO("NumberofFreeCells %i", numberFreeCells);
	freeCell = numberFreeCells;
	numberFreeCells = 0;
		
}
//declaring functions
bool Cornering();
bool Begin_Row();
bool Next_Goal();
bool End_Row();
void sendGoal(double goal_x, double goal_y, double goal_w);


bool Cornering()
{
		goal_y += _row_width_;					//adding row_width to the current goal
		goal_x = x;								//making sure that the x_goal stays the current x position
		goal_w += PI;							//adding pi to the current rotational goal
		sendGoal(goal_x, goal_y ,goal_w);		// sending the goal to the move_base pkg
		while (!Begin_Row());					//waiting untill the beginning of a row is detected
		return true;							// returning a true
}

bool Begin_Row()
{
	if (freeCell < _free_cells_){						//checking if the begin of a row is detected
		ROS_INFO("Begin of row has been detected");
		Next_Goal();									//starting the next goal procedure
		row++;											//adding a row to the count
		return true;
		}		
}
			
bool Next_Goal()
{
	if (sin(w)>0){										//check the rotation
		goal_x = _field_length_;						// set a new x goal
		}
	if (sin(w)<0){										//check the rotation
		goal_x = 0;										//set a new x goal
		}
	sendGoal(goal_x, goal_y ,goal_w);					//send a new goal
	return true;
}

bool End_Row()
{
	if (freeCell > _free_cells_){						//check if the end of the row is reached
		ROS_INFO("End of row has been detected");
		
		if(row<(_num_rows_-1)){							// start cornering procedure
		Cornering();
		}
	}
}



void sendGoal(double goal_x, double goal_y, double goal_w)	//sending a goal to the movebaseclient
{
      
	typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
	MoveBaseClient ac("move_base", true);
	move_base_msgs::MoveBaseGoal goal;
      goal.target_pose.header.frame_id = "base_link";
      goal.target_pose.header.stamp = ros::Time::now();

      goal.target_pose.pose.position.x = goal_x;			//adding x_goal in the message
      goal.target_pose.pose.position.y = goal_y;			//adding y_goal in the message
      goal.target_pose.pose.orientation.w = goal_w;			//adding w_goal in the message

     /*ROS_INFO("Sending goal");
      ac.sendGoal(goal);

      ac.waitForResult();

     if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("Succesfully reached the goal");
     else
       ROS_INFO("Did not reach the goal. Your robot is fucked up");
	*/
}
bool status = false;
void Exit(const std_msgs::UInt16 &state)
{
	
	
	if (state.data == 0)	{
		status=true;
		ROS_INFO("Exit has been detected");
	}	
	else status = false;
}
//typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "slam_navigation");
  ros::NodeHandle nh;
  ros::NodeHandle nh1("~"); 				//subscribing to a private parameter server
  ros::Subscriber sub = nh.subscribe("odom", 1, &odomMsgs);				//subscribed to odometry msg
  ros::Subscriber local_costmap = nh.subscribe("/move_base/local_costmap/costmap",1,&costmap_grid);
  ros::Subscriber exit= nh.subscribe("exit", 1, &Exit);
  nh1.param("free_cells_", _free_cells_,80);
  nh1.param("field_length", _field_length_, 13);
  nh1.param("row_width", _row_width_, 0.75);
  nh1.param("number_of_rows", _num_rows_, 20);
  nh1.param("obstaclecells", _obstaclecells_, 20);
  nh1.param("rowcells", _rowcells_, 20);
  nh1.param("detection_mode", _dMode_, 1);

  //tell the action client that we want to spin a thread by default
  //MoveBaseClient ac("move_base", true);
  
  ros::Rate loop_rate(10);
  



  bag.open("test_bag.bag", rosbag::bagmode::Write);
  ROS_INFO("Bag is opened");
  //wait for the action server to come up
    
	
	sendGoal(_field_length_, 0 ,0);			//sending robot to its first goal
	ros::Time current_time, last_time;

  while (ros::ok()) {
		
		current_time = ros::Time::now();
		if (_dMode_ == 1){
			detectionMode= 2;
			//while(!End_Row());
		}
		
		if (status == false){
			ROS_INFO("Writing bag");
			writeBag();
		}
		if (status == true){
			bag.close();
			ROS_INFO("Closing bag");
		}
	   	
	  
	 	 dt = (current_time-last_time).toSec();
	  	last_time = current_time;
	  	
	  	loop_rate.sleep();
	  	ros::spinOnce();
     
  }
	
}
