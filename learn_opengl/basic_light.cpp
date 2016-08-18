//
//  colors.cpp
//  learn_opengl
//
//  Created by xueqin.chen on 16/6/11.
//  Copyright (c) 2016ƒÍ xueqin.chen. All rights reserved.
//

#include "basic_light.h"

using namespace camera_adv;
namespace basic_light {
    
    // Function prototypes
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void do_movement();
    
    // Window dimensions
    const GLuint WIDTH = 800, HEIGHT = 600;
    
    // Camera
    camera_adv::Camera  camera(glm::vec3(0.0f, 0.0f, 5.0f));
    GLfloat lastX = WIDTH / 2.0;
    GLfloat lastY = HEIGHT / 2.0;
    bool    keys[1024];
    
    // Light attributes
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    
    // Deltatime
    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame
    
    
    ////////////////////////////  object ///////////////////////////////////////////
    // ohject
    const GLchar* objectVertexObjectSS = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 normal;\n"
    
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 lightPos;\n"
    
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "out vec3 LightPos;\n"
    
    "void main()\n"
    "{\n"
    "gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    //============= in world space ======================
    //"FragPos = vec3(model * vec4(position, 1.0f));\n"
    //"Normal = mat3(transpose(inverse(model))) * normal;\n"
    
    //============in view space============
    "FragPos = vec3(view * model * vec4(position, 1.0f));\n"
    "Normal = mat3(view * model) * normal;\n"
    "LightPos = vec3(view * vec4(lightPos, 1.0));\n" // Transform world-space light position to view-space light position
    //"Normal = mat3(transpose(inverse(view * model))) * normal;\n"
    "}\0";
    
    const GLchar* objectFragmentSS = "#version 330 core\n"
    "out vec4 color;\n"
    
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "in vec3 LightPos;\n"
    
    "uniform vec3 objectColor;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 viewPos;\n"
    
    "void main()\n"
    "{\n"
    // Ambient
    "float ambientStrength = 0.2f;\n"
    "vec3 ambient = ambientStrength * lightColor;\n"
    
    //Diffuse
    "vec3 norm = normalize(Normal);\n"
    "vec3 lightDir = normalize(LightPos - FragPos);"
    "float diff = max(dot(norm, lightDir), 0.0);\n"
    "vec3 diffuse = diff * lightColor;\n"
    
    //Specular
    "float specularStrength = 0.8f;\n"
    // in world space
    //"vec3 viewDir = normalize(viewPos - FragPos);\n"
    // in view space
    "vec3 viewDir = normalize(-FragPos);\n"
    // lightSource --> fragment
    "vec3 reflectDir = reflect(-lightDir, norm);\n"
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
    "vec3 specular = specularStrength * spec * lightColor;\n"
    
    "vec3 result = (ambient + diffuse + specular) * objectColor;\n"
    "color = vec4(result, 1.0f);\n"
    "}\n\0";
    
    
    ////////////////////////////Gouraud  shading  object ///////////////////////////////////////////
    // ohject world space
    const GLchar* gouraudObjectVertexObjectSS = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 normal;\n"
    
    "out vec3 LightingColor;\n"
    
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    
    "uniform vec3 lightColor;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    
    "void main()\n"
    "{\n"
    "gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    
    // Gouraud Shading
    // ------------------------
    "vec3 FragPos = vec3(model * vec4(position, 1.0f));\n"
    "vec3 Normal = vec3(model * vec4(normal, 1.0f));\n"
    
    // Ambient
    "float ambientStrength = 0.2f;\n"
    "vec3 ambient = ambientStrength * lightColor;\n"
    
    //Diffuse
    "vec3 norm = normalize(Normal);\n"
    "vec3 lightDir = normalize(lightPos - FragPos);"
    "float diff = max(dot(norm, lightDir), 0.0);\n"
    "vec3 diffuse = diff * lightColor;\n"
    
