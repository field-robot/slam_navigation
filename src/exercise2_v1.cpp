

#define FirstTurn	R
#define FirstRows	2
#define SecondTurn	L
#define SecondRows	1
#define ThirdTurn	R
#define ThirdRows	3
#define FourthTurn	R
#define FourthRows	1
#define FifthTurn	L
#define FifthRows	2
#define SixthTurn	-
#define SixthRows	-
#define SeventhTurn	-
#define SeventhRows	-
#define EigthTurn	-
#define EightRows	-
#define NinthTurn	-
#define	NinthRows	-
#define TenthTurn	-
#define	TenthRows	-

int loops = 0;

void setGoal()
{
	if (loops==0)
	{
		goal_x = 13;
		goal_y = 0;
		goal_w = 0.5*3.1415;
	}
	if (((X-goal_x)<0.5)&& ((Y-goal_y)<0.1)&&((W-goal_w)<0.2)|loops>1)
	{
	switch(loops)
	{
		int dir = 0;
		case 1:
			if (FirstTurn=='R')
			{
				dir = cos(W)
			}
			if (FirstTurn=='L')
			{
				dir = -cos(W);
			}
			if (FirstTurn =='-')break;
			goal_y += dir*FirstRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 0;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;
		case 2:
			if (SecondTurn=='R')
			{
				dir = cos(W)
			}
			if (SecondTurn=='L')
			{
				dir = -cos(W);
			}
			if (SecondTurn =='-')break;
			goal_y += dir*SecondRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 13;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;	
		case 3:
			if (ThirdTurn=='R')
			{
				dir = cos(W)
			}
			if (ThirdTurn=='L')
			{
				dir = -cos(W);
			}
			if (ThirdTurn =='-')break;
			goal_y += dir*ThirdRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 0;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;
		case 4:
			if (FourthTurn=='R')
			{
				dir = cos(W)
			}
			if (FourthTurn=='L')
			{
				dir = -cos(W);
			}
			if (FourthTurn =='-')break;
			goal_y += dir*FourthRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 13;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;
		case 5:
			if (FifthTurn=='R')
			{
				dir = cos(W)
			}
			if (FifthTurn=='L')
			{
				dir = -cos(W);
			}
			if (FifthTurn =='-')break;
			goal_y += dir*FifthRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 0;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;
		case 6:
			if (SixthTurn=='R')
			{
				dir = cos(W)
			}
			if (SixthTurn=='L')
			{
				dir = -cos(W);
			}
			if (SixthTurn =='-')break;
			goal_y += dir*SixthRows*0.75;
			goal_w += 3.1415;
			while ((__abs__(goal_y-Y)>0.1)&&((__abs__(goal_w-W)>0.2));
			goal_x = 13;
			while (__abs__(goal_x-X)>0.5);
			loops++;
			break;
		default: break;
		}
	}
}

bool FindObstacle()
{
	int numberOccupiedCells = 0;
	for (int i = 30-0.5*horizontalSearch; i <horizontalSearch+30; i++)
		{
			for(int j = 30-0.5*verticalSearch; j<verticalSearch+30; j++)
			{
				if(gridmap[i][j] > 0)
				{
						numberOccupiedCells++;
				}
			}
		}
	if (numberOccupiedCells>ObstaclePixels)
	return true;
}


