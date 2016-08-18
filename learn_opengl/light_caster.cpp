#include "light_caster.h"

using namespace camera_adv;

/*
 1 out of outerCutoff, show ambient light
 2. in inner, it's intensity is 1.0
 3, between inner and outer, it's intensity is [0.0, 1.0]
 */
namespace light_caster{
    ////////////////////////////  object ///////////////////////////////////////////
    // ohject
    const GLchar* objectVertexObjectSS = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 normal;\n"
    "layout (location = 2) in vec2 texCoords;\n"
    
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "out vec2 TexCoords;\n"
    
    "void main()\n"
    "{\n"
    "gl_Position = projection * view * model * vec4(position, 1.0f);\n"
    "TexCoords = texCoords;\n"
    
    //============in world space============
    "FragPos = vec3(model * vec4(position, 1.0f));\n"
    "Normal = mat3(transpose(inverse(model))) * normal;\n"
    "}\0";
    
    const GLchar* objectFragmentSS = "#version 330 core\n"
    "struct Material\n"
    "{\n"
    "sampler2D diffuse;\n"
    "sampler2D specular;\n"
    "sampler2D emission;\n"
    "float shininess;\n"
    "};\n"
    
    "struct Light\n"
    "{\n"
    //"vec3 position;\n"
    //"vec3 direction;\n"
    
    "vec3 ambient;\n"
    "vec3 diffuse;\n"
    "vec3 specular;\n"
    
    // point light
    "float constant;\n"
    "float linear;\n"
    "float quadratic;\n"
    // flash light
    "vec3 position;\n"
    "vec3 direction;\n"
    "float cutOff;\n"
    "float outerCutOff;\n"
    "};\n"
    
    
    "out vec4 color;\n"
    
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "in vec2 TexCoords;\n"
    
    "uniform vec3 viewPos;\n"
    "uniform Material material;\n"
    "uniform Light light;\n"
    
    
    "void main()\n"
    "{\n"
    
    "vec3 lightDir = normalize(light.position - FragPos);\n"
    //"vec3 lightDir = normalize(-light.direction);\n" // for directional light
    "float theta = dot(lightDir, normalize(-light.direction));\n"
    // Ambient
    "vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
    
    
    //Diffuse
    "vec3 norm = normalize(Normal);\n"
    "float diff = max(dot(norm, lightDir), 0.0);\n"
    "vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
    
    //Specular
    "float specularStrength = 0.8f;\n"
    // in world space
    "vec3 viewDir = normalize(viewPos - FragPos);\n"
    "vec3 reflectDir = reflect(-lightDir, norm);\n"
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
    "vec3 specular = light.specular * (spec *  vec3(texture(material.specular, TexCoords)));\n"
    //"vec3 specular = light.specular * (spec * (vec3(1.0f) - vec3(texture(material.specular, TexCoords))));\n"
    
    // point light distance
    "float dist = length(light.position - FragPos);\n"
    "float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));\n"
    "float epsilon = light.cutOff - light.outerCutOff;\n"
    "float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n"
    
    "color = vec4(ambient + (diffuse + specular) * attenuation * intensity,1.0f);\n"
    
    
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
    
    
    // Function prototypes
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void do_movement();
    
    // Window dimensions
    const GLuint WIDTH = 800, HEIGHT = 600;
    
    // Camera
    Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
    GLfloat lastX = WIDTH / 2.0;
    GLfloat lastY = HEIGHT / 2.0;
    bool    keys[1024];
    
    // Light attributes
    //glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightPos(1.5f, 0.2f, 2.0f);
    
    // Deltatime
    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame
    
    // The MAIN function, from here we start the application and run the game loop
    void lightCaster()
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
        Shader lightingShader(objectVertexObjectSS, objectFragmentSS);
        Shader  lampShader(lightVertexObjectSS, lightFragmentSS);
        
        // Set up vertex data (and buffer(s)) and attribute pointers
        GLfloat vertices[] = {
            // Positions           // Normals           // Texture Coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };
        
        // Positions all containers
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
        };
        // First, set the container's VAO (and VBO)
        GLuint VBO, containerVAO;
        glGenVertexArrays(1, &containerVAO);
        glGenBuffers(1, &VBO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindVertexArray(containerVAO);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Teture attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        
        // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
        GLuint lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);
        // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Set the vertex attributes (only position data for the lamp))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        
        // Load textures 1
        GLuint diffuseMap;
        glGenTextures(1, &diffuseMap);
        int width, height;
        unsigned char* image;
        // Diffuse map
        image = SOIL_load_image("/Users/cc_xueqin/programming/learning/opengl/project/learn_opengl/res/container2.png", &width, &height, 0, SOIL_LOAD_RGB);
        if (image == NULL){
            std::cout << "ERROR::image is null\n" << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        
        // Load textures 2
        GLuint specularMap;
        glGenTextures(1, &specularMap);
        // Specular map  //lighting_maps_specular_color.png   //container2_specular
        image = SOIL_load_image("/Users/cc_xueqin/programming/learning/opengl/project/learn_opengl/res/container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
        if (image == NULL){
            std::cout << "ERROR::image is null\n" << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Set texture units
        lightingShader.Use();
        glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);
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
            
            //-------------------- set materials --------------------------------------
            // Use cooresponding shader when setting uniforms/drawing objects
            lightingShader.Use();
            // position
            //GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
            //glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
            
            // directional light
            //GLint lightDirPos = glGetUniformLocation(lightingShader.Program, "light.direction");
            //glUniform3f(lightDirPos, -0.0f, -0.0f, -0.01);
            
            GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
            glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
            
            // Set lights properties
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 0.9f, 0.9f, 0.9f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
            // Set point light distance = 50
            glUniform1f(glGetUniformLocation(lightingShader.Program, "light.constant"), 1.0f);
            glUniform1f(glGetUniformLocation(lightingShader.Program, "light.linear"), 0.09);
            glUniform1f(glGetUniformLocation(lightingShader.Program, "light.quadratic"), 0.032);
            // set flash light
            GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
            GLint lightSpotdirLoc = glGetUniformLocation(lightingShader.Program, "light.direction");
            GLint lightSpotCutOffLoc = glGetUniformLocation(lightingShader.Program, "light.cutOff");
            GLint lightSpotOuterCutOffLoc = glGetUniformLocation(lightingShader.Program, "light.outerCutOff");
            glUniform3f(lightPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
            glUniform3f(lightSpotdirLoc, camera.Front.x, camera.Front.y, camera.Front.z);
            glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(12.5f)));
            glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(17.5f)));
            // Set material properties
            glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
            
            //---------------- light ------------------------------------------------
            // Create camera transformations
            glm::mat4 view;
            view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
            // Get the uniform locations
            GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
            GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
            GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
            // Pass the matrices to the shader
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            // Bind diffuse map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            // Bind specular map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);
            
            // Draw the container (using container's vertex attributes)
            glBindVertexArray(containerVAO);
            
            glm::mat4 model;
            glBindVertexArray(containerVAO);
            for (GLuint i = 0; i < 10; i++)
            {
                model = glm::mat4();
                model = glm::translate(model, cubePositions[i]);
                GLfloat angle = glm::radians(45.0f * (i));//glfwGetTime() *
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            glBindVertexArray(0);
            
            //---------------- lamp------------------------------------------------
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