    //Specular
    "float specularStrength = 0.8f;\n"
    // in world space
    "vec3 viewDir = normalize(viewPos-FragPos);\n"
    "vec3 reflectDir = reflect(-lightDir, norm);\n"
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);\n"
    "vec3 specular = specularStrength * spec * lightColor;\n"
    
    "LightingColor = (ambient + diffuse + specular);\n"
    "}\0";
    
    const GLchar* gouraudObjectFragmentSS = "#version 330 core\n"
    "out vec4 color;\n"
    
    "in vec3 LightingColor;\n"
    
    "uniform vec3 objectColor;\n"
    
    "void main()\n"
    "{\n"
    
    "color = vec4(objectColor * LightingColor , 1.0f);\n"
    "}\n\0";
    
    
    
    ////////////////////////////  lump light ///////////////////////////////////////////
    const GLchar* lightVertexObjectSS = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    "}\0";
    
    const GLchar* lightFragmentSS = "#version 330 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = vec4(1.0f);\n"
    "}\n\0";
    
    // The MAIN function, from here we start the application and run the game loop
    void basicLight()
    {
        // Init GLFW
        glfwInit();
        // Set all the required options for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        // Create a GLFWwindow object that we can use for GLFW's functions
        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        
        // Set the required callback functions
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        
        // GLFW Options
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
        glewExperimental = GL_TRUE;
        // Initialize GLEW to setup the OpenGL Function pointers
        glewInit();
        
        // Define the viewport dimensions
        glViewport(0, 0, WIDTH, HEIGHT);
        
        // OpenGL options
        glEnable(GL_DEPTH_TEST);
        
        
        // Build and compile our shader program
        Shader  lampShader(lightVertexObjectSS, lightFragmentSS);
        Shader objectShader(objectVertexObjectSS, objectFragmentSS);
        //Shader objectShader(gouraudObjectVertexObjectSS, gouraudObjectFragmentSS);
        
        // Set up vertex data (and buffer(s)) and attribute pointers
        GLfloat vertices[] = {
            
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
            
        };
        // First, set the container's VAO (and VBO)
        GLuint VBO, containerVAO;
        glGenVertexArrays(1, &containerVAO);
        glGenBuffers(1, &VBO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindVertexArray(containerVAO);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        
        // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
        GLuint lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);
        // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Set the vertex attributes (only position data for the lamp))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        
        // Game loop
        while (!glfwWindowShouldClose(window))
        {
            // Calculate deltatime of current frame
            GLfloat currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
            glfwPollEvents();
            do_movement();
            
            // Clear the colorbuffer
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //////////////////////////////////////////////////////////////////////
            // Use cooresponding shader when setting uniforms/drawing objects
            objectShader.Use();
            GLint objectColorLoc = glGetUniformLocation(objectShader.Program, "objectColor");
            GLint lightColorLoc = glGetUniformLocation(objectShader.Program, "lightColor");
            GLint lightPosLoc = glGetUniformLocation(objectShader.Program, "lightPos");
            GLint viewPosLoc = glGetUniformLocation(objectShader.Program, "viewPos");
            glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
            lightPos.x = sin(glfwGetTime()) * 1.5f;
            lightPos.y = cos(glfwGetTime()/2) * 1.0f;
            glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
            
            // Create camera transformations
            glm::mat4 view;
            view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
            // Get the uniform locations
            GLint modelLoc = glGetUniformLocation(objectShader.Program, "model");
            GLint viewLoc = glGetUniformLocation(objectShader.Program, "view");
            GLint projLoc = glGetUniformLocation(objectShader.Program, "projection");
            // Pass the matrices to the shader
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            // Draw the container (using container's vertex attributes)
            glBindVertexArray(containerVAO);
            
            glm::mat4 model;
            GLfloat angle = glm::radians(glfwGetTime() * 45.0f);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            
            ///////////////////////////////////////////////////////////////
            // Also draw the lamp object, again binding the appropriate shader
            lampShader.Use();
            // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
            modelLoc = glGetUniformLocation(lampShader.Program, "model");
            viewLoc = glGetUniformLocation(lampShader.Program, "view");
            projLoc = glGetUniformLocation(lampShader.Program, "projection");
            // Set matrices
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            model = glm::mat4();
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // Draw the light object (using light's vertex attributes)
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            
            // Swap the screen buffers
            glfwSwapBuffers(window);
        }
        
        // Terminate GLFW, clearing any resources allocated by GLFW.
        glfwTerminate();
        
    }
    
    // Is called whenever a key is pressed/released via GLFW
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                keys[key] = true;
            else if (action == GLFW_RELEASE)
                keys[key] = false;
        }
    }
    
    void do_movement()
    {
        // Camera controls
        if (keys[GLFW_KEY_W])
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S])
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A])
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D])
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
    bool firstMouse = true;
    void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
        
        lastX = xpos;
        lastY = ypos;
        
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(yoffset);
    }
}