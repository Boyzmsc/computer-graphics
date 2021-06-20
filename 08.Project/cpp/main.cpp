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
#include "Model.h"
#include "Mesh.h"
#include "Light.h"

////////////////////////////////////////////////////////////////////////////////
/// initialization 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLFWwindow *createWindow(int width, int height, const char *title);
void init_window(GLFWwindow *window);
bool init_scene_from_file(const std::string &filename);
////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// /// shading 관련 변수
// ////////////////////////////////////////////////////////////////////////////////
glm::vec3 g_clear_color(0.5f, 0.5f, 0.5f);
glm::mat3 mat_normal;
// ////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint program;       // 쉐이더 프로그램 객체의 레퍼런스 값
GLint loc_a_position; // attribute 변수 a_position 위치
GLint loc_u_PVM;      // uniform 변수 u_PVM 위치

GLint loc_u_view_matrix;
GLint loc_u_model_matrix;
GLint loc_u_normal_matrix;

GLint loc_u_camera_position;
GLint loc_u_light_position;

GLint loc_u_light_ambient;
GLint loc_u_light_diffuse;
GLint loc_u_light_specular;

GLint loc_u_obj_ambient;
GLint loc_u_obj_diffuse;
GLint loc_u_obj_specular;
GLint loc_u_obj_shininess;
GLint loc_a_normal;

GLint loc_u_diffuse_texture;
GLint loc_a_texcoord;

