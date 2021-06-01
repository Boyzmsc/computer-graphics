///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <cassert>
#include <map>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// include imGui and imGuIZMO
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"
#include "imGui/imGuIZMO/imGuIZMOquat.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Camera.h"
#include "Object.h"

////////////////////////////////////////////////////////////////////////////////
/// initialization 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* createWindow(int width, int height, const char* title);
void init_window(GLFWwindow* window);
bool init_scene_from_file(const std::string& filename);
////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// /// shading 관련 변수
// //////////////////////////////////////////////////////////////////////////////// 
glm::vec3 g_light_position(0.0f, 1.0f, 0.0f);
glm::vec3 g_light_color(1.0f, 1.0f, 1.0f);

float     g_obj_shininess = 5.0f;

glm::vec3 g_clear_color(0.5f, 0.5f, 0.5f);

glm::mat3 mat_normal;
// ////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치

GLint   loc_u_view_matrix;
GLint   loc_u_model_matrix;
GLint   loc_u_normal_matrix;

GLint   loc_u_camera_position;
GLint   loc_u_light_position;
GLint   loc_u_light_color;
GLint   loc_u_obj_shininess;
GLint   loc_a_normal;

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치
GLuint  index_buffer;     // GPU 메모리에서 index_buffer의 위치

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::mat4     mat_model, mat_view, mat_proj;
glm::mat4     mat_PVM;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
std::vector<Camera> cameras;

float g_aspect = 1.0f;
bool  g_is_perspective = true;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
const aiScene* scene;
std::vector<Object> objects;
std::vector<std::string> object_names;

bool load_asset(const std::string& filename);
// void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
void render(GLFWwindow* window);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard / scroll input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
int obj_select_idx = 0;
int cam_select_idx = 0;

void init_imgui(GLFWwindow* window);
void compose_imgui_frame(GLFWwindow* window, int key, int scancode, int action, int mods);

void key_callback();
void scroll_callback(GLFWwindow* window, double x, double y);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// ImGuIZMO 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::quat qRot = quat(1.f, 0.f, 0.f, 0.f); 
////////////////////////////////////////////////////////////////////////////////

GLFWwindow* createWindow(int width, int height, const char* title)
{
  GLFWwindow* window; // create window

  // Initialize GLFW Library
  if (!glfwInit())
    return NULL;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return NULL;
  }

  // Make the current OpenGL contexts as one in the window
  glfwMakeContextCurrent(window); 

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
  {
    std::cout << "GLEW Init Error!" << std::endl;
  }

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  return window;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
  float Fovy = cameras[cam_select_idx].fovy();
  Fovy += y;
  cameras[cam_select_idx].set_fovy(Fovy);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  glm::vec3 translate = objects[obj_select_idx].translate();
  glm::vec3 scale = objects[obj_select_idx].scale();
  
  // move left
  if (key == GLFW_KEY_H && action == GLFW_PRESS) 
    translate[0] -= 0.1f;
  // mode right
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    translate[0] += 0.1f;
  // move up
  if (key == GLFW_KEY_K && action == GLFW_PRESS)
    translate[1] += 0.1f;
  // move down 
  if (key == GLFW_KEY_J && action == GLFW_PRESS)
    translate[1] -= 0.1f;

  // scale
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    scale += 0.1f;
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    scale -= 0.1f;
  
  objects[obj_select_idx].set_translate(translate);
  objects[obj_select_idx].set_scale(scale);

  // camera extrinsic parameter
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    cameras[cam_select_idx].move_left(0.1f);
  if (key == GLFW_KEY_D && action ==GLFW_PRESS)
  cameras[cam_select_idx].move_right(0.1f);
  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    cameras[cam_select_idx].move_forward(0.1f);
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    cameras[cam_select_idx].move_backward(0.1f);
}

