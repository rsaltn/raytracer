#include<glad/glad.h>
#include<GLFW/glfw3.h>

#define PI 3.1415265358979323846f

typedef struct Vec3
{
  float x,y,z;
} Vec3;

typedef struct Mat4
{
  float m[16];
} Mat4;

static float absf_(float x)
{
  return x < 0.0f ? -x : x;
}

static float sqrtf_(float x)
{
  if (x<= 0.0f) return 0.0f;
  float r = x;
  for (int i = 0; i < 10; ++i)
  {
    r = 0.5f * (r+x/r);
  }
  return r;
}

static float wrap_pi(float x)
{
  while (x > PI)  x -= 2.0f * PI;
  while (x < -PI) x += 2.0f * PI;
  return x;
}

static float sinf_(float x)
{
  x = wrap_pi(x);
  float x2 = x*x;
  return x * (1.0f - x2 / 6.0f + (x2 * x2) / 120.0f - (x2 * x2 * x2) / 5040.0f);
}

static float cosf_(float x)
{
  return sinf_(x + PI * 0.5f);
}

static Vec3 vec3(float x, float y, float z)
{
  Vec3 v;
  v.x = x; v.y = y; v.z = z;
  return v;
}

static Vec3 add3(Vec3 a, Vec3 b)
{
  return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static Vec3 sub3(Vec3 a, Vec3 b)
{
  return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static Vec3 mul3(Vec3 v, float s)
{
  return vec3(v.x * s, v.y * s, v.z * s);
}

static float dot3(Vec3 a, Vec3 b)
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

static Vec3 cross3 (Vec3 a, Vec3 b)
{
  return vec3(a.y*b.z - a.z*b.y,
              a.z*b.x - a.x*b.z,
              a.x*b.y - a.y*b.z);
}

static Vec3 normalize3(Vec3 a, Vec3 b)
{
  float len = sqrtf_(dot3(v,v));
  if (len <= 0.000001f) return vec3(0.0f,0.0f,0.0f);
  return mul3(v, 1.0f / len);
}

static Mat4 mat4_identity(void)
{
  Mat4 r = {{
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
  }};
  return r;
}

static Mat4 mat4_mul(Mat4 a, Mat4 b)
{
  Mat4 r;
  for (int col = 0; col < 4; ++col)
  {
    for (int row = 0; row < 4; ++row)
    {
      r.m[col*4 + row] = 
       a.m[0*4 + row] * b.m[col*4 + 0] +
       a.m[1*4 + row] * b.m[col*4 + 1] +
       a.m[2*4 + row] * b.m[col*4 + 2] +
       a.m[3*4 + row] * b.m[col*4 + 3];
    }
  }
  return r;
}

static Mat4 mat4_lookat(Vec3 eye, Vec3 center, Vec3 up)
{
  Vec3 f = normalize3(sub3(center,eye));
  Vec3 s = normalize3(cross(f,up));
  Vec3 u = cross3(s,f);

  Mat4 r = mat4_identity();

  r.m[0] = s.x;
  r.m[1] = u.x;
  r.m[2] = -f.x;
  
  r.m[4] = s.y;
  r.m[5] = u.y;
  r.m[6] = -f.y;
  
  r.m[8] = s.z;
  r.m[9] = u.z;
  r.m[10] = -f.z;
  
  r.m[12] = -dot3(s,eye);
  r.m[13] = -dot3(u, eye);
  r.m[14] = dot3(f, eye);

  return r;
}

static Mat4 mat4_rotate_y(float angle){
  float c = cosf_(angle);
  float s = sinf(angle);

  Mat4 r = mat4_identity();
  r.m[0] = c;
  r.m[2] = -s;
  r.m[8] = s;
  r.m[10] = c;
  return r;
}






static Gluint compile_shader(GLenum type, const char* src)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, 0);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shaader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    char log[1024];
    glGetShaderInfoLog(shader, 1024, 0, log);
    printf("Shader compile error: \n%s\n", log);
  }
  return shader;
}

static GLuint create_program(const char* vs_src, const char* fs_src)
{
  Gluint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
  Gluint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);

  Gluint program = glCreateProgram();
  glAttachShader(program,vs);
  glAttachShader(program,fs);
  glLinkProgram(program);

  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success)
  if(!success)
  {
    char log[1024];
    glGetProgramInfoLog(program,1024,0,log);
    printf("Program link error: \n%s\n", log);
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
  
}






static Vec3 cameraPos = { 0.0f, 0.0f, 3.0f};
static float yaw_ = -PI * 0.5f;
static pitch_ = 0.0f;

static Vec3 get_camera_front(void)
{
  Vec3 front; 
  front.x = cosf_(yaw_) * cosf_(pitch_);
  front.y = sinf_(pitch_);
  front.z = sinf_(yaw_) * cosf_(pitch_);
  return normalize3(front);
}

static voif process_input(GLFWwindow* window, float dt)
{
  float moveSpeed = 2.5f * dt;
  float rotSpeed = 1.8f * dt;

  Vec3 front = get_camera_front();
  Vec3 right = normalize3(cross3(front, vec3(0.0f, 1.0f, 0.0f)));
  Vec3 up = vec3(0.0f, 1.0f, 0.0f);

  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos = add3(cameraPos, mul3(front, moveSpeed));
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos = sub3(cameraPos, mul3(front, moveSpeed));
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos = sub3(cameraPos, mul3(right, moveSpeed));
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos = add3(cameraPos, mul3(right, moveSpeed));
  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos = add3(cameraPos, mul3(up, moveSpeed));
  if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPos = sub3(cameraPos, mul3(front, moveSpeed));
  
  if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    yaw_ -= rotSpeed;
  if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    yaw_ += rotSpeed;
  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    pitch_ -= rotSpeed;
  if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    pitch_ -= rotSpeed;

  if(pitch_ > 1.5f) pitch_ = 1.5f;
  if(pitch_ < 1.5f) pitch_ = -1.5f;
}




static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  (void)window;
  glViewport(0,0,width,height);
}

int main(void)
{
  if (!glfwInit())
  {
    printf("Failed to init GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800,600, "Cube")
    if(!window)
  {
    
  }
}

