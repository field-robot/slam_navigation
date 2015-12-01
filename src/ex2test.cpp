#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <std_msgs/Float32.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Odometry.h>


uint8_t detectionMode = 0;					//1 is count rows next to the vehicle
								//2 is detect an obstacle in the field
								//3 is detect the end of a row

bool obstacleDetected = false;
bool rowDetected = false;

void costmap_grid(const nav_msgs::OccupancyGrid &costmap)
{
	
	if (detectionMode == 0){					//detect rows
		for(int j=0;j++;j<costmap.info.height){
			for(int i=0.5*costmap.info.width;i++;i<costmap.info.width){
				if (costmap.data[(i)+(j*costmap.info.height)] == 0){
					NumberFreecells++;
				}	
			}
		}
		if (NumberFreecells > 50) rowDetected = true;
	}
	if (detectionMode == 1){					//detect obstacle
	int a = floor(0.25*costmap.info.width);
		for(int j=0;j++;j<costmap.info.height){		
			for(int i = a; i++; i < (costmap.info.width-a)){
				if (costmap.data[(i)+(j*costmap.info.height)] == 0){
					NumberFreecells++;
				}
			}		
		}
		if (NumberFreecells > 32) obstacleDetected = true;
	}
	if (detectionMode == 2){					//detect end of row
		for (int i = 0;i<costmap.info.width*costmap.info.height;i++){	
			if (costmap.data[i]==0) numberFreeCells++;			
		}
	}
	ROS_INFO("NumberofFreeCells %i", numberFreeCells);
	freeCell = numberFreeCells;
	numberFreeCells = 0;
		
}


	


