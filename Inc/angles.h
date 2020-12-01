#ifndef ANGLES_h
#define ANGLES_h
#include <math.h>
#include <stdlib.h>

int xangles(int x, int y, int z){
			   int x_val, y_val, z_val;
			   x_val = (int)x;
			   y_val = (int)y;
			   z_val = (int)z - 1000;
		       int x2, y2, z2;
			   x2 = x * x;
			   y2 = y * y;
			   z2 = z * z;
			   float accel_angle_x;
			   float result;
			   result=sqrt(y2+z2);
			   result=x_val/result;
			   accel_angle_x = -atan(result) * 63;
			   return (int)accel_angle_x;
		}
		int yangles(int x, int y, int z){
			  int x_val, y_val, z_val;
		      x_val = (int)x;
			  y_val = (int)y;
			  z_val = (int)z - 1000;
			  int x2, y2, z2;
			  x2 = x * x;
	          y2 = y * y;
			  z2 = z * z;
			  float accel_angle_y;
			  float result1;
			  result1=sqrt(x2+z2);
			  result1=y_val/result1;
			  accel_angle_y = atan(result1) * 63;
			  return (int)accel_angle_y;
		}
#endif
