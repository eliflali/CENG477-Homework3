#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef CENG477_HOMEWORK3_CALLBACKS_H
#define CENG477_HOMEWORK3_CALLBACKS_H
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void reshape(GLFWwindow* window, int w, int h);
#endif //CENG477_HOMEWORK3_CALLBACKS_H
