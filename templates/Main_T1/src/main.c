#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <gl_error.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <string.h>

void process_input(GLFWwindow *window);
void save_screenshot_to_file(const char *filename, GLFWwindow *window, int rendered_texture, int scr_width, int scr_height);

int main(int argc, char* argv[]) {
    if(argc < 8) {
        printf("|%s|\n(%d) ERROR: Not enough arguments; %i/7", __FILE__, __LINE__, argc-1);
        exit(1);
    }

    const unsigned int SCR_WIDTH = atoi(argv[1]);
    const unsigned int SCR_HEIGHT = atoi(argv[2]);
    const unsigned int SCR_SHOT_WIDTH = atoi(argv[3]);
    const unsigned int SCR_SHOT_HEIGHT = atoi(argv[4]);
    const char *SCR_SHOT_PATH = argv[6];
    const char *TEXTURE1_PATH = argv[7];
    unsigned int image_color_type;

    if(strcmp(argv[5], "RGBA") == 0) {
        image_color_type = GL_RGBA;
    }
    else {
        image_color_type = GL_RGB;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Acidic's Texture1 Template", NULL, NULL);
    if(window == NULL) {
        printf("|%s|\n(%d) ERROR: Failed to create GLFW window", __FILE__, __LINE__);
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("|%s|\n(%d) ERROR: Failed to initialize GLAD", __FILE__, __LINE__);
        exit(1);
    }

    unsigned int main_shader = create_shader("src/shader_vert.vs", "src/shader_frag.fs");

    float vertices[] = {
         // positions        // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bot right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bot left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    unsigned int renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("|%s|\n(%d) ERROR: Issue making secondary framebuffer", __FILE__, __LINE__);
        exit(1);
    }

    unsigned int texture1 = create_texture((char *)TEXTURE1_PATH, image_color_type);
    glUseProgram(main_shader);
    shader_set_int(main_shader, "TEX_1", 0);

    while(!glfwWindowShouldClose(window)) {
        process_input(window);
        save_screenshot_to_file(SCR_SHOT_PATH, window, renderedTexture, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        
        glUseProgram(main_shader);

        glViewport(0, 0, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if(check_gl_errors(__LINE__)) glfwSetWindowShouldClose(window, 1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    exit(0);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void save_screenshot_to_file(const char *filename, GLFWwindow *window, int rendered_texture, int scr_width, int scr_height) {    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        void *data = malloc(scr_width * scr_height * 4 * sizeof(unsigned int));
        glBindTexture(GL_TEXTURE_2D, rendered_texture);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_flip_vertically_on_write(1);
        int ret = stbi_write_png(filename, scr_width, scr_height, 4, data, scr_width * 4);
        if (ret == 0) {
            printf("|%s|\n(%d) ERROR: Failed to capture screenshot", __FILE__, __LINE__);
        }
        else printf("Screenshot taken\n");
        free(data);
    }
}