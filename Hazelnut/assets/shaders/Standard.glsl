// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
}





#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;
in vec2 v_TexCoord;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;


uniform vec4 u_Color;
void main()
{
	//color = vec4(v_TexCoord.x, v_TexCoord.y, 0.0, 1.0);
	color = texture(u_NormalMap, v_TexCoord *  u_TilingFactor);
	color.a = 1.0f;
}