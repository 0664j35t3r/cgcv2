#version 110

uniform int light_enabled;
uniform vec3 light_position;
uniform vec4 light_diffuse;
uniform vec3 mat_diffuse;
varying vec3 N;
varying vec3 v;

void main()
{
  vec4 final_color = vec4(0);
  if (light_enabled != 0)
  {
    vec3 L = normalize(light_position - v);
    float lambert = dot(N, L);
    if (lambert > 0.0)
    {
      final_color += vec4(mat_diffuse, 1) * light_diffuse * lambert;
    }
    
    final_color = clamp(final_color, 0.0, 1.0);
  }
  else
  {
    final_color = vec4(mat_diffuse, 1);
  }

  gl_FragColor = final_color;
}
