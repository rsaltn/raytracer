#ifndef GLMSE_H
#define GLMSE_H

typedef struct 
{
  float x;
  float y;
  float z;
} vec3;

typedef struct 
{
  float x;
  float y;
  float z;
  float w;
} vec4;

typedef struct
{
  float value[4][4]; 
} mat4;

vec3 Rvec3(float x, float y, float z);
float toRad(float angle);
mat4 mat4I(float value);
mat4 translate(mat4 mat, vec3 vec);
vec4 vec4_mul_mat4(vec4 vec, mat4 mat);
mat4 rotatingMat4(mat4 mat, float angle, vec3 axis);
mat4 scaleMat4(mat4 mat, vec3 vec);
#endif