GLuint create_shader_from_file(const std::string &filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::mat4 mat_model, mat_view, mat_proj;
glm::mat4 mat_PVM;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
std::vector<Camera> cameras;

float g_aspect = 1.0f;
bool g_is_perspective = true;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
std::vector<Model> models;
std::vector<std::string> model_names;
Light light;

bool load_asset(const std::string &filename);
// void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object(); // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
void render(GLFWwindow *window);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard / scroll input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
int obj_select_idx = 0;
int cam_select_idx = 0;

void init_imgui(GLFWwindow *window);
void compose_imgui_frame(GLFWwindow *window, int key, int scancode, int action, int mods);

void key_callback();
void scroll_callback(GLFWwindow *window, double x, double y);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 결과 출력 함수
////////////////////////////////////////////////////////////////////////////////
void store_info(const std::string &filename);
bool load_info(const std::string &filename);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// ImGuIZMO 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::quat qRot = quat(1.f, 0.f, 0.f, 0.f);
////////////////////////////////////////////////////////////////////////////////

GLFWwindow *createWindow(int width, int height, const char *title)
{
  GLFWwindow *window; // create window

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

void scroll_callback(GLFWwindow *window, double x, double y)
{
  float Fovy = cameras[cam_select_idx].fovy();
  Fovy -= y;
  cameras[cam_select_idx].set_fovy(Fovy);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  glm::vec3 translate = models[obj_select_idx].translate();
  glm::vec3 scale = models[obj_select_idx].scale();

  // move left
  if (key == GLFW_KEY_H && action == GLFW_PRESS)
    translate[0] -= 0.05f;
  // mode right
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    translate[0] += 0.05f;
  // move up
  if (key == GLFW_KEY_K && action == GLFW_PRESS)
    translate[1] += 0.05f;
  // move down
  if (key == GLFW_KEY_J && action == GLFW_PRESS)
    translate[1] -= 0.05f;

  // scale
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    scale += 0.025f;
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    scale -= 0.025f;

  models[obj_select_idx].set_translate(translate);
  models[obj_select_idx].set_scale(scale);

  // Camera Extrinsic Parameter
  // Camera Translate
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    cameras[cam_select_idx].move_left(0.05f);
  if (key == GLFW_KEY_D && action == GLFW_PRESS)
    cameras[cam_select_idx].move_right(0.05f);

  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    cameras[cam_select_idx].move_forward(0.05f);
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    cameras[cam_select_idx].move_backward(0.05f);

  if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    cameras[cam_select_idx].move_down(0.05f);
  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    cameras[cam_select_idx].move_up(0.05f);

  // Camera Rotate
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_z(-5.0f);
  if (key == GLFW_KEY_E && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_z(5.0f);

  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_y(-5.0f);
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_y(5.0f);

  if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_x(5.0f);
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    cameras[cam_select_idx].rotate_x(-5.0f);
}

void init_window(GLFWwindow *window)
{
  init_imgui(window);
  init_shader_program();

  glEnable(GL_DEPTH_TEST);

  // init_buffer_objects();

  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

bool init_scene_from_file(const std::string &filename)
{
  std::ifstream fin(filename);
  if (fin.fail())
    return false;

  // Init Models
  int count;
  fin >> count;
  for (int i = 0; i < count; i++)
  {
    std::string name;
    float s_x, s_y, s_z;
    float t_x, t_y, t_z;
    float r_00, r_10, r_20, r_30;
    float r_01, r_11, r_21, r_31;
    float r_02, r_12, r_22, r_32;
    float r_03, r_13, r_23, r_33;

    fin >> name;
    if (!load_asset(name))
    {
      std::cout << "Failed to load a asset file: " << name << std::endl;
      return -1;
    }

    fin >> s_x >> s_y >> s_z;
    fin >> t_x >> t_y >> t_z;

    fin >> r_00 >> r_10 >> r_20 >> r_30;
    fin >> r_01 >> r_11 >> r_21 >> r_31;
    fin >> r_02 >> r_12 >> r_22 >> r_32;
    fin >> r_03 >> r_13 >> r_23 >> r_33;

    models[i].set_scale(glm::vec3(s_x, s_y, s_z));
    models[i].set_translate(glm::vec3(t_x, t_y, t_z));
    models[i].set_rotate(glm::mat4(r_00, r_01, r_02, r_03,
                                   r_10, r_11, r_12, r_13,
                                   r_20, r_21, r_22, r_23,
                                   r_30, r_31, r_32, r_33));
  }

  // Init Cameras
  fin >> count;

  for (int i = 0; i < count; i++)
  {
    float pos_x, pos_y, pos_z;
    float front_x, front_y, front_z;
    float up_x, up_y, up_z;

    fin >> pos_x >> pos_y >> pos_z >>
        front_x >> front_y >> front_z >>
        up_x >> up_y >> up_z;

    Camera camera(
        glm::vec3(pos_x, pos_y, pos_z),
        glm::vec3(front_x, front_y, front_z),
        glm::vec3(up_x, up_y, up_z),
        45.0f,
        0.0f,
        0.0f);
    cameras.push_back(camera);
  }

  // Init Light
  float bg_r, bg_g, bg_b;
  float lp_x, lp_y, lp_z;
  float ambient_r, ambient_g, ambient_b;
  float diffuse_r, diffuse_g, diffuse_b;
  float specular_r, specular_g, specular_b;

  fin >> bg_r >> bg_g >> bg_b;
  fin >> lp_x >> lp_y >> lp_z;
  fin >> ambient_r >> ambient_g >> ambient_b;
  fin >> diffuse_r >> diffuse_g >> diffuse_b;
  fin >> specular_r >> specular_g >> specular_b;

  g_clear_color = glm::vec3(bg_r, bg_g, bg_b);
  light.pos = glm::vec3(lp_x, lp_y, lp_z);
  light.ambient = glm::vec3(ambient_r, ambient_g, ambient_b);
  light.diffuse = glm::vec3(diffuse_r, diffuse_g, diffuse_b);
  light.specular = glm::vec3(specular_r, specular_g, specular_b);

  return true;
}

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

bool load_asset(const std::string &filename)
{
  Model model;
  if (model.load_model(filename))
  {
    models.push_back(model);
    model_names.push_back(filename);
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
    ImGui::Begin("Model Control");

    for (int i = 0; i < models.size(); i++)
    {
      std::string tmp = model_names[i].c_str();
      size_t start = tmp.find("/");
      size_t end = tmp.find(".");
      std::string name = tmp.substr(start + 1, end - start - 1);
      ImGui::RadioButton(name.c_str(), &obj_select_idx, i);
    }

    glm::vec3 translate = models[obj_select_idx].translate();
    glm::vec3 scale = models[obj_select_idx].scale();

    if (ImGui::SliderFloat3("Tranlsate", glm::value_ptr(translate), -10.0f, 10.0f))
      models[obj_select_idx].set_translate(translate);

    if (ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.0f, 1.5f))
      models[obj_select_idx].set_scale(scale);

    if (ImGui::gizmo3D("Rotation", qRot))
    {
      models[obj_select_idx].set_rotate(glm::mat4_cast(qRot));
    }

    ImGui::End();
  }

  {
    ImGui::Begin("Camera Control");

    for (int i = 0; i < cameras.size(); i++)
    {
      char camera_name[10];
      sprintf(camera_name, "%s %d", "Camera", i + 1);
      ImGui::RadioButton(camera_name, &cam_select_idx, i);
    }
    ImGui::Checkbox("Perspective", &g_is_perspective);

    ImGui::Text("View Direction");
    vec3 dir = vec3(cameras[cam_select_idx].front_direction());
    if (ImGui::gizmo3D("##gizmo2", dir, 100))
    {
      cameras[cam_select_idx].update_front_direction(dir);
    }

    ImGui::End();
  }

  {
    ImGui::Begin("Light Control");

    ImGui::ColorEdit3("Background Color", glm::value_ptr(g_clear_color));

    glm::vec3 vec(-light.pos);
    ImGui::gizmo3D("Light Direction", vec);
    light.pos = -vec;

    ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
    ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
    ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));

    ImGui::End();
  }
  {
    ImGui::Begin("Materials");

    std::vector<Mesh> &mMeshes = models[obj_select_idx].meshes();

    std::string label;
    for (int i = 0; i < mMeshes.size(); ++i)
    {
      label = "Ambient";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mMeshes[i].mMaterial.ambient));
      label = "Diffuse";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mMeshes[i].mMaterial.diffuse));
      label = "Specular";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mMeshes[i].mMaterial.specular));
      label = "Shininess";
      ImGui::SliderFloat(label.c_str(), &mMeshes[i].mMaterial.shininess, 5.0f, 500.0f);
      ImGui::NewLine();
    }

    ImGui::End();
  }

  {
    ImGui::Begin("Load/Store");

    if (ImGui::Button("Save", ImVec2(125.0f, 25.0f)))
    {
      store_info("output.txt");
    }
    if (ImGui::Button("Load", ImVec2(125.0f, 25.0f)))
    {
      if (!load_info("output.txt"))
      {
        std::cout << "Failed to load a info file" << std::endl;
      }
    }

    ImGui::End();
  }
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

  loc_u_diffuse_texture = glGetUniformLocation(program, "u_diffuse_texture");
  loc_a_texcoord = glGetAttribLocation(program, "a_texcoord");

  loc_a_normal = glGetAttribLocation(program, "a_normal");

  loc_u_model_matrix = glGetUniformLocation(program, "u_model_matrix");
  loc_u_normal_matrix = glGetUniformLocation(program, "u_normal_matrix");

  loc_u_camera_position = glGetUniformLocation(program, "u_camera_position");
  loc_u_view_matrix = glGetUniformLocation(program, "u_view_matrix");

  loc_u_light_position = glGetUniformLocation(program, "u_light_position");
  loc_u_light_ambient = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular = glGetUniformLocation(program, "u_light_specular");

  loc_u_obj_ambient = glGetUniformLocation(program, "u_obj_ambient");
  loc_u_obj_diffuse = glGetUniformLocation(program, "u_obj_diffuse");
  loc_u_obj_specular = glGetUniformLocation(program, "u_obj_specular");
  loc_u_obj_shininess = glGetUniformLocation(program, "u_obj_shininess");
}

