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
out vec2 v_TexCoord;
void main()
{
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
	//gl_Position = u_LightSpaceViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
	v_worldPos = (u_ModelMatrix * vec4(a_Position, 1.0)).xyz;
	v_camPos = u_CameraPos;
	v_FragPosLightSpace = u_LightSpaceViewProjection * vec4(v_worldPos, 1.0);;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform float u_TilingFactor;

in vec3 v_worldPos;
in vec3 v_camPos;
in vec4 v_FragPosLightSpace;
in vec2 v_TexCoord;

uniform sampler2D u_ShadowMap;
uniform sampler2D u_Texture;
uniform vec4 u_Color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth> closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	color = u_Color;
	float Inshadow = ShadowCalculation(v_FragPosLightSpace);
    //color *= (1.0 - Inshadow);
	color.xyz = vec3(1.0 - Inshadow, 1.0 - Inshadow, 1.0 - Inshadow);
	color.a = 1.0f;

	vec3 projCoords = v_FragPosLightSpace.xyz / v_FragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(u_ShadowMap, v_TexCoord).r; 
	float depth = projCoords.z;

	float diff = depth - closestDepth;
	color.xyz = vec3(v_TexCoord.x, v_TexCoord.y, 0.0f);

}