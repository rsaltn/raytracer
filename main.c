#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define W_WIDTH 1000
#define W_HEIGHT 700
#define MAX_SPHERES 16

typedef struct
{
    float x;
    float y;
    float z;
} Vec3;

typedef struct
{
    Vec3 pos;
    float radius;
    Vec3 color;
    float emission;
} Sphere;

typedef struct
{
    Vec3 pos;
    float yaw;
    float pitch;
} Camera;

char *load_shader_source(const char* filepath);
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Vec3 vec3(float x, float y, float z);
Vec3 add(Vec3 a, Vec3 b);
Vec3 sub(Vec3 a, Vec3 b);
Vec3 mul(Vec3 a, float s);
float dotVec(Vec3 a, Vec3 b);
Vec3 crossVec(Vec3 a, Vec3 b);
Vec3 normalizeVec(Vec3 v);
Vec3 getCameraFront(void);
void addSphere(float x, float y, float z, float radius, float r, float g, float b, float emission);

Sphere spheres[MAX_SPHERES];
int sphereCount = 0;

Camera camera = {
    {0.0f, 0.0f, 7.0f},
    -90.0f,
    0.0f
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastMouseX = W_WIDTH / 2.0f;
float lastMouseY = W_HEIGHT / 2.0f;
bool firstMouse = true;

int screenWidth = W_WIDTH;
int screenHeight = W_HEIGHT;

int main(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "basic raytraced planets", NULL, NULL);

    if (window == NULL)
    {
        printf("failed to make window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("failed to load glad\n");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    unsigned int shaderProgram = createShaderProgram("raytrace.vs", "raytrace.fs");

    float quadVertices[] =
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,

        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*
       object creation
        addSphere(x, y, z, radius, red, green, blue, emission)

        emission 0 = no light object
        emission > 0 = glowing object
    */

    addSphere(0.0f, 0.0f, 0.0f, 1.1f, 1.0f, 0.75f, 0.25f, 5.0f);     // sun
    addSphere(3.0f, 0.0f, 0.0f, 0.45f, 0.2f, 0.45f, 1.0f, 0.0f);
    addSphere(-3.5f, 0.3f, -1.0f, 0.7f, 0.9f, 0.25f, 0.15f, 0.0f);
    addSphere(0.5f, -2.0f, -3.0f, 0.55f, 0.2f, 0.9f, 0.35f, 0.0f);
    addSphere(1.8f, 1.2f, -2.2f, 0.3f, 0.7f, 0.7f, 0.8f, 0.0f);
    addSphere(-1.8f, -1.4f, -2.8f, 0.35f, 0.6f, 0.35f, 1.0f, 0.0f);

    glUseProgram(shaderProgram);

    int resLoc = glGetUniformLocation(shaderProgram, "uResolution");
    int timeLoc = glGetUniformLocation(shaderProgram, "uTime");

    int camPosLoc = glGetUniformLocation(shaderProgram, "uCamPos");
    int camFrontLoc = glGetUniformLocation(shaderProgram, "uCamFront");
    int camRightLoc = glGetUniformLocation(shaderProgram, "uCamRight");
    int camUpLoc = glGetUniformLocation(shaderProgram, "uCamUp");

    int sphereCountLoc = glGetUniformLocation(shaderProgram, "uSphereCount");

    int spherePosLoc[MAX_SPHERES];
    int sphereRadiusLoc[MAX_SPHERES];
    int sphereColorLoc[MAX_SPHERES];
    int sphereEmissionLoc[MAX_SPHERES];

    for (int i = 0; i < MAX_SPHERES; i++)
    {
        char name[128];

        sprintf(name, "uSpherePos[%d]", i);
        spherePosLoc[i] = glGetUniformLocation(shaderProgram, name);

        sprintf(name, "uSphereRadius[%d]", i);
        sphereRadiusLoc[i] = glGetUniformLocation(shaderProgram, name);

        sprintf(name, "uSphereColor[%d]", i);
        sphereColorLoc[i] = glGetUniformLocation(shaderProgram, name);

        sprintf(name, "uSphereEmission[%d]", i);
        sphereEmissionLoc[i] = glGetUniformLocation(shaderProgram, name);
    }

    glUniform1i(sphereCountLoc, sphereCount);

    for (int i = 0; i < sphereCount; i++)
    {
        glUniform1f(sphereRadiusLoc[i], spheres[i].radius);
        glUniform3f(sphereColorLoc[i], spheres[i].color.x, spheres[i].color.y, spheres[i].color.z);
        glUniform1f(sphereEmissionLoc[i], spheres[i].emission);
    }

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        float t = currentFrame;

        spheres[1].pos.x = cosf(t * 0.7f) * 3.0f;
        spheres[1].pos.z = sinf(t * 0.7f) * 3.0f;

        spheres[4].pos.x = spheres[1].pos.x + cosf(t * 2.0f) * 0.8f;
        spheres[4].pos.z = spheres[1].pos.z + sinf(t * 2.0f) * 0.8f;
        spheres[4].pos.y = sinf(t * 2.0f) * 0.2f;

        Vec3 front = getCameraFront();
        Vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
        Vec3 right = normalizeVec(crossVec(front, worldUp));
        Vec3 up = normalizeVec(crossVec(right, front));

        glUseProgram(shaderProgram);

        glUniform2f(resLoc, (float)screenWidth, (float)screenHeight);
        glUniform1f(timeLoc, currentFrame);

        glUniform3f(camPosLoc, camera.pos.x, camera.pos.y, camera.pos.z);
        glUniform3f(camFrontLoc, front.x, front.y, front.z);
        glUniform3f(camRightLoc, right.x, right.y, right.z);
        glUniform3f(camUpLoc, up.x, up.y, up.z);

        for (int i = 0; i < sphereCount; i++)
        {
            glUniform3f(spherePosLoc[i], spheres[i].pos.x, spheres[i].pos.y, spheres[i].pos.z);
        }

        glClearColor(0.0f, 0.0f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}

char *load_shader_source(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");

    if (!file)
    {
        fprintf(stderr, "Failed to open shader source: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *source = malloc(length + 1);

    if (!source)
    {
        fprintf(stderr, "Failed to allocate memory for source\n");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(source, 1, length, file);
    source[read_size] = '\0';

    fclose(file);

    return source;
}

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    char* vertexShaderSource = load_shader_source(vertexPath);
    char* fragmentShaderSource = load_shader_source(fragmentPath);

    if (!vertexShaderSource || !fragmentShaderSource)
    {
        printf("failed to load shader files\n");
        return 0;
    }

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infolog[1024];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infolog);
        printf("vertex shader error:\n%s\n", infolog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infolog);
        printf("fragment shader error:\n%s\n", infolog);
    }

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infolog);
        printf("program link error:\n%s\n", infolog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(vertexShaderSource);
    free(fragmentShaderSource);

    return shaderProgram;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    float speed = 4.0f * deltaTime;

    Vec3 front = getCameraFront();
    Vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
    Vec3 right = normalizeVec(crossVec(front, worldUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos = add(camera.pos, mul(front, speed));

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos = sub(camera.pos, mul(front, speed));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos = sub(camera.pos, mul(right, speed));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos = add(camera.pos, mul(right, speed));

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.pos.y += speed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.pos.y -= speed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    screenWidth = width;
    screenHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastMouseX;
    float yoffset = lastMouseY - (float)ypos;

    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    float sensitivity = 0.08f;

    camera.yaw += xoffset * sensitivity;
    camera.pitch += yoffset * sensitivity;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;

    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;
}

Vec3 vec3(float x, float y, float z)
{
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec3 add(Vec3 a, Vec3 b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 sub(Vec3 a, Vec3 b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 mul(Vec3 a, float s)
{
    return vec3(a.x * s, a.y * s, a.z * s);
}

float dotVec(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 crossVec(Vec3 a, Vec3 b)
{
    return vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Vec3 normalizeVec(Vec3 v)
{
    float len = sqrtf(dotVec(v, v));

    if (len == 0.0f)
        return vec3(0.0f, 0.0f, 0.0f);

    return mul(v, 1.0f / len);
}

Vec3 getCameraFront(void)
{
    float yawRad = camera.yaw * 3.14159265f / 180.0f;
    float pitchRad = camera.pitch * 3.14159265f / 180.0f;

    Vec3 front;

    front.x = cosf(yawRad) * cosf(pitchRad);
    front.y = sinf(pitchRad);
    front.z = sinf(yawRad) * cosf(pitchRad);

    return normalizeVec(front);
}

void addSphere(float x, float y, float z, float radius, float r, float g, float b, float emission)
{
    if (sphereCount >= MAX_SPHERES)
    {
        printf("too many spheres\n");
        return;
    }

    spheres[sphereCount].pos = vec3(x, y, z);
    spheres[sphereCount].radius = radius;
    spheres[sphereCount].color = vec3(r, g, b);
    spheres[sphereCount].emission = emission;

    sphereCount++;
}
