varying vec3 v_normal;

void main()
{
	vec3 lightDir = vec3(gl_LightSource[0].position);
	float intensity = dot(lightDir, normalize(v_normal));

  float colorMod;
	vec4 color;
	if (intensity > 0.95)
    colorMod = 1.0;
	else if (intensity > 0.5)
    colorMod = 0.6;
	else if (intensity > 0.25)
    colorMod = 0.4;
	else
    colorMod = 0.2;

  // Use normal as color source
  color = colorMod * vec4(v_normal, 1.0);

	gl_FragColor = color;
}
