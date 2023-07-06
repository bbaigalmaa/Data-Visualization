/* Compile
 g++ main.cpp -std=c++17 include/*.cpp include/backends/imgui_impl_opengl3.cpp include/backends/imgui_impl_sdl2.cpp ./thirdParty/glad/src/glad.c -I./thirdParty/glad/include -I./thirdParty/glm/glm-master -I./thirdparty/imgui/imgui-master -I./include -o prog -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2
 
 
 
 
 g++ main.cpp -std=c++17 ./thirdParty/glad/src/glad.c -I./thirdParty/glad/include -I./thirdParty/glm/glm-master -I./thirdparty/imgui/imgui-master -I./include -o prog -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2

 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


//Globals
int gScreenHeight = 640;
int gScreenWidth = 680;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

bool gQuit = false; //if true we are quit
bool show_demo_window = false; //imgui window
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


//Imgui switching variables
bool color1 = false;
bool color2 = false;
bool texture1Switch = false;
bool texture2Switch = false;

bool color1rect = false;
bool color2rect = false;

char texture1Path[128] = "/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/assets/mossy.png";

bool texLeftTop = false;
bool texLeftBottom = false;
bool texRightBottom = false;
bool texRightTop = false;

//ImGuiIO& io = ImGui::GetIO();



//VAO
GLuint gVertexArrayObject = 0;
//VBO
GLuint gVertexBufferObject = 0;

//VAO - cube
GLuint gVertexArrayCubeObject = 0;
//VBO - cube
GLuint gVertexBufferCubeObject = 0;

//IBO
//used to store the aaray of indices that we want to draw from, whne we do tindexed drawing
GLuint gIndexBufferObject = 0;
//Program Object (for our shader)
GLuint gGraphicsPipelineShaderProgram = 0;
GLuint gNormalShaderProgram = 0;
GLuint gRectangleShaderProgram = 0;
//Texture object
GLuint gTextureObject  = 0;

float g_uOffset = -2.0f;
float g_uRotate = 0.0f;
float g_uScale = 0.5f;
GLfloat normalLength = 1.0f;
GLfloat rectSize = 0.3f;
//Camera
Camera      m_camera;
// variables for calculating real time fps - modified in Update()
double      m_delta_time;



//-------------------- Error handling -------------------------------
static void GLClearAllErrors() {
    while(glGetError() != GL_NO_ERROR){
        
    }
}


//Returns true if we have an error
static bool GLCheckErrorStatus(const char* function, int line){
    
    while(GLenum error = glGetError()){
        std::cout << "OpenGL error: " << error
                  << "\tLine: " << __LINE__
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

//--------------------- Error Handling End -----------------------

std::vector<GLfloat> readFile(const std::string& filename) {
    std::string str = "";
    std::string word = "";
    std::vector<GLfloat> result;
    
    std::ifstream myFile(filename);
    std::fstream line;

    if(myFile.is_open()){
        while(std::getline(myFile, str)){
            
            std::stringstream line(str);
            while (std::getline(line, word, ' ')) {
                result.emplace_back((GLfloat)std::stof(word));
               
            }
        }
        myFile.close();
    } else {
        std::cout << "could not open the file to read data." << std::endl;
    }
    
    return result;
    
}

SDL_Surface* LoadTextureImage(const std::string& filename) {
        
    // Load sample.png into image
    SDL_Surface* image = IMG_Load(filename.c_str());
    
    if (image == nullptr) {
        std::cout << "IMG_Load: " << IMG_GetError() << "\n";
    }
    return image;
        
}

std::string LoadShaderAsString(const std::string& filename) {
        
    std::string result = "";
        
    std::string line = "";
    
    std::ifstream myFile(filename);
    
    
    std::cout << filename << std::endl;
    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + "\n";
        }
        myFile.close();
    } else {
        std::cout << "could not open the file." << std::endl;
    }
    
    return result;
        
}

GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject;

    if(type == GL_VERTEX_SHADER){
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    } else if(type == GL_FRAGMENT_SHADER){
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    } else if(type == GL_GEOMETRY_SHADER){
        shaderObject = glCreateShader(GL_GEOMETRY_SHADER);
    }
    
    const char* src = source.c_str(); //c string version of c++ string
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    return shaderObject;

}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource, const std::string& geometryshadersource){
//GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource){
    GLuint programObject = glCreateProgram();
    
    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);
    GLuint myGeometryShader = CompileShader(GL_GEOMETRY_SHADER, geometryshadersource);
   

    
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myGeometryShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    //Validate our program
    glValidateProgram(programObject);
    
    GLint testProgramStatus;
    glGetProgramiv(programObject, GL_LINK_STATUS, &testProgramStatus);
    
    //std::cout << "Link status: " << testProgramStatus << std::endl;
    if (testProgramStatus != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(programObject, 1024, &log_length, message);
        // Write the error to a log
        std::cout << "Link status: " << message << std::endl;
    } else {
        std::cout << "Link status: linked" << std::endl;
    }
    
    GLint testActiveUniforms, testActiveAttrib;
    glGetProgramiv(programObject, GL_ACTIVE_ATTRIBUTES, &testActiveAttrib);
    glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &testActiveUniforms);
    std::cout << "Active attributes: " << testActiveAttrib << std::endl;
    std::cout << "Active uniforms: " << testActiveUniforms << std::endl;
    

    return programObject;
}

void CreateGraphicsPipeline(){
    
    std::string vertexShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/frag.glsl");
    std::string geometryShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/geometry.glsl");
    
    
    std::string vertexLineShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/vertLine.glsl");
    std::string fragmentLineShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/fragLine.glsl");
    std::string geometryLineShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/geometryLine.glsl");
    
    std::string vertexRectShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/vertRect.glsl");
    std::string fragmentRectShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/fragRect.glsl");
    std::string geometryRectShaderSource = LoadShaderAsString("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/shader/geometryRect.glsl");
    
    
    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource, geometryShaderSource);
    gNormalShaderProgram = CreateShaderProgram(vertexLineShaderSource, fragmentLineShaderSource, geometryLineShaderSource);
    gRectangleShaderProgram = CreateShaderProgram(vertexRectShaderSource, fragmentRectShaderSource, geometryRectShaderSource);
    
    
}

//Configuring OpenGL
void GetOpenGLVersionInfo(){
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
     std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
     std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
     std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void VertexSpecification(){

    //Libes on the CPU
  //  const std::vector<GLfloat> vertexData = readFile("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/OpenGLTest copy 2/elteLogo.xyz");
    
    const std::vector<GLfloat> vertexData = readFile("/Users/baigalmaa/Study/FirstYearOfMsc/Semester2/Computer Graphics/Main assignment/DataVisualization/sample.txt");
    
  /*  const std::vector<GLfloat> vertexDataCube{
        //Face quad
        //2th vertex - top left vertex position
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //0th vertex - bottom left vertex position
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        //1th vertex - bottom right vertex position
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        //3th vertex - top right vertex position
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //2th vertex - top left vertex position
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //1th vertex - bottom right vertex position
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        
        //Right quad
        //3th vertex - top right vertex position
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //1th vertex - bottom right vertex position
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        //5th vertex - bottom right vertex position
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
        //7th vertex - top right vertex position
        0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //3th vertex - top right vertex position
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //5th vertex - bottom right vertex position
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        
        //Back quad
        //7th vertex - top right vertex position
        0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //5th vertex - bottom right vertex position
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
        //4th vertex - bottom left vertex position
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
        //6th vertex - top left vertex position
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //7th vertex - top right vertex position
        0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //4th vertex - bottom left vertex position
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
        
        //Left quad
        //6th vertex - top left vertex position
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //4th vertex - bottom left vertex position
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
        //0th vertex - bottom left vertex position
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        //2th vertex - top left vertex position
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //6th vertex - top left vertex position
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //0th vertex - bottom left vertex position
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        
        //Top quad
        //6th vertex - top left vertex position
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //2th vertex - top left vertex position
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //3th vertex - top right vertex position
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        //7th vertex - top right vertex position
        0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //6th vertex - top left vertex position
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        //3th vertex - top right vertex position
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        
        //Bottom quad
        //4th vertex - bottom left vertex position
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
        //0th vertex - bottom left vertex position
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        //1th vertex - bottom right vertex position
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        //5th vertex - bottom right vertex position
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
        //4th vertex - bottom left vertex position
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
        //1th vertex - bottom right vertex position
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f
        
    };
    */
    
    
    //Start setting things up on GPU
    glGenVertexArrays(1, &gVertexArrayObject); //1 - how many, &gVertexArrayObject
    glBindVertexArray(gVertexArrayObject);

    //Start generating out VBO for position
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GL_FLOAT), vertexData.data(), GL_STATIC_DRAW);
    
    //VertexArrayAttrib - Points
    {
        //Linking the attributes in VAO - POSITION
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              3, //number of components in this case x,y,z = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //13*2
                              (void*)0 );
        
        //Linking the attributes in VAO - COLOR1
        glEnableVertexAttribArray(1); //for index 1 of VAO
        glVertexAttribPointer(1,
                              3, //number of components in this case r g b = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //how many bytes I have to hop to get next r g b information - 12 bytes = 4 bytes * 3 floats.
                              (GLvoid*)(sizeof(GL_FLOAT)*3 )); //
        
        //Linking the attributes in VAO - COLOR2
        glEnableVertexAttribArray(2); //for index 2 of VAO
        glVertexAttribPointer(2,
                              3, //number of components in this case r g b = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //how many bytes I have to hop to get next r g b information - 12 bytes = 4 bytes * 3 floats.
                              (GLvoid*)(sizeof(GL_FLOAT)*6 )); //
        
        //Linking the attributes in VAO - TEX1
        glEnableVertexAttribArray(3); //for index 3 of VAO
        glVertexAttribPointer(3,
                              2, //number of components in this case r g b = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //how many bytes I have to hop to get next r g b information - 12 bytes = 4 bytes * 3 floats.
                              (GLvoid*)(sizeof(GL_FLOAT)*8 )); //
        
        //Linking the attributes in VAO - TEX2
        glEnableVertexAttribArray(4); //for index 4 of VAO
        glVertexAttribPointer(4,
                              2, //number of components in this case r g b = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //how many bytes I have to hop to get next r g b information - 12 bytes = 4 bytes * 3 floats.
                              (GLvoid*)(sizeof(GL_FLOAT)*10 )); //
        
        //Linking the attributes in VAO - NORMAL
        glEnableVertexAttribArray(5); //for index 4 of VAO
        glVertexAttribPointer(5,
                              3, //number of components in this case r g b = 3
                              GL_FLOAT,
                              GL_FALSE, //Is the data normalized
                              sizeof(GL_FLOAT)*16, //how many bytes I have to hop to get next r g b information - 12 bytes = 4 bytes * 3 floats.
                              (GLvoid*)(sizeof(GL_FLOAT)*13 )); //
        
        //p1 p2 p3 c1 c2 c3 -
        //p1 p2 p3 c11 c12 c13 c21 c22 c23 t11 t12 t21 t22 n1 n2 n3
        
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
    }
    
    glGenTextures(1, &gTextureObject); //1 - how many, &gVertexArrayObject
    glBindTexture(GL_TEXTURE_2D, gTextureObject);
    
