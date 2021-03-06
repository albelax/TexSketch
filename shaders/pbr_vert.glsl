#version 410
#extension GL_EXT_gpu_shader4 : enable

// this demo is based on code from here https://learnopengl.com/#!PBR/Lighting
/// @brief the vertex passed in
layout (location = 0) in vec3 VertexPosition;
/// @brief the normal passed in
layout (location = 2) in vec3 VertexNormal;
/// @brief the in uv
layout (location = 1) in vec2 TexCoord;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec3 colNorm;

uniform mat4 MVP;
uniform mat3 N;
uniform mat4 MV;

void main()
{
  gl_Position = MVP * vec4(VertexPosition, 1.0);
  WorldPos = vec3(MV * vec4(VertexPosition, 1.0));
  Normal = N * VertexNormal;
  TexCoords = TexCoord;
  colNorm = VertexNormal;
}
