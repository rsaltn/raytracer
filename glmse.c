#include"glmse.h"
#include<math.h>

#define PI M_PI

float toRad(float angle)
{
  return angle * PI / 180;
}

mat4 mat4I(float value)
{
  mat4 mat = 
  {
    {
      {value,0.0f,0.0f,0.0f},
      {0.0f,value,0.0f,0.0f},
      {0.0f,0.0f,value,0.0f},
      {0.0f,0.0f,0.0f,value}
    }
  };
  return mat;
}

vec3 Rvec3(float x, float y, float z)
{
  vec3 vec = {x, y, z};
  return vec;
}
vec4 Rvec4(float x, float y, float z, float w)
{
  vec4 vec = {x, y, z, w};
  return vec;
}

mat4 translate(mat4 mat, vec3 vec)
{
  mat.value[3][0] += vec.x;
  mat.value[3][1] += vec.y;
  mat.value[3][2] += vec.z;
  return mat;
}

vec4 mat4_mul_vec4(vec4 vec, mat4 mat)
{
  vec4 result;
  result.x = 
  mat.value[0][0] * vec.x +
  mat.value[0][1] * vec.y +
  mat.value[0][2] * vec.z +
  mat.value[0][3] * vec.w;
  
  result.y = 
  mat.value[1][0] * vec.x +
  mat.value[1][1] * vec.y +
  mat.value[1][2] * vec.z +
  mat.value[1][3] * vec.w;

  result.z = 
  mat.value[2][0] * vec.x +
  mat.value[2][1] * vec.y +
  mat.value[2][2] * vec.z +
  mat.value[2][3] * vec.w;

  result.w = 
  mat.value[3][0] * vec.x +
  mat.value[3][1] * vec.y +
  mat.value[3][2] * vec.z +
  mat.value[3][3] * vec.w;

  return result;
}

mat4 mat4_mul_mat4(mat4 mat1, mat4 mat2)
{
  mat4 result = {0};
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      result.value[i][j] = mat1.value[i][0] * mat2.value[0][j] + mat1.value[i][1] * mat2.value[1][j] + mat1.value[i][2] * mat2.value[2][j] + mat1.value[i][3] * mat2.value[3][j];
    }
  }
  return result;
}

mat4 rotatingMat4(mat4 mat, float angle, vec3 axis)
{
  float cosA = cosf(angle);
  float sinA = sinf(angle);
  float oneMinusCos = 1.0f - cosA;

  float x = axis.x;
  float y = axis.y;
  float z = axis.z;

  mat4 result =
  {
    {
      {cosA + x * x * oneMinusCos,      x * y * oneMinusCos - z * sinA,  x * z * oneMinusCos + y * sinA,  0.0f},
      {y * x * oneMinusCos + z * sinA,  cosA + y * y * oneMinusCos,      y * z * oneMinusCos - x * sinA,  0.0f},
      {z * x * oneMinusCos - y * sinA,  z * y * oneMinusCos + x * sinA,  cosA + z * z * oneMinusCos,      0.0f},
      {0.0f,                            0.0f,                            0.0f,                            1.0f}
    }
  };

  return mat4_mul_mat4(result, mat);
}

mat4 scaleMat4(mat4 mat, vec3 vec)
{
  for(int i = 0; i < 4; i++)
  {
    mat.value[0][0] *= vec.x;
    mat.value[0][1] *= vec.y;
    mat.value[0][2] *= vec.z;
  }
  return mat;
}