void render_object()
{
  // set transform
  mat_view = cameras[cam_select_idx].get_view_matrix();

  if (g_is_perspective)
    mat_proj = glm::perspective(glm::radians(cameras[cam_select_idx].fovy()), g_aspect, 0.1f, 10000.0f);
  else
    mat_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // TODO : send data to GPU
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, glm::value_ptr(mat_view));
  glUniform3fv(loc_u_camera_position, 1, glm::value_ptr(cameras[cam_select_idx].position()));

  glUniform3fv(loc_u_light_position, 1, glm::value_ptr(light.pos));
  glUniform3fv(loc_u_light_ambient, 1, glm::value_ptr(light.ambient));
  glUniform3fv(loc_u_light_diffuse, 1, glm::value_ptr(light.diffuse));
  glUniform3fv(loc_u_light_specular, 1, glm::value_ptr(light.specular));

  for (int i = 0; i < models.size(); ++i)
  {
    mat_model = models[i].get_model_matrix();
    glUniformMatrix4fv(loc_u_model_matrix, 1, GL_FALSE, glm::value_ptr(mat_model));

    mat_normal = mat3(transpose(inverse(mat_model)));
    glUniformMatrix3fv(loc_u_normal_matrix, 1, GL_FALSE, glm::value_ptr(mat_normal));

    mat_PVM = mat_proj * mat_view * mat_model;
    glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, glm::value_ptr(mat_PVM));

    models[i].draw(loc_a_position, loc_a_normal, loc_u_obj_ambient, loc_u_obj_diffuse, loc_u_obj_specular, loc_u_obj_shininess, loc_u_diffuse_texture, loc_a_texcoord);
  }

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

