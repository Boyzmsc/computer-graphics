﻿///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// include imGui and imGuIZMO
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"
#include "imGui/imGuIZMO/imGuIZMOquat.h"

// model hpp
#include "models/avocado_vlist.hpp"

// Camera hpp
#include "Camera.h"

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint program;       // 쉐이더 프로그램 객체의 레퍼런스 값
GLint loc_a_position; // attribute 변수 a_position 위치
GLint loc_a_color;    // attribute 변수 a_color 위치

GLint loc_u_PVM; // uniform 변수 u_PVM 위치

GLuint position_buffer; // GPU 메모리에서 position_buffer의 위치
GLuint color_buffer;    // GPU 메모리에서 color_buffer의 위치
GLuint index_buffer;    // GPU 메모리에서 index_buffer의 위치

GLuint create_shader_from_file(const std::string &filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::mat4 mat_model, mat_view, mat_proj, mat_rot;
glm::mat4 mat_PVM;

glm::vec3 vec_translate(0.0), vec_scale(0.5);

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
Camera g_camera;

bool g_is_perspective = false;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////

void init_buffer_objects(); // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();       // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard / scroll input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
void init_imgui(GLFWwindow *window);
void compose_imgui_frame(GLFWwindow *window, int key, int scancode, int action, int mods);

void key_callback();
void scroll_callback(GLFWwindow *window, double x, double y);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// ImGuIZMO 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::quat qRot = quat(1.f, 0.f, 0.f, 0.f);
////////////////////////////////////////////////////////////////////////////////

void init_imgui(GLFWwindow *window)
{
  const char *glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
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

    ImGui::SliderFloat3("tranlsate", glm::value_ptr(vec_translate), -10.0f, 10.0f);
    ImGui::SliderFloat3("scale", glm::value_ptr(vec_scale), 0.0f, 1.5f);

    ImGui::gizmo3D("rotation", qRot);

    ImGui::End();
  }

  // control window
  {
    ImGui::Begin("camera control");

    ImGui::Checkbox("perspective", &g_is_perspective);

    float yaw = g_camera.yaw();
    float pitch = g_camera.pitch();

    ImGui::Text("view direction");
    vec3 dir = vec3(g_camera.front_direction());
    if (ImGui::gizmo3D("##gizmo2", dir, 100))
    {
      g_camera.update_front_direction(dir);
    }

    ImGui::End();
  }

  // output window
  {
    ImGui::Begin("output");

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "current translate");
    ImGui::Text("x = %.3f, y = %.3f, z = %.3f", vec_translate[0], vec_translate[1], vec_translate[2]);
    ImGui::NewLine();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "current rotation matrix");
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_rot[0][0], mat_rot[1][0], mat_rot[2][0], mat_rot[3][0]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_rot[0][1], mat_rot[1][1], mat_rot[2][1], mat_rot[3][1]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_rot[0][2], mat_rot[1][2], mat_rot[2][2], mat_rot[3][2]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_rot[0][3], mat_rot[1][3], mat_rot[2][3], mat_rot[3][3]);
    ImGui::NewLine();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "current scaling");
    ImGui::Text("x = %.3f, y = %.3f, z = %.3f", vec_scale[0], vec_scale[1], vec_scale[2]);
    ImGui::NewLine();
    ImGui::NewLine();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "model matrix");
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_model[0][0], mat_model[1][0], mat_model[2][0], mat_model[3][0]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_model[0][1], mat_model[1][1], mat_model[2][1], mat_model[3][1]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_model[0][2], mat_model[1][2], mat_model[2][2], mat_model[3][2]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", mat_model[0][3], mat_model[1][3], mat_model[2][3], mat_model[3][3]);
    ImGui::NewLine();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "fov : %.3f", g_camera.fovy());

    glm::mat4 view_mat = g_camera.get_view_matrix();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "view matrix");
    ImGui::Text("%.3f %.3f %.3f %.3f ", view_mat[0][0], view_mat[1][0], view_mat[2][0], view_mat[3][0]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", view_mat[0][1], view_mat[1][1], view_mat[2][1], view_mat[3][1]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", view_mat[0][2], view_mat[1][2], view_mat[2][2], view_mat[3][2]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", view_mat[0][3], view_mat[1][3], view_mat[2][3], view_mat[3][3]);
    ImGui::NewLine();

    ImGui::End();
  }
}

