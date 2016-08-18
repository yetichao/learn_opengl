#ifndef LIGHT_MUTIPLE_H
#define LIGHT_MUTIPLE_H


#include <stdio.h>
#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader_path.h"
#include "camera_advance.h"

namespace light_mutiple {
	void mutipleLight();
}

#endif