//    const std::string& filename = texture1Path;
//    SDL_Surface* img = LoadTextureImage(filename);
//
//    glTexImage2D(GL_TEXTURE_2D
//                 , 0 //mipmap level
//                 , GL_RGB
//                 , img->w
//                 , img->h
//                 , 0
//                 , GL_RGB
//                 , GL_UNSIGNED_BYTE
//                 , img->pixels);
    
    
    //setting up filters
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    glTexParameteri(
//        GL_TEXTURE_2D,
//        GL_TEXTURE_MIN_FILTER,
//        GL_LINEAR_MIPMAP_LINEAR
//    );
//
//    glTexParameteri(
//        GL_TEXTURE_2D,
//        GL_TEXTURE_MAG_FILTER,
//        GL_LINEAR
//    );
    
    //texture tiling - st are the normalized texture coordinates - normalized (u,v)
//    glTexParameteri(
//                        GL_TEXTURE_2D,
//                        GL_TEXTURE_WRAP_S,//wrap in s direction
//                        GL_REPEAT
//                        );
//
//    glTexParameteri(
//                        GL_TEXTURE_2D,
//                        GL_TEXTURE_WRAP_T,//wrap in t direction
//                        GL_CLAMP_TO_BORDER
//                        );
    
//    //send texture to shader
//    glActiveTexture(GL_TEXTURE0); //set active texture unit we would like to use - GL_TEXTURE0 is the default value cannot be specified
//    glBindTexture(GL_TEXTURE_2D, gTextureObject); //binding GL_TEXTURE_2D texture to texture unit 0 - GL_TEXTURE0 // texture unit 0 (GL_TEXTURE0) will use gTextureObject this texture
    
    
     

}

