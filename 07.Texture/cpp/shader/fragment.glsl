#version 120                  // GLSL 1.20

varying   vec2 v_texcoord; 

uniform sampler2D u_diffuse_texture;

void main()
{

	gl_FragColor = texture2D(u_diffuse_texture, v_texcoord);

	// gl_FragColor = vec4(v_color, 1.0f);
}