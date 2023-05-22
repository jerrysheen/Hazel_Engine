// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;
uniform mat4 u_LightSpaceViewProjection;

out VS_OUT {
    vec2 v_TexCoord;
    mat3 TBN;
	vec3 WorldPos;
    vec4 FragPosLightSpace;
} vs_out;  


void main()
{
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
    //gl_Position = u_LightSpaceViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
	vs_out.v_TexCoord = a_TexCoord;

	vec3 T = normalize(vec3(u_ModelMatrix * vec4(a_Tangent,   0.0)));
	vec3 N = normalize(vec3(u_ModelMatrix * vec4(a_Normal,    0.0)));
	vec3 B = normalize(cross(N, T));
	//vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	
	vs_out.WorldPos = (u_ModelMatrix * vec4(a_Position, 1.0)).xyz;
	vs_out.TBN = mat3(T, B, N);
    vs_out.FragPosLightSpace = u_LightSpaceViewProjection * vec4(vs_out.WorldPos, 1.0);;
}





#type fragment
#version 330 core

#define PI 3.1415926
layout(location = 0) out vec4 color;

uniform float u_TilingFactor;
in VS_OUT {
    vec2 v_TexCoord;
    mat3 TBN;
	vec3 WorldPos;
    vec4 FragPosLightSpace;
} fs_in;

// texture.
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_AoMap;
uniform sampler2D u_GlossnessMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_ShadowMap;



uniform vec3 u_CameraPos;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


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
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


void main()
{
	//color = vec4(v_TexCoord.x, v_TexCoord.y, 0.0, 1.0);
   
    

	vec3 normal = texture(u_NormalMap, fs_in.v_TexCoord *  u_TilingFactor).rgb;
	vec3 albedo = texture(u_DiffuseMap, fs_in.v_TexCoord *  u_TilingFactor).rgb;
	float metallic = texture(u_GlossnessMap, fs_in.v_TexCoord *  u_TilingFactor).r;
	float smoothness = texture(u_SpecularMap, fs_in.v_TexCoord *  u_TilingFactor).r;
	float ao = texture(u_AoMap, fs_in.v_TexCoord *  u_TilingFactor).r;

	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fs_in.TBN * normal);

    float roughness = 1.0 - smoothness;

    // dummy light..
    vec3 lightPositions = vec3(0.3, 0.5, 0.2);


	vec3 N = normalize(normal); 
    vec3 V = normalize(u_CameraPos - fs_in.WorldPos);
    vec3 L = normalize(lightPositions);
    float attenuation = 4.5f;
    vec3 H = normalize(V + L);

    // cook-torrance brdf
    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);      


    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    metallic = 1.0 - metallic;
    kD *= 1.0 - metallic;   

    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo = (kD * albedo / PI + specular) * attenuation * NdotL; 


    vec3 ambient = vec3(0.1) * albedo * ao;
    vec3 totalColor = ambient + Lo;

    float Inshadow = ShadowCalculation(fs_in.FragPosLightSpace);
    totalColor *= (1.0 - Inshadow);
//    totalColor = totalColor / (totalColor + vec3(1.0));
//  color = pow(color, vec3(1.0/2.2));  

    color = vec4(totalColor, 0.0);

}

