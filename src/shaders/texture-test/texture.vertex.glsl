#version 400

in vec3 coord3d;
in vec2 texcoord;
out vec2 texcoord_f;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
 
void main(void) {
  gl_Position = p * v * m * vec4(coord3d, 1.0);
  texcoord_f = texcoord;
}