void init_window(GLFWwindow* window) 
{
  init_imgui(window);
  init_shader_program();

  glEnable(GL_DEPTH_TEST);

  // init_buffer_objects();

  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

bool init_scene_from_file(const std::string& filename)
{
  std::ifstream fin(filename);
  if (fin.fail()) 
    return false;

  int count;
  fin >> count;
  for (int i = 0; i < count; i++)
  {
    std::string name;
    float scale, x, y, z;

    fin >> name;
    if (!load_asset(name))
    { 
      std::cout << "Failed to load a asset file: " << name << std::endl;
      return -1;
    }

    fin >> scale >> x >> y >> z;

    objects[i].set_scale(glm::vec3(scale));
    objects[i].set_translate(glm::vec3(x, y, z));
  }

  // init cameras
  fin >> count;

  for (int i = 0; i < count; i++)
  {
    float pos_x, pos_y, pos_z;
    float front_x, front_y, front_z;
    float up_x, up_y, up_z;

    fin >> pos_x >> pos_y >> pos_z >> 
      front_x >> front_y >> front_z >>
        up_x >> up_y >> up_z ;

    Camera camera(
      glm::vec3(pos_x, pos_y, pos_z),
      glm::vec3(front_x, front_y, front_z),
      glm::vec3(up_x, up_y, up_z),
      45.0f,
      0.0f,
      0.0f
    );
    cameras.push_back(camera);
  }

  return true;
}

void init_imgui(GLFWwindow* window) 
{
  const char* glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}


bool load_asset(const std::string& filename)
{  
  const aiScene* curr_scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
  if (curr_scene != NULL)
  {
    for (int i = 0; i < curr_scene->mNumMeshes; ++i) 
    {
      Object obj;

      obj = Object(curr_scene->mMeshes[i]);
      obj.init_buffer_objects();
      objects.push_back(obj);
      object_names.push_back(filename.c_str());
    }

    return true;
  }
  else 
    return false;
}

void compose_imgui_frame()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // control window
  {
    ImGui::Begin("model control");

    
    for (int i = 0; i < object_names.size(); i++)
    {
      ImGui::RadioButton(object_names[i].c_str(), &obj_select_idx, i);
    }
    
    glm::vec3 translate = objects[obj_select_idx].translate();
    glm::vec3 scale = objects[obj_select_idx].scale();

    if (ImGui::SliderFloat3("tranlsate", glm::value_ptr(translate), -10.0f, 10.0f))
      objects[obj_select_idx].set_translate(translate);
    
    if (ImGui::SliderFloat3("scale", glm::value_ptr(scale), 0.0f, 1.5f))
      objects[obj_select_idx].set_scale(scale);

    if(ImGui::gizmo3D("rotation", qRot))
    {
      objects[obj_select_idx].set_rotate(glm::mat4_cast(qRot));
    }
    
    ImGui::End();
  }

  // control window
  {
    ImGui::Begin("camera control");

    ImGui::RadioButton("camera 0", &cam_select_idx, 0);
    ImGui::RadioButton("camera 1", &cam_select_idx, 1);
    ImGui::Checkbox("perspective", &g_is_perspective);
    
    ImGui::Text("view direction");
    vec3 dir = vec3(cameras[cam_select_idx].front_direction());
    if (ImGui::gizmo3D("##gizmo2", dir, 100))
    {
      cameras[cam_select_idx].update_front_direction(dir);
    }

    
    ImGui::End();
  }

  {
    ImGui::Begin("phong shading");

    ImGui::ColorEdit3("background color", &g_clear_color[0]);

    glm::vec3 light(-g_light_position); 
    ImGui::gizmo3D("Light direction", light);
    g_light_position = -light;

    ImGui::ColorEdit3("Light color", &g_light_color[0]);
    ImGui::SliderFloat("shininess", &g_obj_shininess, 0.0f, 500.0f);

    ImGui::End();
  }
}

// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  // Get rid of BOM in the head of shader_string
  // Because, some GLSL compiler (e.g., Mesa Shader compiler) cannot handle UTF-8 with BOM
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0)  // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3);                  // Now get rid of the BOM.
  }

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar * *)& shader_src, NULL);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled != GL_TRUE)
  {
    std::cout << "Shader COMPILE error: " << std::endl;

    GLint buf_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked != GL_TRUE)
  {
    std::cout << "Shader LINK error: " << std::endl;

    GLint buf_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetProgramInfoLog(program, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteProgram(program);
    program = 0;
  }

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");

  // TODO : get locations 

}

void render_object()
{
  // set transform
  mat_view = cameras[cam_select_idx].get_view_matrix();

  if (g_is_perspective) 
    mat_proj = glm::perspective(glm::radians(cameras[cam_select_idx].fovy()), g_aspect, 0.1f, 1000.0f);
  else 
    mat_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);


  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // TODO : send camera data to GPU 

  for (int i = 0; i < objects.size(); ++i)
  {
    mat_model = objects[i].get_model_matrix();
    // TODO : set normal
    
    mat_PVM = mat_proj * mat_view * mat_model;

    // TODO : send data to GPU

    objects[i].draw(loc_a_position, loc_a_color, loc_a_normal);
  }

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

void render(GLFWwindow* window) 
{
  glClearColor(g_clear_color[0], g_clear_color[1], g_clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwPollEvents();
  compose_imgui_frame();

  render_object();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap front and back buffers
  glfwSwapBuffers(window);

  // Poll for and process events
  glfwPollEvents();
}


int main(int argc, char* argv[])
{
  // create window
  GLFWwindow* window = createWindow(1000, 1000, "Hello Assimp");

  // initialize window
  init_window(window);
  if (!init_scene_from_file("info.txt"))
  {
    std::cout << "Failed to load a info file" << std::endl;
    return -1;
  }

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    render(window);
  }

  glfwTerminate();

  return 0;
}
