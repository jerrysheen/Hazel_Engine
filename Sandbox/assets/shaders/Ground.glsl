// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec3 u_CameraPos;

out vec3 v_worldPos;
out vec3 v_camPos;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_worldPos = (u_Transform * vec4(a_Position, 1.0)).xyz;
	v_camPos = u_CameraPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;

in vec3 v_worldPos;
in vec3 v_camPos;


uniform sampler2D u_Texture;
uniform vec4 u_Color;
void main()
{
	vec2 temp;
	vec2 temp1;

	// log10(height);
	float height = (1 / 3.3219) * log(abs(v_camPos.y));
	float raction = height - fract(height);
	temp = fract(v_worldPos.xz / (pow(10, raction + 1)));
	temp1 = fract(v_worldPos.xz / (pow(10, raction)));
	if(raction < -0.0000001f) discard;
	// base color : { 104.0/256.0, 97.0 / 256.0, 92.0 / 256.0, 1 }
	// big grid;
	vec4 color00;
	vec4 color01;
	if(temp.x < 0.0025f || temp.y < 0.0025f) 
	{
		color00 = vec4(116.0/255.0, 113.0/255.0, 116.0/255.0, 1.0);
	}
	else
	{
		color00 = vec4(104.0/255.0, 97.0 / 255.0, 92.0 / 255.0, 1.0);
	}

	// small grid;
	if(temp1.y < 0.015f || temp1.x < 0.015f)
	{
		color01 = vec4(116.0/255.0, 113.0/255.0, 116.0/255.0, 1.0);
	}
	else
	{
		color01 = vec4(104.0/255.0, 97.0 / 255.0, 92.0 / 255.0, 1.0);
	}
	//color = color00;
	//color = color01;
	color = mix(color01, color00, fract(height));
}