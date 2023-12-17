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
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{		
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    float nrSamples = 0.0f;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}