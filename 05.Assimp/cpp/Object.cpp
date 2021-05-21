#include "Object.h"

void Object::init_buffer_objects()
{
    // TODO
    std::vector<unsigned int> indices;
    aiFace face;
    for (int i = 0; i < mMesh->mNumFaces; i++)
    {
        face = mMesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, mMesh->mNumVertices * sizeof(aiVector3D), &mMesh->mVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, mMesh->mNumVertices * sizeof(aiColor4D), &mMesh->mColors[0][0], GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

glm::mat4 Object::get_model_matrix()
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    // TODO
    mat_model = mat_model * glm::translate(glm::mat4(1.0f), Object::translate());
    mat_model = mat_model * Object::rotate();
    mat_model = mat_model * glm::scale(glm::mat4(1.0f), Object::scale());

    return mat_model;
}

void Object::draw(int loc_a_position, int loc_a_color)
{
    // TODO
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
    glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // IBO를 이용해 물체 그리기
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glDrawElements(GL_TRIANGLES, mMesh->mNumFaces * mMesh->mFaces->mNumIndices, GL_UNSIGNED_INT, (void *)0);

    // 정점 attribute 배열 비활성화
    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_color);
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