void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL2 could not initialize video subsystem." << std::endl;
        exit(1);
    }

    
    const char* glsl_version = "#version 410";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
        

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //turning on double buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // setting up depth size 24
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    

    //SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
   // gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", 0,0,gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);
    
    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", 0,0,gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);

    if(gGraphicsApplicationWindow == nullptr){
        std::cout << "SDL window was not able to be created" << std::endl;
        exit(1);
    }
        
    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
    
    SDL_GL_MakeCurrent(gGraphicsApplicationWindow, gOpenGLContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if(gOpenGLContext == nullptr){
        std::cout << "OpenGL context not found." << std::endl;
        exit(1);
    }

    //Initialize Glad library

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cout << "Glad was not initialized" << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo();
    
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(gGraphicsApplicationWindow, gOpenGLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    
    //camera set up
    m_camera = Camera(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_camera.setProj(glm::radians(60.0f),  (float)gScreenWidth/(float)gScreenHeight, 0.1f, 1000.0f);
    
}

void Input(){
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0){
        ImGui_ImplSDL2_ProcessEvent(&e);
        bool is_mouse_captured = ImGui::GetIO().WantCaptureMouse;
        bool is_keyboard_captured = ImGui::GetIO().WantCaptureKeyboard;
        if (e.type == SDL_QUIT)
            gQuit = true;
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(gGraphicsApplicationWindow))
            gQuit = true;
        
        if(e.type == SDL_KEYUP){
            
            if(!is_keyboard_captured){
                std::cout << "Up key pressed" << std::endl;
                m_camera.KeyboardUp(e.key);
            }
               
        }
        if(e.type == SDL_KEYDOWN){
            if(!is_keyboard_captured){
                std::cout << "Down key pressed" << std::endl;
                m_camera.KeyboardDown(e.key);
            }
                
        }
        
        if(e.type == SDL_MOUSEMOTION){
            if(!is_mouse_captured){
                std::cout << "Mouse motion" << std::endl;
                m_camera.MouseMove(e.motion);
            }
               
        }
        
        if(e.type == SDL_QUIT){
            std::cout << "Goodbye" << std::endl;
            gQuit = true;
        }
    }
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    
    {

        ImGui::Begin("Helper");                          // Create a window called "Hello, world!" and append into it.

      //  ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)//
     //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      // ImGui::Checkbox("Another Window", &show_another_window);
        
        
       // ImGui::InputText( "Text", &texture1Path.c_str());
        

        ImGui::SliderFloat("Length of normal", &normalLength, 0.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Size of rectangle", &rectSize, 0.1f, 1.0f);
       // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        
        if (ImGui::Button("Color point with color1")){
            color1 = true;
            color2 = false;
            texture1Switch = false;
            texture2Switch = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Color point with color2")){
            color1 = false;
            color2 = true;
            texture1Switch = false;
            texture2Switch = false;
        }
        
        
        if (ImGui::Button("Color point with texture1")){
            color1 = false;
            color2 = false;
            texture1Switch = true;
            texture2Switch = false;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Color point with texture2")){
            color1 = false;
            color2 = false;
            texture1Switch = false;
            texture2Switch = true;
        }
                
        if (ImGui::Button("Color rectangle with color1")){
            color1rect = true;
            color2rect = false;
            texLeftTop = false;
            texLeftBottom = false;
            texRightTop = false;
            texRightBottom = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Color rectangle with color2")){
            color1rect = false;
            color2rect = true;
            texLeftTop = false;
            texLeftBottom = false;
            texRightTop = false;
            texRightBottom = false;
        }
        
        
        //texture coords from rectangle corners
        if (ImGui::Button("Left top corner texture")){
            color1rect = false;
            color2rect = false;
            texLeftTop = true;
            texLeftBottom = false;
            texRightTop = false;
            texRightBottom = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Left bottom corner texture")){
            color1rect = false;
            color2rect = false;
            texLeftTop = false;
            texLeftBottom = true;
            texRightTop = false;
            texRightBottom = false;
        }
        
        if (ImGui::Button("Right top corner texture")){
            color1rect = false;
            color2rect = false;
            texLeftTop = false;
            texLeftBottom = false;
            texRightTop = true;
            texRightBottom = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Right bottom corner texture")){
            color1rect = false;
            color2rect = false;
            texLeftTop = false;
            texLeftBottom = false;
            texRightTop = false;
            texRightBottom = true;
        }
        
        
        ImGui::InputText("Texture path", texture1Path, IM_ARRAYSIZE(texture1Path));
        

        
//        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
//            counter++;
//        ImGui::SameLine();
//        ImGui::Text("counter = %d", counter);

        ImGui::End();
    }
    
    // 3. Show another simple window.
//    if (show_another_window)
//    {
//        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//        ImGui::Text("Hello from another window!");
//        if (ImGui::Button("Close Me"))
//            show_another_window = false;
//        ImGui::End();
//    }
    
    ImGui::Render();
    
    
    
    
    //Retrieve keyboard states
    /*
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if(state[SDL_SCANCODE_UP]){
        
        //m_camera.KeyboardUp(e.key);
        g_uOffset += 0.001f;
        std::cout << "g_uOffset: " << g_uOffset << std::endl;
    }
    if(state[SDL_SCANCODE_DOWN]){
        //m_camera.KeyboardDown(e.key);
        g_uOffset -= 0.001f;
        std::cout << "g_uOffset: " << g_uOffset << std::endl;
    }
    
    if(state[SDL_SCANCODE_LEFT]){
        g_uRotate += 0.1f;
        std::cout << "g_uRotate: " << g_uRotate << std::endl;
    }
    if(state[SDL_SCANCODE_RIGHT]){
        g_uRotate -= 0.1f;
        std::cout << "g_uRotate: " << g_uRotate << std::endl;
    }*/
    
    
    
}

void PreDraw(){
    //setting OPENGL state
   // glEnable(GL_DEBUG_OUTPUT);
    
    //glDisable(GL_DEPTH_TEST);
    //Texture
    
    const std::string& filename = texture1Path;
    SDL_Surface* img = LoadTextureImage(filename);
    
    glTexImage2D(GL_TEXTURE_2D
                 , 0 //mipmap level
                 , GL_RGB
                 , img->w
                 , img->h
                 , 0
                 , GL_RGB
                 , GL_UNSIGNED_BYTE
                 , img->pixels);
    
    //setting up filters
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );
    
   // texture tiling - st are the normalized texture coordinates - normalized (u,v)
    glTexParameteri(
                        GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,//wrap in s direction
                        GL_REPEAT
                        );

    glTexParameteri(
                        GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,//wrap in t direction
                        GL_CLAMP_TO_BORDER
                        );
   
    
    //send texture to shader
    glActiveTexture(GL_TEXTURE0); //set active texture unit we would like to use - GL_TEXTURE0 is the default value cannot be specified
    glBindTexture(GL_TEXTURE_2D, gTextureObject); //binding GL_TEXTURE_2D texture to texture unit 0 - GL_TEXTURE0 // texture unit 0 (GL_TEXTURE0) will use gTextureObject this texture
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    glViewport(0,0,gScreenWidth, gScreenHeight); //size of screen
    glClearColor(0.1f, 0.1f, 0.3f, 1.f);
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    
    glUseProgram(gGraphicsPipelineShaderProgram);
    
   //
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
   
    
    
    //Camera Update
    static Uint32 last_time = SDL_GetTicks();
    m_delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

    m_camera.Update(static_cast<float>(m_delta_time));

    last_time = SDL_GetTicks();
    
    
    
/*
    //Model transformation by translating our object into world space
    glm::mat4  model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));
    
    //UPdate our mkodel matrix by applying a rotation after our translation
    model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    
    model = glm::scale(model, glm::vec3(g_uScale, g_uScale, g_uScale));
    */
    
   // glm::mat4 model = glm::translate(glm::mat4(1.0f), m_camera.getEye());
    glm::mat4 model = m_camera.getViewMatrix();
    //Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");
    
    if(u_ModelMatrixLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);

    } else {
        std::cout << "Could not find u_ModelMatrix."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    // Projection matrix in prespective
  // glm::mat4 perspective = glm::perspective(glm::radians(45.0f), //fovy - how wide can you see - expressed in radians
    //                                         (float)gScreenWidth/(float)gScreenHeight,
     //                                        0.1f, //how close we can see things
      //                                       10.0f); // not further than 10 unit
    
    glm::mat4 identity = glm::mat4(1); //identity matrix
    glm::mat4 perspective = m_camera.getViewProj();//m_camera.getProj(); //
    
    //Retrieve our location of our perspective matrix uniform
    GLint u_ProjectionLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Projection");
    
    if(u_ProjectionLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);

    } else {
        std::cout << "Could not find u_Projection."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    
    //Retrieve our location of our perspective matrix uniform
    GLint u_WorldLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_World");
    
    if(u_WorldLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldLocation,1,GL_FALSE,&identity[0][0]);

    } else {
        std::cout << "Could not find u_World."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    glm::mat4 inverseTranspose = glm::inverse(glm::transpose(identity));
    
    //Retrieve our location of our perspective matrix uniform
    GLint u_WorldITLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_WorldIT");
    
    if(u_WorldITLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldITLocation,1,GL_FALSE,&inverseTranspose[0][0]);

    } else {
        std::cout << "Could not find u_WorldIT."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    
    
    //Retrieve our location of our texture sampler uniform
    GLint u_SamplerLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_sampler2D");
    
    if(u_SamplerLocation >= 0){
        glUniform1i(u_SamplerLocation, 0); // assign for texture unit 0;

    } else {
        std::cout << "Could not find u_sampler2D."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    /*
    //Retrieve our location of our color sampler uniform
    GLint u_ColorLocation =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Color");
    
    if(u_ColorLocation >= 0){
        glUniform3f(u_ColorLocation, (GLfloat)clear_color.x, (GLfloat)clear_color.y, (GLfloat)clear_color.z);

    } else {
        std::cout << "Could not find u_ColorLocation."<< std::endl;
        exit(EXIT_FAILURE);
    }*/
    
    GLint u_Color1Location =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Color1");
    if(u_Color1Location >= 0){
        if(color1)
            glUniform1i(u_Color1Location, 1);
        else
            glUniform1i(u_Color1Location, 0);

    } else {
        std::cout << "Could not find u_Color1Location."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_Color2Location =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Color2");
    if(u_Color2Location >= 0){
        if(color2)
            glUniform1i(u_Color2Location, 1);
        else
            glUniform1i(u_Color2Location, 0);

    } else {
        std::cout << "Could not find u_Color2Location."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_Texture1Location =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Tex1");
    if(u_Texture1Location >= 0){
        if(texture1Switch)
            glUniform1i(u_Texture1Location, 1);
        else
            glUniform1i(u_Texture1Location, 0);

    } else {
        std::cout << "Could not find u_Texture1Location."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_Texture2Location =  glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Tex2");
    if(u_Texture2Location >= 0){
        if(texture2Switch)
            glUniform1i(u_Texture2Location, 1);
        else
            glUniform1i(u_Texture2Location, 0);

    } else {
        std::cout << "Could not find u_Texture2Location."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    glUseProgram(gNormalShaderProgram);
    //Normal shader
    
    GLint u_ModelMatrixLocationNormal =  glGetUniformLocation(gNormalShaderProgram, "u_ModelMatrix");
    if(u_ModelMatrixLocationNormal >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ModelMatrixLocationNormal,1,GL_FALSE,&model[0][0]);

    } else {
        std::cout << "Could not find u_ModelMatrix on normal shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Normal shader
    GLint u_ProjectionLocationNormal =  glGetUniformLocation(gNormalShaderProgram, "u_Projection");
    
    if(u_ProjectionLocationNormal >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ProjectionLocationNormal,1,GL_FALSE,&perspective[0][0]);

    } else {
        std::cout << "Could not find u_Projection on normal shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Normal shader
    GLint u_WorldLocationNormal =  glGetUniformLocation(gNormalShaderProgram, "u_World");
    
    if(u_WorldLocationNormal >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldLocationNormal,1,GL_FALSE,&identity[0][0]);

    } else {
        std::cout << "Could not find u_World on normal shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Normal shader
    GLint u_WorldITLocationNormal =  glGetUniformLocation(gNormalShaderProgram, "u_WorldIT");
    
    if(u_WorldITLocationNormal >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldITLocationNormal,1,GL_FALSE,&inverseTranspose[0][0]);

    } else {
        std::cout << "Could not find u_WorldIT on normal shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Normal shader
    GLint u_NormalLengthLocation =  glGetUniformLocation(gNormalShaderProgram, "u_normalLength");
    
    if(u_NormalLengthLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniform1f(u_NormalLengthLocation, normalLength);

    } else {
        std::cout << "Could not find u_normalLength on normal shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    //Rectangle shader uniform
    glUseProgram(gRectangleShaderProgram);
    
    GLint u_ModelMatrixLocationRect =  glGetUniformLocation(gRectangleShaderProgram, "u_ModelMatrix");
    if(u_ModelMatrixLocationRect >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ModelMatrixLocationRect,1,GL_FALSE,&model[0][0]);

    } else {
        std::cout << "Could not find u_ModelMatrix on rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    GLint u_ProjectionLocationRect =  glGetUniformLocation(gRectangleShaderProgram, "u_Projection");
    
    if(u_ProjectionLocationRect >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_ProjectionLocationRect,1,GL_FALSE,&perspective[0][0]);

    } else {
        std::cout << "Could not find u_Projection on rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    GLint u_WorldLocationRect =  glGetUniformLocation(gRectangleShaderProgram, "u_World");
    
    if(u_WorldLocationRect >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldLocationRect,1,GL_FALSE,&identity[0][0]);

    } else {
        std::cout << "Could not find u_World on rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    GLint u_WorldITLocationRect =  glGetUniformLocation(gRectangleShaderProgram, "u_WorldIT");
    
    if(u_WorldITLocationRect >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniformMatrix4fv(u_WorldITLocationRect,1,GL_FALSE,&inverseTranspose[0][0]);

    } else {
        std::cout << "Could not find u_WorldIT on rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_sizeRectLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_rectSize");
    
    if(u_sizeRectLocation >= 0){
        //std::cout << "Location of model matrix location: " << u_ModelMatrixLocation << std::endl;
        glUniform1f(u_sizeRectLocation, rectSize);

    } else {
        std::cout << "Could not find u_rectSize on rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_Color1RectLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_Color1");
    if(u_Color1RectLocation >= 0){
        if(color1rect)
            glUniform1i(u_Color1RectLocation, 1);
        else
            glUniform1i(u_Color1RectLocation, 0);

    } else {
        std::cout << "Could not find u_Color1RectLocation."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_Color2RectLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_Color2");
    if(u_Color2RectLocation >= 0){
        if(color2rect)
            glUniform1i(u_Color2RectLocation, 1);
        else
            glUniform1i(u_Color2RectLocation, 0);

    } else {
        std::cout << "Could not find u_Color2RectLocation."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_TexLeftTopLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_TexLeftTop");
    if(u_TexLeftTopLocation >= 0){
        if(texLeftTop)
            glUniform1i(u_TexLeftTopLocation, 1);
        else
            glUniform1i(u_TexLeftTopLocation, 0);

    } else {
        std::cout << "Could not find u_TexLeftTopLocation in rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_TexLeftBottomLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_TexLeftBottom");
    if(u_TexLeftBottomLocation >= 0){
        if(texLeftBottom)
            glUniform1i(u_TexLeftBottomLocation, 1);
        else
            glUniform1i(u_TexLeftBottomLocation, 0);

    } else {
        std::cout << "Could not find u_TexLeftBottomLocation in rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_TexRightTopLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_TexRightTop");
    if(u_TexRightTopLocation >= 0){
        if(texRightTop)
            glUniform1i(u_TexRightTopLocation, 1);
        else
            glUniform1i(u_TexRightTopLocation, 0);

    } else {
        std::cout << "Could not find u_TexLeftBottomLocation in rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    GLint u_TexRightBottomLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_TexRightBottom");
    if(u_TexRightBottomLocation >= 0){
        if(texRightBottom)
            glUniform1i(u_TexRightBottomLocation, 1);
        else
            glUniform1i(u_TexRightBottomLocation, 0);

    } else {
        std::cout << "Could not find u_TexRightBottomLocation in rectangle shader."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Retrieve our location of our texture sampler uniform
    GLint u_SamplerRectLocation =  glGetUniformLocation(gRectangleShaderProgram, "u_sampler2D");
    
    if(u_SamplerRectLocation >= 0){
        glUniform1i(u_SamplerRectLocation, 0); // assign for texture unit 0;

    } else {
        std::cout << "Could not find u_SamplerRectLocation."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    
}

void Draw(){
    //which vertex object
    GLCheck(glBindVertexArray(gVertexArrayObject);)
    GLCheck(glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);)
    
    GLCheck(glUseProgram(gGraphicsPipelineShaderProgram);)
    GLCheck(glDrawArrays(GL_POINTS, 0, 27);)
    
    GLCheck(glUseProgram(gNormalShaderProgram);)
    GLCheck(glDrawArrays(GL_POINTS, 0, 27);)
    
    GLCheck(glUseProgram(gRectangleShaderProgram);)
    GLCheck(glDrawArrays(GL_POINTS, 0, 27);)
    
    
    glUseProgram(0); //clean up turn the program up
}

void MainLoop(){
    
    while(!gQuit){
        Input();
        
        PreDraw();
        
        Draw();

        //Update the screen
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void CleanUp(){
    // Cleanup Imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    
    SDL_DestroyWindow(gGraphicsApplicationWindow);

    SDL_Quit();
    
    
    
}



int main(){
        
    InitializeProgram();
    
    //Responsible for getting vertex data on GPU
    VertexSpecification();

    //Compiling some source code shipping into the GPU
    CreateGraphicsPipeline();

    MainLoop();
    
    CleanUp();
    
    
          
    return 0;
}


