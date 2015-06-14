#version 110

attribute vec3 position;
attribute vec3 normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 normal_mat;

uniform vec3 mat_diffuse;
uniform int light_enabled;
uniform vec3 light_position;
uniform vec4 light_diffuse;

varying vec3 N;
varying vec3 v;

void main()
{
  mat4 mv_mat = view_mat * model_mat;
  vec4 pos = vec4(position, 1.0);

  if (light_enabled != 0)
  {
    N = normalize(vec3(normal_mat * vec4(normal, 0.0)));
    v = vec3(mv_mat * pos);
  }
  
  mat4 mvp_mat = proj_mat * mv_mat;
  gl_Position = mvp_mat * pos;
}
