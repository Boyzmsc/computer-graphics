#version 120                  // GLSL 1.20

uniform mat4 u_PVM;

// for phong shading
uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;

uniform vec3 u_light_position;
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform vec3  u_obj_ambient;
uniform vec3  u_obj_diffuse;
uniform vec3  u_obj_specular;
uniform float u_obj_shininess;

uniform vec3 u_camera_position;
uniform mat4 u_view_matrix;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;

void main()
{
    v_texcoord = a_texcoord;
    v_position = a_position;
    v_normal = a_normal;
    gl_Position = u_PVM * vec4(a_position, 1.0f);
}