//
//  colors.h
//  learn_opengl
//
//  Created by xueqin.chen on 16/6/11.
//  Copyright (c) 2016Äê xueqin.chen. All rights reserved.
//

#ifndef __learn_light_caster__
#define __learn_light_caster__

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
#include "shader_str.h"
#include "camera_advance.h"

namespace light_caster {
	void lightCaster();
}




#endif /* defined(__learn_opengl__colors__) */
