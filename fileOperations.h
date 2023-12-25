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
#ifndef CENG477_HOMEWORK3_FILEOPERATIONS_H
#define CENG477_HOMEWORK3_FILEOPERATIONS_H
bool ParseObj(const string& fileName);
bool ReadDataFromFile(
        const string& fileName, ///< [in]  Name of the shader file
        string& data);


#endif //CENG477_HOMEWORK3_FILEOPERATIONS_H
