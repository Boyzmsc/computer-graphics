#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h> 
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "Mesh.h"



class Model
{
public: 
    Model() {};
    
    bool load_model(const std::string& filename) ;
    void draw(int loc_a_position, int loc_u_diffuse_texture, int loc_a_texcoord);
    bool init_texture_object(std::string filepath);

    void      set_translate(glm::vec3 vec)  { vec_translate = vec; }
    void      set_scale(glm::vec3 vec) { vec_scale = vec; }
    void      set_rotate(glm::mat4 mat) { mat_rotate = mat; }

    glm::vec3   translate() { return vec_translate; }
    glm::vec3   scale() { return vec_scale; }
    glm::mat4   rotate() { return mat_rotate; }
    
    std::vector<Mesh>& meshes() { return mMeshes; }
    // std::string file_name() { return file_name.c_str(); }

    glm::mat4 get_model_matrix();

private :
    std::vector<Mesh> mMeshes;
    std::string file_name;

    bool has_texture = false;
    GLuint texid;

    glm::vec3  vec_translate;
    glm::vec3  vec_scale;
    glm::mat4  mat_rotate;

    

};