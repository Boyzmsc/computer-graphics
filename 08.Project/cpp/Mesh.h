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

class Mesh
{
public:
    Mesh() {};
    Mesh(const aiMesh* mesh)
        : mMesh(mesh) {};

    void init_buffer_objects();
    void draw(int loc_a_position, int loc_a_texcoord); 
    void print_info();
    
private:

    GLuint  position_buffer; // GPU 메모리에서 vertices_buffer 위치 
    GLuint  texcoord_buffer;    // GPU 메모리에서 color_buffer 위치
    bool    is_color = false;

    std::vector<Face> faces;
    
    const aiMesh* mMesh;

};
