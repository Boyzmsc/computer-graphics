#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_color;       // per-vertex colr (per-vertex input)
attribute vec3 a_normal;      // per-vertex normal

varying   vec3 v_color;

uniform mat4 u_PVM;

// for phong shading
uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform float u_obj_shininess;

uniform vec3 u_camera_position;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;


void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);


  float ambientStrength = 0.2;
  vec3 ambient = ambientStrength * u_light_color;

  // world coordinate
  vec3 position_wc = (u_model_matrix * vec4(a_position, 1.0f)).xyz; 
  vec3 normal_wc   = normalize(u_normal_matrix * a_normal); 

  // set ambient
  vec3 light_dir = normalize(u_light_position - position_wc);
  float diff = max(dot(normal_wc, light_dir), 0.0);
  vec3 diffuse = diff * u_light_color;

  // set reflect vector
  vec3 reflect_dir = reflect(-light_dir, normal_wc);

  // set view vector 
  vec3 view_dir = normalize(u_camera_position - position_wc);

  // set specular 
  float rdotv = pow(max(dot(view_dir, reflect_dir), 0.0), u_obj_shininess);
  vec3 specular = rdotv * u_light_color;

  vec3 color = (ambient + diffuse + specular) * a_color;

  // to fragment shader
  v_color = color;
}