//
//  adv_depth_testing.h
//  learn_opengl
//
//  Created by xueqin.chen on 16/7/16.
//  Copyright (c) 2016年 xueqin.chen. All rights reserved.
//

#ifndef __learn_opengl__adv_depth_testing__
#define __learn_opengl__adv_depth_testing__

#include <stdio.h>
// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader_str.h"
#include "camera_advance.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

namespace adv_depth_testing {
    void depthTesting();
}


#endif /* defined(__learn_opengl__adv_depth_testing__) */
