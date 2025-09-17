#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <shader_s.h>

#include <memory>
#include <iostream>

void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov);
glm::mat4 makePerspectiveMatrix(float fov, float aspect_ratio, float n, float f);
void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height);

int main(int argc, char *argv[])
{
    const unsigned int SCR_WIDTH = atoi(argv[1]);
    const unsigned int SCR_HEIGHT = atoi(argv[2]);
    const unsigned int SCR_SHOT_WIDTH = atoi(argv[3]);
    const unsigned int SCR_SHOT_HEIGHT = atoi(argv[4]);
    const char *SCR_SHOT_PATH = argv[5];

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("src/shader_vert.vs", "src/shader_frag.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // not all jpgs are made equally, if it causes a segmentation error just convert to png, it aint worth debugging
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // do GL_RGBA on the second GL_RGB for pngs, except if it doesn't feel like it for some reason
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader.setInt("texture1", 0);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    unsigned int frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    unsigned int renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // "DrawBuffers" are where the fragment shader return to, this is basically saying return to our renderTexture
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

    // create fov for perspective matrix calculation
    // modify for different zoom levels
    float perspective_fov = 0.785f;

    // seperate view matrix components to allow for camera movement and rotation
    glm::mat4 view_dir = glm::mat4(1.0f);
    glm::mat4 view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 model = glm::mat4(1.0f);
    ourShader.setMat4("model", model);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, view_dir, view_loc, perspective_fov);
        saveScreenshotToFile(SCR_SHOT_PATH, window, renderedTexture, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        ourShader.setMat4("view", view_dir * view_loc);
        ourShader.setMat4("projection", makePerspectiveMatrix(perspective_fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)); 

        // call shaders
        ourShader.use();

        glViewport(0, 0, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        // Rendering will now be done to renderedTexture
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        // this has to be done twice, once for the renderedTexture, and once for the screen
        // there probably is a better way to do this, but I have been searching guides for 2 weeks straight and just want this to work
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        // Rendering will now be done to screen
        // the "0" is the screen, and IS NOT renderTexture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render the triangle
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov)
{
    float spd = 0.0001f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        spd *= 5;
    // camera movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    // camera rotation
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    // camera zoom
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        fov += spd * 0.5f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        fov -= spd * 0.5f;
    // reset camera position
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // reset camera rotation
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        view_dir = glm::mat4(1.0f);
    // reset camera fov
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        fov = 0.785f;
}

// recreation of glm's perspective matrix with (l, r, b, t) inputs
glm::mat4 makePerspectiveMatrix(float n, float f, float l, float r, float b, float t) {
    glm::mat4 perspective = glm::mat4();
    // remember that glm does matrix[c][r] format
    perspective[0] = glm::vec4(2 * n / (r - l), 0.0f, 0.0f, 0.0f);
    perspective[1] = glm::vec4(0.0f, 2 * n / (t - b), 0.0f, 0.0f);
    perspective[2] = glm::vec4((r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1.0f);
    perspective[3] = glm::vec4(0.0f, 0.0f, -(2 * f * n)/(f-n), 0.0f);
    return perspective;
}

// recreation of glm's glm::perspective()
glm::mat4 makePerspectiveMatrix(float fov, float aspect_ratio, float n, float f) {
    float t = tan(fov * 0.5) * n;
    float r = t * aspect_ratio;
    return makePerspectiveMatrix(n, f, -r, r, -t, t);
}

void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height) {    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        std::cout << "argsataerra" << std::endl;
        std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(scr_width * scr_height * 4 * sizeof(unsigned int));
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

        stbi_flip_vertically_on_write(true);
        int ret = stbi_write_png(filename.c_str(), scr_width, scr_height, 4, data.get(), scr_width * 4);
        if (ret == 0)
        {
            std::cout << "Failed to capture screenshot" << std::endl;
        }
    }
}