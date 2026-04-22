#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stdio.h>

typedef struct {
  float r;
  float g;
  float b;
} NormalizedRGB;

NormalizedRGB HexToNormalizedRGB(unsigned int hex)
{
    NormalizedRGB color;

    color.r = ((hex >> 16) & 0xFF)/255.0f;
    color.g = ((hex >> 8) & 0xFF)/255.0f;
    color.b = (hex & 0xFF)/255.0f;

    return color;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0,0,width,height);
}

void exitKey(GLFWwindow* window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {glfwSetWindowShouldClose(window,true);}
}

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(800,600, "Window", NULL, NULL);
  if(window == NULL)
  {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  glViewport(0,0,800,600);

  glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
  
  NormalizedRGB c = HexToNormalizedRGB(0xd37e5c)
   
  while(!glfwWindowShouldClose(window))
  {
    glClearColor(c.r,c.g,c.b, 1.0f);
    gl(GL_COLOR_BUFFER_BIT);
    exitKey(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;

}
