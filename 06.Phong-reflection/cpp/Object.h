#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h> 
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


#include <GL/glew.h>
#include <iostream>

struct Face
{    
    GLuint index_buffer = 0;
    GLuint num_indices = 0;
};

class Object
{
public:
    Object() 
        : vec_translate(0.0),
          vec_scale(1.0),
          mat_rotate(1.0) {};
    Object(const aiMesh* mesh)
        : mMesh(mesh),
          vec_translate(0.0),
          vec_scale(1.0),
          mat_rotate(1.0){};

    void init_buffer_objects();
    void draw(int loc_a_position, int loc_a_color); // rendering 함수: 물체를 렌더링하는 함수.
    void draw(int loc_a_position, int loc_a_color, int loc_a_normal); 
    void print_info();

    void      set_translate(glm::vec3 vec)  { vec_translate = vec; }
    void      set_scale(glm::vec3 vec) { vec_scale = vec; }
    void      set_rotate(glm::mat4 mat) { mat_rotate = mat; }

    glm::vec3 translate() { return vec_translate; }
    glm::vec3 scale() { return vec_scale; }
    glm::mat4 rotate() { return mat_rotate; }

    glm::mat4 get_model_matrix();

private:

    GLuint  position_buffer; // GPU 메모리에서 vertices_buffer 위치 
    GLuint  color_buffer;    // GPU 메모리에서 color_buffer 위치
    GLuint  normal_buffer;
    bool    is_color = false;

    std::vector<Face> faces;
    const aiMesh* mMesh;

    glm::vec3  vec_translate;
    glm::vec3  vec_scale;
    glm::mat4  mat_rotate;
};
