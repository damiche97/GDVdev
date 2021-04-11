varying vec3 v_normal;

void main()
{
  v_normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
}
