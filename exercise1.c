#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stdio.h>
#include<stdlib.h>
#include<stb_image.h>
#include<math.h>

void processInput(GLFWwindow* window, float* XoffSet, float* YoffSet);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
char *load_shader_source(const char* filepath);


int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const int W_WIDTH = 800, W_HEIGHT = 600;

  GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "2 Triangles", NULL, NULL);

  if(window == NULL)
  {
    printf("Failed to init window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD");
    glfwTerminate();
    return -1;
  }

  glViewport(0,0, W_WIDTH, W_HEIGHT);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  float vertices1[] =
  {
    -0.6f,0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.6f,-0.5f,0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,0.0f,0.0f, 1.0f, 0.0f, 0.0f
  };

  float vertices2[] =
  {
    0.0f,0.0f,0.0f, 1.0f, 0.0f, 0.0f,
    0.6f,0.5f,0.0f, 0.0f, 1.0f, 0.0f,
    0.6f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f
  };
  const char* vertexShaderSource = load_shader_source("/home/saltn/Horizon_NDF/exercise1Shader.vs");

  unsigned int vertexShader;
  
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infolog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
      printf("%s", infolog);
    }


  const char* fragmentShaderSource = load_shader_source("/home/saltn/Horizon_NDF/exercise1Shader.fs");

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
    printf("%s", infolog);
  }

  const char* fragmentYShaderSource = "#version 330 core\n"
      "out vec4 FragColor;\n"
      "uniform vec4 color;\n"
      "void main()\n"
      "{\n"
      "   FragColor = vec4(0.5f + color.x, 0.5f + color.y, 0.5f + color.z, 1.0f);\n"
      "}";

  unsigned int fragmentYShader;
  fragmentYShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentYShader, 1, &fragmentYShaderSource, NULL);
  glCompileShader(fragmentYShader);

  glGetShaderiv(fragmentYShader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(fragmentYShader, 512, NULL, infolog);
    printf("%s", infolog);
  }
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram,GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
    printf("%s", infolog);
  }

  unsigned int shaderYProgram;
  shaderYProgram = glCreateProgram();

  glAttachShader(shaderYProgram, vertexShader);
  glAttachShader(shaderYProgram, fragmentYShader);
  glLinkProgram(shaderYProgram);

  glGetProgramiv(shaderYProgram,GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(shaderYProgram, 512, NULL, infolog);
    printf("%s", infolog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(fragmentYShader);

  unsigned int VAO[2], VBO[2];
  glGenVertexArrays(2, VAO);

  glGenBuffers(2, VBO);

  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1),vertices1, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(VAO[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  
    float XoffSet = 0.0f;
    float YoffSet = 0.0f;

  while(!glfwWindowShouldClose(window))
  {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    processInput(window, &XoffSet, &YoffSet);

  

    float offsets[] = {
        XoffSet,
        XoffSet - 2.0f,
        XoffSet + 2.0f
    };
    glUseProgram(shaderProgram);
    int YUniLoc = glGetUniformLocation(shaderProgram, "YoffSet");
    int XUniLoc = glGetUniformLocation(shaderProgram, "XoffSet");
    for(int i = 0; i < 3; i++)
    {
        glUniform1f(XUniLoc, offsets[i]);
        glUniform1f(YUniLoc, YoffSet);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  glDeleteBuffers(2, VBO);
  glDeleteVertexArrays(2, VAO);
  
  glDeleteProgram(shaderProgram);
  glDeleteProgram(shaderYProgram);
  
  glfwTerminate();
  return 0;
}

char *load_shader_source(const char *filepath)
{
  FILE *file = fopen(filepath, "rb");
  if(!file)
  {
    fprintf(stderr, "Failed to open shader source: %s\n", filepath);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  char *source = malloc(length + 1);

  if(!source)
  {
    fprintf(stderr, "Failed to allocate memory for source");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(source, 1 , length, file);
  source[read_size] = '\0';

  fclose(file);
  return source;
}

void processInput(GLFWwindow* window, float* XoffSet, float* YoffSet)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    *YoffSet += 0.001f;
  }
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    *XoffSet -= 0.001f;
  }
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    *YoffSet -= 0.001f;
  }
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    *XoffSet += 0.001f;
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0,0,width,height);
}


