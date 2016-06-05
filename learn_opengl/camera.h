//
//  camera.h
//  learn_opengl
//
//  Created by xueqin.chen on 16/6/9.
//  Copyright (c) 2016年 xueqin.chen. All rights reserved.
//

#ifndef __learn_opengl__camera__
#define __learn_opengl__camera__

#include <stdio.h>
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
#include "shader_str.h"
#include "utill.h"
#include <iostream>

void camera();


#endif /* defined(__learn_opengl__camera__) */