void scroll_callback(GLFWwindow *window, double x, double y)
{
  // TODO
  g_camera.set_fovy((float)(g_camera.fovy() - (float)y));
  if (g_camera.fovy() < 1.0f)
  {
    g_camera.set_fovy(1.0f);
  }
  if (g_camera.fovy() > 90.0f)
  {
    g_camera.set_fovy(90.0f);
  }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // move left
  if (key == GLFW_KEY_H && action == GLFW_PRESS)
    vec_translate[0] -= 0.1f;
  // move right
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    vec_translate[0] += 0.1f;
  // move up
  if (key == GLFW_KEY_K && action == GLFW_PRESS)
    vec_translate[1] += 0.1f;
  // move down
  if (key == GLFW_KEY_J && action == GLFW_PRESS)
    vec_translate[1] -= 0.1f;

  // scale
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    vec_scale += 0.1f;
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    vec_scale -= 0.1f;

  // TODO
  // camera move forward
  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    g_camera.move_forward(0.1f);
  // camera move backward
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    g_camera.move_backward(0.1f);

  // camera move right
  if (key == GLFW_KEY_D && action == GLFW_PRESS)
    g_camera.move_right(0.1f);
  // camera move left
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    g_camera.move_left(0.1f);
}

// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string &filename, GLuint shader_type)
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
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0) // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3); // Now get rid of the BOM.
  }

  const GLchar *shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar **)&shader_src, NULL);
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
  GLuint vertex_shader = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

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
}

void init_buffer_objects()
{
  // VBO
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(avocado::vlist::position), avocado::vlist::position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(avocado::vlist::color), avocado::vlist::color, GL_STATIC_DRAW);

  // IBO
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(avocado::vlist::index), avocado::vlist::index, GL_STATIC_DRAW);
}

void set_transform()
{
  mat_view = glm::mat4(1.0f);
  mat_proj = glm::mat4(1.0f);
  mat_model = glm::mat4(1.0f);

  // TODO
  if (g_is_perspective)
  {
    g_camera.set_mode(g_camera.kPerspective);
    mat_proj = glm::perspective(glm::radians(g_camera.fovy()), 1.0f, 0.01f, 100.0f);
  }
  else
  {
    g_camera.set_mode(g_camera.kOrtho);
    mat_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
  }

  mat_view = g_camera.get_view_matrix();

  mat_rot = glm::mat4_cast(qRot);
  mat_model = mat_model * glm::translate(vec_translate);
  mat_model = mat_model * mat_rot;
  mat_model = mat_model * glm::scale(glm::mat4(1.0f), vec_scale);
}

void render_object()
{
  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, glm::value_ptr(mat_PVM));

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 color_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // 버텍스 쉐이더의 attribute 중 a_color 부분 활성화
  glEnableVertexAttribArray(loc_a_color);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_color에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_color, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  // IBO를 이용해 물체 그리기
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glDrawElements(GL_TRIANGLES, avocado::vlist::num_index, GL_UNSIGNED_INT, (void *)0);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

int main(void)
{
  GLFWwindow *window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(800, 800, "Camera", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // Make the current OpenGL context as one in the window
  glfwMakeContextCurrent(window);

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
    std::cout << "GLEW Init Error!" << std::endl;

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  init_imgui(window);
  init_shader_program();

  init_buffer_objects();

  glfwSetKeyCallback(window, key_callback);
  // TODO
  glfwSetScrollCallback(window, scroll_callback);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glfwPollEvents();
    compose_imgui_frame();

    set_transform();
    render_object();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
