#include <stdio.h>  
#include <stdlib.h>  
#include <ctype.h>  
#include <string.h>  
#include <time.h>  
#include <conio.h>
#define row 4
#define col 4

//生成随机数
int RandNum()
{
	srand(time(NULL));
	int num = rand()%5;
	if(num==0 || num==1 || num==2 || num==3)
	{
		return 2;
	}
	else
	{
		return 4;
	}
}

void Initial(int *arr) //初始化二维数组
{
	srand(time(NULL));
	int pos = rand()%(row*col);
	int poss = rand()%(row*col);
	arr[pos] = 2;
	arr[(pos+5)%(row*col)] = RandNum();
}

void Rands(int *arr) //随机位置生成随机数2或4
{
	srand(time(NULL));
	int pos = rand()%(row*col);
	while(arr[pos] != 0)
	{
		pos = (pos+1)%(row*col);
	}
	arr[pos] = RandNum();
}
		
int MoveLeft(int arr[row][col])  // 向左移动
{
	int flag = -1;
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col-1; j++)
		{
			if(arr[i][j] != 0)
			{
				int k = j+1;
				while(k < col)
				{
					if(arr[i][k] !=  0)
					{
					     if(arr[i][j] == arr[i][k])
						{
							flag = 0;
							arr[i][j] += arr[i][k];
							arr[i][k] = 0;
						    
						}
					    break;
					}
					k++;
				}
			}
		}
	}

	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col-1; j++)
		{
			
			if(arr[i][j] == 0)
			{
				int k = j+1;
				while(k < col)
				{
					if(arr[i][k] != 0)
					{
						flag = 0;
						arr[i][j] = arr[i][k];
						arr[i][k] = 0;
						k = col;
					}
					k++;
				}
				}
			}
		}

	if(flag == 0)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

int MoveUp(int arr[row][col])  //向上移动
{
	int flag = -1;
	for(int j = 0; j < row-1; j++)
	{
		for(int i = 0; i < col; i++)
		{
			if(arr[i][j] != 0)
			{
				int k = i+1;
				while(k < row)
				{
					if(arr[k][j] != 0)
					{
					if(arr[i][j] == arr[k][j])
					{
						flag = 0;
						arr[i][j] += arr[k][j];
						arr[k][i] = 0;
						
					}
					break;
				   }
					k++;
			     }
		}
	}
	}

	for(int j = 0; j < col-1; j++)
	{
		for(int i = 0; i < row-1; i++)
		{
			
			if(arr[i][j] == 0)
			{
				int k = i+1;
				   while(k < row)
				  {
					if(arr[i][j] != arr[k][j])
					{
						flag = 0;
						arr[i][j] += arr[k][j];
						arr[k][j] = 0;
						k = row;
					}
					k++;
				  }
		     }
		}
	}

	if(flag == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Contain2048(int arr[row][col])
{
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			if(arr[i][j] == 2048)
			{
				printf("You have a victory!");
				return true;
			}
		}
	}
	return false;
}

int MoveRight(int arr[row][col])  //向右移动
{
	int flag = -1;
	for(int i = 0; i < row; i++)
	{
		for(int j = col-1; j >= 1; j--)
		{
			if(arr[i][j] != 0)
			{
				int k = j-1;
				while(k >= 0)
				{
					if(arr[i][k] != 0)
					{
					     if(arr[i][j] == arr[i][k])
						{
							flag = 0;
							arr[i][j] += arr[i][k];
							arr[i][k] = 0;
						    
						 }
					    break;
					}
					k--;
				}
			}
		}
	}

	for(int i = 0; i < row; i++)
	{
		for(int j = col-1; j >= 1; j--)
		{
			if(arr[i][j] == 0)
			{
			int k = j-1;
				while(k >= 0)
				{
					if(arr[i][j] != arr[i][k])
					{
						flag = 0;
						arr[i][j] = arr[i][k];
						arr[i][k] = 0;
						k = 0;
					}
					k--;
				}
			}
		}
    }

	if(flag == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int MoveDown(int arr[row][col])  //向下移动
{
	int flag = -1;
	    for(int j = 0; j < col; j++)
		{
			for(int i = row-1; i >= 1; i--)
			{
			if(arr[i][j] != 0)
			{
				int k = i-1;
				while(k >= 0)
				{
					if(arr[k][j] != 0)
					{
					if(arr[i][j] == arr[k][j])
					{
						flag = 0;
						arr[i][j] += arr[k][j];
						arr[k][j] = 0;
						
					}
					break;
					}
					k--;
				}
			}
		}
	}

	 for(int j = 0; j < col; j++)
		{
			for(int i = row-1; i >= 1; i--)
			{
			{
				if(arr[i][j] == 0)
				{
					int k = i-1;
				while(k >= 0)
				{
					if(arr[k][j] != 0)
					{
						flag = 0;
						arr[i][j] = arr[k][j];
						arr[k][j] = 0;
						k = 0;
					}
					k--;
				}
			    }
		   }
	       }
       }

	 if(flag == 0)
	 {
		 return 1;
	 }
	 else
	 {
		 return 0;
	 }
}

void display(int arr[row][col]) //打印数组
{
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			printf("%5d",arr[i][j]);
		}
		printf("\n");
	}
}

bool GameOver(int arr[row][col]) //判断游戏结束
{
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			if(arr[i][j] == 0)
			{
				return false;
			}
		}
	}
	return true;
}

void Menu(int arr[row][col]) //主菜单
{
	int ch;
	while(1)
	{
	switch(ch= _getch())
	{
	case  72:{if(MoveUp(arr))
			 {
				  Rands(&arr[0][0]);
			  }
			  system("cls");
			  display(arr);
			  if(Contain2048(arr))
			  {
				  return ;
			  }
			  break;
			 }
	case 80:{if(MoveDown(arr))
			{
				  Rands(&arr[0][0]);
			  }
			  system("cls");
			  display(arr);
			  if(Contain2048(arr))
			  {
				  return ;
			  }
			  break;
			}
	case 97:{if(MoveLeft(arr))
			{
				  Rands(&arr[0][0]);
			  }
			  system("cls");
			  display(arr);
			  if(Contain2048(arr))
			  {
				  return ;
			  }
			  break;
			}
	case 77:{if(MoveRight(arr))
			{
				  Rands(&arr[0][0]);
			  }
			  system("cls");
			  display(arr);
			  if(Contain2048(arr))
			  {
				  return ;
			  }
			  break;
			}
	default:break;
	}
	 if(GameOver(arr))
	{
	   printf("Game Over!!!\n");
	   return;
	}
	}
}

int main()
{
	int arr[row][col] = {0};
	Initial(&arr[0][0]);
	display(arr);
	Menu(arr);
	return 0;
}
