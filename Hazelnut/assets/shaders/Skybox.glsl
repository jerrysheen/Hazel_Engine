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

out vec3 v_worldPos;
out vec3 v_camPos;
out vec4 v_FragPosLightSpace;
out vec3 v_TexCoord;
out vec3 v_Normal;
void main()
{
	v_TexCoord = a_Position;

	vec4 pos = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
    pos = u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
    
    //gl_Position = u_LightSpaceViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
	v_worldPos = (u_ModelMatrix * vec4(a_Position, 1.0)).xyz;
	v_camPos = u_CameraPos;
    v_Normal = mat3(transpose(inverse(u_ModelMatrix))) * a_Normal;
	v_FragPosLightSpace = u_LightSpaceViewProjection * vec4(v_worldPos, 1.0);;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;

in vec3 v_worldPos;
in vec3 v_camPos;
in vec4 v_FragPosLightSpace;
in vec3 v_TexCoord;
in vec3 v_Normal;

//uniform sampler2D u_ShadowMap;
uniform samplerCube u_SkyboxTexture;
uniform vec4 u_Color;

void main()
{
    vec3 I = normalize(v_worldPos - v_camPos);
    vec3 R = reflect(I, normalize(v_Normal));
    //vec4 reflect_color = texture(u_SkyboxTexture, R) * 1.0f;
    // Combine them
    color = texture(u_SkyboxTexture, v_TexCoord);
    //color =  reflect_color;
}