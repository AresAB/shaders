#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <shader_s.h>

#include <memory>
#include <iostream>

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height);

int main(int argc, char *argv[])
{
    const unsigned int SCR_WIDTH = atoi(argv[1]);
    const unsigned int SCR_HEIGHT = atoi(argv[2]);
    const unsigned int SCR_X_HEAVY = atoi(argv[3]);
    const unsigned int SCR_SHOT_WIDTH = atoi(argv[4]);
    const unsigned int SCR_SHOT_HEIGHT = atoi(argv[5]);
    const char *SCR_SHOT_PATH = argv[7];
    const char *TEXTURE1_PATH = argv[8];
    unsigned int image_color_type;

    std::string arg5(argv[6]);
    std::string rgba = "RGBA";

    if (arg5 == rgba)
    {
        image_color_type = GL_RGBA;
        std::cout << "a" << std::endl;
    }
    else
    {
        image_color_type = GL_RGB;
        std::cout << "b" << std::endl;
    }

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
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("src/shader_vert.vs", "src/shader_frag.fs"); // you can name your shader files however you like
    Shader debugShader("src/shader_vert.vs", "src/original.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // texture coords
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,  // top left 
        -1.0f, -1.0f * (1 - SCR_X_HEAVY), 0.0f,   0.0f, 0.0f, // bottom left
         1.0f * SCR_X_HEAVY,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f * SCR_X_HEAVY, -1.0f * (1 - SCR_X_HEAVY), 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,  // top left 
        -1.0f, -1.0f * (1 - SCR_X_HEAVY), 0.0f,   0.0f, 0.0f, // bottom left
         1.0f * SCR_X_HEAVY,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
         1.0f,  1.0f * (1 - SCR_X_HEAVY), 0.0f,   1.0f, 1.0f, // top right
        -1.0f * SCR_X_HEAVY, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f * SCR_X_HEAVY,  1.0f * (1 - SCR_X_HEAVY), 0.0f,   0.0f, 1.0f,  // top left 
        -1.0f * SCR_X_HEAVY, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
         1.0f,  1.0f * (1 - SCR_X_HEAVY), 0.0f,   1.0f, 1.0f // top right
    };
    unsigned int indices[] = {  
        0, 1, 2, // first triangle
        1, 2, 3,  // second triangle
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
    // texture 1
    // ---------
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
    // not all jpgs are made equally, if it causes a segmentation error just convert to png, it aint worth it
    unsigned char *data = stbi_load(TEXTURE1_PATH, &width, &height, &nrChannels, 0);
    if (data)
    {
        // do GL_RGBA on the second GL_RGB for pngs, except if it doesn't feel like it for some reason
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_color_type, GL_UNSIGNED_BYTE, data);
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

    debugShader.use();
    debugShader.setInt("texture1", 0);

    // screenshot shenanigans, don't ask me how it works, it probably doesn't
    // -------------------------------------------------------------------------------------------
    // Sources:
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
    // https://blog.42yeah.is/opengl/2023/05/27/framebuffer-export.html


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

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        saveScreenshotToFile(SCR_SHOT_PATH, window, renderedTexture, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // call shaders
        ourShader.use();

        glViewport(0, 0, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        // Rendering will now be done to renderedTexture
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        // render the triangle
        // this has to be done twice, once for the renderedTexture, and once for the screen
        // there probably is a better way to do this, but I have been searching guides for 2 weeks straight and just want this to work
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        // Rendering will now be done to screen
        // the "0" is the screen, and IS NOT renderTexture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        debugShader.use();

        // render the triangle
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 10, 6);

        ourShader.use();

        // render the triangle
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 4, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// commented out because it might mess with screensaver logic and I generally want textures to keep their original resolution ratios
// ---------------------------------------------------------------------------------------------
/*void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}*/

void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height) {    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        std::cout << "argsataerra" << std::endl;
        std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(scr_width * scr_height * 4 * sizeof(unsigned int));
        // Or you can just:
        //unsigned char *data = new unsigned char[scr_width * scr_height * 3];
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scr_width, scr_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());

        stbi_flip_vertically_on_write(true);
        //int ret = stbi_write_jpg(filename.c_str(), scr_width, scr_height, 3, data.get(), 100);
        int ret = stbi_write_png(filename.c_str(), scr_width, scr_height, 4, data.get(), scr_width * 4);
        if (ret == 0)
        {
            std::cout << "Failed to capture screenshot" << std::endl;
        }
    }
}