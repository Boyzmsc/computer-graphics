#include "Object.h"

void Object::init_buffer_objects() 
{
    // TODO
}

glm::mat4 Object::get_model_matrix()
{
    glm::mat4 mat_model = glm::mat4(1.0f);
    
    // TODO

    return mat_model;
}

void Object::draw(int loc_a_position, int loc_a_color)
{
    // TODO
}
    
void Object::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << mMesh->mNumVertices << std::endl;
    for (int i = 0; i < mMesh->mNumVertices; ++i)
    {
        aiVector3D vertex = mMesh->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (mMesh->mColors[0] != NULL)
        {
            aiColor4D color = mMesh->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}