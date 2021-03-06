#version 410
#extension GL_EXT_gpu_shader4 : enable

// This code is based on code from here https://learnopengl.com/#!PBR/Lighting
layout (location = 0) out vec4 fragColour;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D ColourTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D NormalTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D AOTexture;
uniform samplerCube skybox;
const int envMaxLOD = 8;

uniform int tiling;

// camera parameters
uniform vec3 camPos;

// lights
const float scale = 10.0f;
const float height = 4.0f;
const vec3 lightPositions[4] = vec3[4](
      vec3(0, 0, 10),
      vec3( 0, 10, 0),
      vec3(-scale, height,  scale),
      vec3( scale, height,  scale)
      );

const float intensity = 100.f;
const vec3 lightColors[4] = vec3[4](
      vec3(intensity, intensity, intensity),
      vec3(intensity, intensity, intensity),
      vec3(intensity, intensity, intensity),
      vec3(intensity, intensity, intensity)
      );

// Define pi
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float nom   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

//----------------------------------------------------------------------------

//vec3 fresnelSchlick(float cosTheta, vec3 F0)
//{
//	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
//}

// ----------------------------------------------------------------------------

vec3 fresnelSchlickCommon(float cosTheta, vec3 f0, vec3 base)
{
  return f0 + (base - f0) * pow(1.0 - cosTheta, 5.0);

}

// ----------------------------------------------------------------------------

vec3 fresnelSchlick( float cosTheta, vec3 f0 )
{
  return fresnelSchlickCommon(cosTheta, f0, vec3(1.0));
}

// ----------------------------------------------------------------------------

vec3 fresnelSchlickRoughness(float cosTheta, vec3 f0, float roughness)
{
  return fresnelSchlickCommon(cosTheta, f0, max(vec3(1.0 - roughness), f0));
}

// ----------------------------------------------------------------------------

vec3 diffuseTerm(vec3 f, float metallic)
{
  return (1.0 - f) * (1.0 - metallic);
}

// ----------------------------------------------------------------------------

/** From http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/ */
mat4 rotationMatrix( vec3 axis, float angle )
{
  //axis = normalize(axis);
  float s = sin( angle );
  float c = cos( angle );
  float oc = 1.0 - c;
  return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
              oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
              oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
              0.0,                                0.0,                                0.0,                                1.0 );
}

// ----------------------------------------------------------------------------

vec3 rotateVector( vec3 src, vec3 tgt, vec3 vec )
{
  float angle = acos( dot( src, tgt ) );

  if ( angle == 0 )
  {
    return vec;
  }
  vec3 axis = normalize( cross( src, tgt ) );
  mat4 R = rotationMatrix( axis, angle );
  vec4 norm = R * vec4( vec, 1.0f );
  return norm.xyz / norm.w;
}

// ----------------------------------------------------------------------------

void main()
{
	vec2 texCoords = TexCoords * tiling;
  vec3 N = normalize( Normal );
  vec3 V = normalize( camPos - WorldPos );
  vec3 R = reflect( -V, N );

  // normal texture
	vec3 tgt = normalize( texture( NormalTexture, texCoords ).rgb * 2.0 - 1.0 );
  vec3 src = vec3( 0.0, 0.0, 1.0 );
  N = rotateVector( src, tgt, N);

  // other textures
	vec3 albedo = texture( ColourTexture, texCoords ).rgb;
	float metallic = texture( MetallicTexture, texCoords ).r;
	float roughness = texture( RoughnessTexture, texCoords ).r;

  if(roughness < 0.001f) roughness = 0.001f;
  if(roughness > 0.999f) roughness = 0.999f;
  roughness = 1.0f - roughness;


  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)
  vec3 F0 = vec3( 0.04 );
  F0 = mix( F0, albedo, metallic );

  // reflectance equation
  vec3 Lo = vec3(0.0);
  for( int i = 0; i < 4; ++i )
  {
    vec3 trans = vec3( 0.0, 0.0, -2.0 );
    vec3 ray = lightPositions[i] - WorldPos + trans;
    // calculate per-light radiance
    vec3 L = normalize(ray);
    vec3 H = normalize(V + L);
    float dist = length(ray);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = lightColors[i] * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 brdf = nominator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + brdf) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
  }

  // cubemap reflection
//  vec3 lookup = reflect( -V, N ); // R
//  float gloss = ( 1 - texture(RoughnessTexture, TexCoords).r ) * 16;
//  vec3 reflectionColor = textureLod( skybox, lookup, gloss).rgb;
  // cubemap reflection


  // irradiance
//  vec3 f = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
  //	vec3 irradiance = texture( u_irradianceMap, n ).rgb;
//  vec3 diffuse = diffuseTerm( f, metallic ); // * ( irradiance * eyeAlbedo );

  //	const float MAX_REFLECTION_LOD = 4.0;
  //  vec3 prefilteredColor = textureLod(u_prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb; // reflectionColor
  //	vec2 envBRDF  = texture(u_brdfMap, vec2( max( dot(N, V), 0.0), roughness )).rg;
  //	vec3 specular = prefilteredColor * ( f * envBRDF.x + envBRDF.y );
  // irradiance

  // ambient lighting (note that the next IBL tutorial will replace
  // this ambient lighting with environment lighting).

	float ao = texture( AOTexture, texCoords ).r;
  vec3 ambient = vec3( 0.03 ) * albedo * ao;
  vec3 color = ambient + Lo;

//  vec3 reflection = reflectionColor * roughness;
  // HDR tonemapping
  color = color / ( color + vec3( 1.0 ) );
  // gamma correct
  color = pow( color, vec3( 1.0 / 2.2 ) );


  //  fragColour = vec4( mix( x, color, roughness ), 1 );
    fragColour = vec4( color, 1.0f );
//    fragColour = texture(NormalTexture, texCoords);
//    fragColour = vec4(Normal, 1.0);
}

