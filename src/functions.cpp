bool Cornering()
{
		goal_y += 0.75;
		goal_x = x;
		goal_z += PI;
		while (!Begin_Row());
		return true;
}

bool Begin_Row()
{
	if (free_cells < _free_cells_){		
		ROS_INFO("Begin of row has been detected");
		Next_Goal();
		return true;
		}		
}
			
bool Next_Goal()
{
	if (sin(w)>0){
		x_goal = 13;
		}
	if (sin(w)<0){
		x_goal = 0;
		}
	return true;
}

bool End_Row()
{
	if (free_cells > _free_cells_){
		ROS_INFO("End of row has been detected");
		Cornering();
	}
}
