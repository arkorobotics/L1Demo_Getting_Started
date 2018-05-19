#ifndef ALU_H 
#define ALU_H 

#define M_PI 3.141592653589

extern double rotationMatrix[4][4];
extern double inputMatrix[4][1];
extern double outputMatrix[4][1]; 

float radians(uint16_t angle);
int realtoint(float oldval, float oldmin, float oldmax, float newmin, float newmax);

void alu_rot(double x, double y, double z);
void alu_calc_rot_matrix(double angle, double u, double v, double w);

#endif