void render(GLFWwindow *window)
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

// Store Func
void store_info(const std::string &filename)
{
  std::ofstream fout;
  fout.open(filename);

  // Model
  fout << models.size() << "\n";

  for (int i = 0; i < models.size(); i++)
  {
    fout << model_names[i].c_str() << "\n";

    glm::vec3 translate = models[i].translate();
    glm::vec3 scale = models[i].scale();
    glm::mat4 rotate = models[i].rotate();

    // current scale
    fout << scale[0] << " " << scale[1] << " " << scale[2] << "\n";
    // current translate
    fout << translate[0] << " " << translate[1] << " " << translate[2] << "\n";
    // current rotate
    fout << rotate[0][0] << " " << rotate[1][0] << " " << rotate[2][0] << " " << rotate[3][0] << "\n";
    fout << rotate[0][1] << " " << rotate[1][1] << " " << rotate[2][1] << " " << rotate[3][1] << "\n";
    fout << rotate[0][2] << " " << rotate[1][2] << " " << rotate[2][2] << " " << rotate[3][2] << "\n";
    fout << rotate[0][3] << " " << rotate[1][3] << " " << rotate[2][3] << " " << rotate[3][3] << "\n";

    // current material
    std::vector<Mesh> mMeshes = models[i].meshes();

    for (int j = 0; j < mMeshes.size(); ++j)
    {
      fout << mMeshes[j].mMaterial.ambient[0] << " " << mMeshes[j].mMaterial.ambient[1] << " " << mMeshes[j].mMaterial.ambient[2] << "\n";
      fout << mMeshes[j].mMaterial.diffuse[0] << " " << mMeshes[j].mMaterial.diffuse[1] << " " << mMeshes[j].mMaterial.diffuse[2] << "\n";
      fout << mMeshes[j].mMaterial.specular[0] << " " << mMeshes[j].mMaterial.specular[1] << " " << mMeshes[j].mMaterial.specular[2] << "\n";
      fout << mMeshes[j].mMaterial.shininess << "\n";
    }
  }

  // Camera
  fout << cameras.size() << "\n";

  for (int i = 0; i < cameras.size(); i++)
  {
    glm::vec3 position = cameras[i].position();
    glm::vec3 front_dir = cameras[i].front_direction();
    glm::vec3 up_dir = cameras[i].up_direction();

    // current position
    fout << position[0] << " " << position[1] << " " << position[2] << "\n";
    // current front_direction
    fout << front_dir[0] << " " << front_dir[1] << " " << front_dir[2] << "\n";
    // current up_direction
    fout << up_dir[0] << " " << up_dir[1] << " " << up_dir[2] << "\n";
  }

  // Background Color
  fout << g_clear_color[0] << " " << g_clear_color[1] << " " << g_clear_color[2] << "\n";

  // Light Position
  fout << light.pos[0] << " " << light.pos[1] << " " << light.pos[2] << "\n";

  // Light Color
  fout << light.ambient[0] << " " << light.ambient[1] << " " << light.ambient[2] << "\n";
  fout << light.diffuse[0] << " " << light.diffuse[1] << " " << light.diffuse[2] << "\n";
  fout << light.specular[0] << " " << light.specular[1] << " " << light.specular[2] << "\n";
  fout.close();
}

