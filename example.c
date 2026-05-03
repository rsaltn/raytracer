#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

void processInput(GLFWwindow *window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowsHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
  GLFWwindow* window = glfwCreateWindow(800,600,"Learning", NULL, NULL);
  if (window == NULL)
  {
    printf("Failde to create GLFW window\n");
    glfwTerminate();
      return -1;
  }

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD \n");
    return -1;
  }

  glViewport(0,0,800,600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  NormalizedRGB c = HexToNormalizedRGB(0x0e1520);

  float vertices[] = 
  {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f
  };

  int indices[] = 
  {
    0,1,2,
    2,3,0
  };

  const char* vertexShaderSource = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
  "}\0";

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  const char* fragmentShaderSource = "#version 330 core\n" 
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  FragColor = vec4(119.0f/255.0f,178.0f/255.0f, 85.0f/255.0f, 1.0f);\n"
  "}\0";

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  
  int successF, successV;
  char infoLogF[512], infoLogV[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successV);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successF);

  if(!successV || !successF)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLogV);
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogF);
    printf("%s\n", infoLogV);
    printf("%s\n", infoLogF);
  }

  unsigned int shaderProgram;

  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int successS;
  char infoLogS[512];

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successS);

  if(!successS)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLogS);
  }

unsigned int VAO, VBO, EBO;

glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//arguments
//1. which vertex attribute you want to configure (location we set in vertexShaderSource)
//2. size of the vertex attribute, which is vec3 (3)
//3. The data type
//4. To normalize data. But our data is already normalized.
//5. steps between to different vertices (stride) in bites
//6. the offset where the data begins
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);


  while(!glfwWindowShouldClose(window))
  {
    glClearColor(c.r,c.g,c.b,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    processInput(window);
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }
glfwTerminate();

  return 0;
}
