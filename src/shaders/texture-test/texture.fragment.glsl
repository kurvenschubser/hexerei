#version 400

in vec2 texcoord_f;
uniform sampler2D tex;
 
void main(void) {
  gl_FragColor = texture2D(tex, texcoord_f);
}
