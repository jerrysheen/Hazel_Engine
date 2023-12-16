// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;
uniform vec3 u_CameraPos;
uniform mat4 u_LightSpaceViewProjection;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

out vec3 WorldPos;
out vec3 v_camPos;
out vec4 v_FragPosLightSpace;
out vec3 v_TexCoord;
out vec3 v_Normal;
void main()
{
    WorldPos = a_Position;

	mat4 rotView = mat4(mat3(u_ViewMatrix));
	vec4 clipPos = u_ProjectionMatrix * rotView * vec4(WorldPos, 1.0);
    //gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
	gl_Position = clipPos.xyww;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;

in vec3 WorldPos;
in vec3 v_camPos;
in vec4 v_FragPosLightSpace;
in vec3 v_TexCoord;
in vec3 v_Normal;

//uniform sampler2D u_ShadowMap;
uniform samplerCube u_SkyboxTexture;
uniform vec4 u_Color;

void main()
{
    // vec3 I = normalize(v_worldPos - v_camPos);
    // vec3 R = reflect(I, normalize(v_Normal));
    // //vec4 reflect_color = texture(u_SkyboxTexture, R) * 1.0f;
    // // Combine them
    // color = texture(u_SkyboxTexture, v_TexCoord);
     vec3 envColor = texture(u_SkyboxTexture, WorldPos).rgb;
     color = vec4(envColor, 1.0f);
    //color =  reflect_color;
}