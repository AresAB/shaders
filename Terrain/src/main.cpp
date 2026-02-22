#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imconfig.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui.cpp>
#include <imgui/imgui_impl_glfw.cpp>
#include <imgui/imgui_impl_opengl3.cpp>
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_tables.cpp>
#include <imgui/imgui_widgets.cpp>
#include <imgui/imgui_demo.cpp>

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.cpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader_s.h>
#include <quad_s.h>
#include <cube_s.h>
#include <table_s.h>

#include <memory>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(std::string filename, GLenum image_type);
void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov, float& z_near, float& z_far);
glm::mat4 makePerspectiveMatrix(float fov, float aspect_ratio, float n, float f);
void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height);

int main(int argc, char *argv[])
{
    // input handling
    // --------------------
    const unsigned int SCR_WIDTH = atoi(argv[1]);
    const unsigned int SCR_HEIGHT = atoi(argv[2]);
    const unsigned int SCR_SHOT_WIDTH = atoi(argv[3]);
    const unsigned int SCR_SHOT_HEIGHT = atoi(argv[4]);
    const char *SCR_SHOT_PATH = argv[5];

    // setup
    // --------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_DEPTH_TEST);

    // texture rendering buffer setup
    // --------------------
    unsigned int frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    unsigned int depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    unsigned int renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

    // post processing shader setup
    // --------------------
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    Shader post_shader("src/post_vert.vs", "src/post_frag.fs");
    post_shader.use();
    post_shader.setInt("scr_tex", 0);

    ScreenQuad screen_quad = ScreenQuad();

    // scene setup (only section (aside from render loop) you should be touching)
    // --------------------
    // texture generation

    // shader generation
    Shader ourShader("src/shader_vert.vs", "src/shader_frag.fs"); // you can name your shader files however you like
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    //ourShader.setInt("texture1", 0);

    // scene object(s) initialization
    Quad quad = Quad();

    // uniforms
    glm::vec3 light_col = glm::vec3(1);
    int size_length = 200;
    
    ourShader.setFloat("material.shininess", 128 * 0.078); // obsidian (http://devernay.free.fr/cours/opengl/materials.html)
    ourShader.setVec3("light.dir", glm::vec3(0, -0.5, 0.866));
    ourShader.setVec3("light.ambient", light_col * 0.1f);
    ourShader.setVec3("light.diffuse", light_col * 0.6f);
    ourShader.setVec3("light.specular", glm::vec3(1.0f));

    ourShader.setInt("size_len", size_length);

    // rendering matricies setup
    // --------------------
    ourShader.use();
    // create fov for perspective matrix calculation
    float perspective_fov = 0.785f; // modify for different zoom levels
    float z_near = 0.1f; // near plane z
    float z_far = 250.f; // far plane z
    glm::mat4 perspective = makePerspectiveMatrix(perspective_fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, z_near, z_far);

    // seperate view matrix components to allow for camera movement and rotation
    glm::mat4 view_dir = glm::mat4(1.0f);
    glm::mat4 view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -3.0f));
    glm::mat4 view = view_dir * view_loc;

    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(.5, 1., .5));
    model = glm::translate(model, glm::vec3(1,-1,-50.9));
    model = glm::rotate(model, -3.14f/2, glm::vec3(1, 0, 0));
    ourShader.setMat4("model", model);

    int scr_width;
    int scr_height;
    // render loop
    // --------------------
    ImVec2 seed_vec = ImVec2(230.f, 230.f);
    ImVec4 primary_color = ImVec4(0.0f, 0.5f, 0.f, 1.f);
    ImVec4 secondary_color = ImVec4(0.7f, 0.4f, 0.2f, 1.f);
    int lattice_num = 4;
    while (!glfwWindowShouldClose(window))
    {
	glfwGetWindowSize(window, &scr_width, &scr_height);
        // input processing
        // --------------------
        processInput(window, view_dir, view_loc, perspective_fov, z_near, z_far);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
	ImGui::Begin("Debug Menu");
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("%f FPS Average", io.Framerate);
	ImGui::Text("Position: %f, %f, %f", -view_loc[3][0], -view_loc[3][1], -view_loc[3][2]);
	ImGui::SliderInt("Number of Lattices", &lattice_num, 2, 10);
	ImGui::SliderFloat2("Terrain Seed", (float*)&seed_vec, 220.0f, 240.f, "%.1f");
	ImGui::ColorEdit4("Primary Color", (float*)&primary_color);
	ImGui::ColorEdit4("Secondary Color", (float*)&secondary_color);
	ImGui::End();

        saveScreenshotToFile(SCR_SHOT_PATH, window, renderedTexture, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // toggle wireframe

        // render to texture
        // --------------------
        glViewport(0, 0, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = view_dir * view_loc;
        perspective = makePerspectiveMatrix(perspective_fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, z_near, z_far);

        ourShader.use();

        ourShader.setMat4("view", view);
        ourShader.setMat4("perspective", perspective); 
        ourShader.setVec3("view_pos", -glm::vec3(view_loc[3]));

	ourShader.setVec2("base_seed", seed_vec.x, seed_vec.y);
	ourShader.setVec3("primary", primary_color.x, primary_color.y, primary_color.z);
	ourShader.setVec3("secondary", secondary_color.x, secondary_color.y, secondary_color.z);
    	ourShader.setInt("lattices", lattice_num);

        glActiveTexture(GL_TEXTURE0);

        quad.render_instanced(size_length * size_length);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // disable wireframe

        // render to screen
        // --------------------
        glViewport(0, 0, scr_width, scr_height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        post_shader.use();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screen_quad.render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // poll for inputs and display screen buffer
        // --------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    screen_quad.deallocate();
    quad.deallocate();

    glfwTerminate();
    return 0;
}

unsigned int loadTexture(std::string filename, GLenum image_type) {
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);
    return tex_id;
}

void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov, float& z_near, float& z_far)
{
    float spd = 0.01f;

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
        view_dir = glm::rotate(view_dir, spd * 0.1f, -glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd * 0.1f, glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd * 0.1f, -glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd * 0.1f, glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd * 0.1f, glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd * 0.1f, -glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    // camera zoom
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        fov += spd * 0.5f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        fov -= spd * 0.5f;
    // reset camera position
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -3.0f));
    // reset camera rotation
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        view_dir = glm::mat4(1.0f);
    // reset camera fov
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        fov = 0.785f;
    // near and far plane distances
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        z_near -= spd;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        z_near += spd;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        z_far -= 10 * spd;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        z_far += 10 * spd;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
