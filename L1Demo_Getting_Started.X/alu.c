#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "alu.h"

double rotationMatrix[4][4];
double inputMatrix[4][1] = { {0.0}, {0.0}, {0.0}, {0.0} };
double outputMatrix[4][1] = { {0.0}, {0.0}, {0.0}, {0.0} };

float radians(uint16_t angle) 
{
	return ((angle*3.14159)/180.0);
}

int realtoint(float oldval, float oldmin, float oldmax, float newmin, float newmax) 
{
	return (int)((((oldval - oldmin) * (newmax - newmin)) / (oldmax - oldmin)) + newmin);
}

void alu_rot(double x, double y, double z)
{
	inputMatrix[0][0] = x;
    inputMatrix[1][0] = y;
    inputMatrix[2][0] = z;
    inputMatrix[3][0] = 1.0; 

    int i;
    int j;
    int k;

    for(i = 0; i < 4; i++ )
    {
        for(j = 0; j < 1; j++)
        {
            outputMatrix[i][j] = 0;
            for(k = 0; k < 4; k++)
            {
                outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
            }
        }
    }
}

void alu_calc_rot_matrix(double angle, double u, double v, double w)
{
    double L = (u * u + v * v + w * w);
    double u2 = u * u;
    double v2 = v * v;
    double w2 = w * w; 

    angle = angle * M_PI / 180.0; //converting to radian value

    double cos_ang = cos(angle);
    double sqrt_ang = sqrt(L);
    double sin_ang = sin(angle);

    rotationMatrix[0][0] = (u2 + (v2 + w2) * cos_ang) / L;
    rotationMatrix[0][1] = (u * v * (1 - cos_ang) - w * sqrt_ang * sin_ang) / L;
    rotationMatrix[0][2] = (u * w * (1 - cos_ang) + v * sqrt_ang * sin_ang) / L;
    rotationMatrix[0][3] = 0.0; 

    rotationMatrix[1][0] = (u * v * (1 - cos_ang) + w * sqrt_ang * sin_ang) / L;
    rotationMatrix[1][1] = (v2 + (u2 + w2) * cos_ang) / L;
    rotationMatrix[1][2] = (v * w * (1 - cos_ang) - u * sqrt_ang * sin_ang) / L;
    rotationMatrix[1][3] = 0.0; 

    rotationMatrix[2][0] = (u * w * (1 - cos_ang) - v * sqrt_ang * sin_ang) / L;
    rotationMatrix[2][1] = (v * w * (1 - cos_ang) + u * sqrt_ang * sin_ang) / L;
    rotationMatrix[2][2] = (w2 + (u2 + v2) * cos_ang) / L;
    rotationMatrix[2][3] = 0.0; 

    rotationMatrix[3][0] = 0.0;
    rotationMatrix[3][1] = 0.0;
    rotationMatrix[3][2] = 0.0;
    rotationMatrix[3][3] = 1.0;
}

