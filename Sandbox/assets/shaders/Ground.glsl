// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_worldPos;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_worldPos = (u_Transform * vec4(a_Position, 1.0)).xyz;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;
in vec3 v_worldPos;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
void main()
{
	vec2 temp;
	// v_worldPos.x  = v_worldPos.y * v_worldPos.x;
	// v_worldPos.z = v_worldPos.y * v_worldPos.z;
	temp = fract(v_worldPos.xz);
	//modf(v_TexCoord.x, x);
	if(temp.x < 0.01f || temp.y < 0.01f)
	{
		color = vec4(116.0/255.0, 113.0/255.0, 116.0/255.0, 1.0);
	}
	else
	{
		discard;
	}
}