// Load Func
bool load_info(const std::string &filename)
{
  std::ifstream fin(filename);
  if (fin.fail())
    return false;

  // Model
  int count;
  fin >> count;
  for (int i = 0; i < count; i++)
  {
    std::string name;
    float s_x, s_y, s_z;
    float t_x, t_y, t_z;
    float r_00, r_10, r_20, r_30;
    float r_01, r_11, r_21, r_31;
    float r_02, r_12, r_22, r_32;
    float r_03, r_13, r_23, r_33;

    fin >> name;
    fin >> s_x >> s_y >> s_z;
    fin >> t_x >> t_y >> t_z;

    fin >> r_00 >> r_10 >> r_20 >> r_30;
    fin >> r_01 >> r_11 >> r_21 >> r_31;
    fin >> r_02 >> r_12 >> r_22 >> r_32;
    fin >> r_03 >> r_13 >> r_23 >> r_33;

    models[i].set_scale(glm::vec3(s_x, s_y, s_z));
    models[i].set_translate(glm::vec3(t_x, t_y, t_z));
    models[i].set_rotate(glm::mat4(r_00, r_01, r_02, r_03,
                                   r_10, r_11, r_12, r_13,
                                   r_20, r_21, r_22, r_23,
                                   r_30, r_31, r_32, r_33));

    // material
    std::vector<Mesh> &mMeshes = models[i].meshes();

    for (int j = 0; j < mMeshes.size(); ++j)
    {
      float a_r, a_g, a_b;
      float d_r, d_g, d_b;
      float s_r, s_g, s_b;
      float sh;

      fin >> a_r >> a_g >> a_b;
      fin >> d_r >> d_g >> d_b;
      fin >> s_r >> s_g >> s_b;
      fin >> sh;

      mMeshes[j].mMaterial.ambient = glm::vec3(a_r, a_g, a_b);
      mMeshes[j].mMaterial.diffuse = glm::vec3(d_r, d_g, d_b);
      mMeshes[j].mMaterial.specular = glm::vec3(s_r, s_g, s_b);
      mMeshes[j].mMaterial.shininess = sh;
    }
  }

  // Camera
  fin >> count;

  for (int i = 0; i < count; i++)
  {
    float pos_x, pos_y, pos_z;
    float front_x, front_y, front_z;
    float up_x, up_y, up_z;

    fin >> pos_x >> pos_y >> pos_z >>
        front_x >> front_y >> front_z >>
        up_x >> up_y >> up_z;

    Camera camera(
        glm::vec3(pos_x, pos_y, pos_z),
        glm::vec3(front_x, front_y, front_z),
        glm::vec3(up_x, up_y, up_z),
        45.0f,
        0.0f,
        0.0f);

    cameras[i] = camera;
  }

  // Light
  float bg_r, bg_g, bg_b;
  float lp_x, lp_y, lp_z;
  float ambient_r, ambient_g, ambient_b;
  float diffuse_r, diffuse_g, diffuse_b;
  float specular_r, specular_g, specular_b;

  fin >> bg_r >> bg_g >> bg_b;
  fin >> lp_x >> lp_y >> lp_z;
  fin >> ambient_r >> ambient_g >> ambient_b;
  fin >> diffuse_r >> diffuse_g >> diffuse_b;
  fin >> specular_r >> specular_g >> specular_b;

  g_clear_color = glm::vec3(bg_r, bg_g, bg_b);
  light.pos = glm::vec3(lp_x, lp_y, lp_z);
  light.ambient = glm::vec3(ambient_r, ambient_g, ambient_b);
  light.diffuse = glm::vec3(diffuse_r, diffuse_g, diffuse_b);
  light.specular = glm::vec3(specular_r, specular_g, specular_b);

  return true;
}

int main(int argc, char *argv[])
{
  // create window
  GLFWwindow *window = createWindow(1500, 1500, "Project");

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
