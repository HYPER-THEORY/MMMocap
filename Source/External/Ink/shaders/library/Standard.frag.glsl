#include <common>
#include <packing>
#include <brdf>
#include <iblfilter>

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model_view;
uniform mat4 model_view_proj;
uniform mat3 normal_mat;
uniform vec3 camera_pos;

#ifdef FORWARD_RENDERING
#include <lightprocess>
#endif

uniform float alpha_test;

#ifdef USE_NORMAL_MAP
uniform sampler2D normal_map;
uniform float normal_scale;
#endif

uniform vec3 color;

#if defined(USE_COLOR_MAP) || defined(USE_COLOR_ALPHA_MAP)
uniform sampler2D color_map;
#endif

uniform float alpha;

#ifdef USE_ALPHA_MAP
uniform sampler2D alpha_map;
#endif

uniform vec3 emissive;

#ifdef USE_EMISSIVE_MAP
uniform sampler2D emissive_map;
#endif

#ifdef USE_AO_MAP
uniform sampler2D ao_map;
uniform float ao_intensity;
#endif

uniform float metalness;

#ifdef USE_METALNESS_MAP
uniform sampler2D metalness_map;
#endif

uniform float roughness;

#ifdef USE_ROUGHNESS_MAP
uniform sampler2D roughness_map;
#endif

uniform float specular;

#ifdef USE_SPECULAR_MAP
uniform sampler2D specular_map;
#endif

#ifdef USE_REFLECTION_PROBE
uniform samplerCube ref_map;
uniform float ref_lod;
uniform float ref_intensity;
#endif

in vec3 v_normal;
in vec2 v_uv;
in vec3 v_world_pos;

#ifdef USE_TANGENT_SPACE
in vec3 v_tangent;
in vec3 v_bitangent;
#endif

#ifdef USE_VERTEX_COLOR
in vec3 v_color;
#endif

#ifdef FORWARD_RENDERING
layout(location = 0) out vec4 out_color;
#endif

#ifdef DEFERRED_RENDERING
layout(location = 0) out vec4 g_color;       /* G-Buffer base color */
layout(location = 1) out vec4 g_normal;      /* G-Buffer world normal */
layout(location = 2) out vec4 g_material;    /* G-Buffer material data */
layout(location = 3) out vec4 g_light;       /* G-Buffer indirect light */
#endif

void main() {
	/* calculate color and alpha */
	vec4 t_color = vec4(color, alpha);
	#ifdef USE_VERTEX_COLOR
		t_color.xyz *= v_color;
	#endif
	#ifdef USE_COLOR_MAP
		t_color.xyz *= texture(color_map, v_uv).xyz;
	#endif
	#ifdef USE_ALPHA_MAP
		t_color.w *= texture(alpha_map, v_uv).x;
	#endif
	#ifdef USE_COLOR_ALPHA_MAP
		t_color.xyzw *= texture(color_map, v_uv).xyzw;
	#endif
	
	/* discard if failing alpha test */
	if (t_color.w < alpha_test) discard;
	
	/* calculate normal in world space */
	float face_dir = gl_FrontFacing ? 1. : -1.;
	vec3 t_normal = normalize(v_normal) * face_dir;
	#ifdef USE_NORMAL_MAP
		vec3 normal = texture(normal_map, v_uv).xyz;
		normal = normalize(unpack_normal(normal));
		normal.xy *= normal_scale;
		#ifdef USE_TANGENT_SPACE
			vec3 tangent = normalize(v_tangent) * face_dir;
			vec3 bitangent = normalize(v_bitangent) * face_dir;
			mat3 tbn_mat = mat3(tangent, bitangent, t_normal);
			t_normal = normalize(tbn_mat * normal);
		#endif
		#ifdef USE_OBJECT_SPACE
			t_normal = normalize(normal_mat * normal);
		#endif
	#endif
	
	/* calculate metalness */
	float t_metalness = metalness;
	#ifdef USE_METALNESS_MAP
		t_metalness *= texture(metalness_map, v_uv).x;
	#endif
	
	/* calculate roughness */
	float t_roughness = roughness;
	#ifdef USE_METALNESS_MAP
		t_roughness *= texture(roughness_map, v_uv).x;
	#endif
	
	/* calculate specular IOR */
	float t_specular = specular;
	#ifdef USE_SPECULAR_MAP
		t_specular *= texture(specular_map, v_uv).x;
	#endif
	
	/* calculate emissive color */
	vec3 t_emissive = emissive;
	#ifdef USE_EMISSIVE_MAP
		t_emissive *= texture(emissive_map, v_uv).xyz;
	#endif
	
	/* calculate ambient occlusion */
	float t_occlusion = 1.;
	#ifdef USE_AO_MAP
		t_occlusion *= (texture(ao_map, v_uv).x - 1.) * ao_intensity + 1.;
	#endif
	
	/* calculate diffuse color */
	vec3 diffuse = t_color.xyz * (1 - t_metalness);
	
	/* calculate specular F0 */
	vec3 specular_f0 = mix(vec3(t_specular * 0.08), t_color.xyz, t_metalness);
	
	/* view from position to camera position */
	vec3 view_dir = normalize(camera_pos - v_world_pos);
	
	/* calculate indirect light */
	vec3 indirect_light = t_emissive;
	
	#ifdef USE_REFLECTION_PROBE
		/* calculate single-scatter and multi-scatter */
		vec3 single_scatter = vec3(0.);
		vec3 multi_scatter = vec3(0.);
		scattering(t_normal, view_dir, specular_f0, t_roughness, single_scatter, multi_scatter);
		
		/* calculate irradiance and radiance from reflection map */
		vec3 irradiance = ibl_diffuse(ref_map, ref_lod, t_normal) * INV_PI * ref_intensity;
		vec3 radiance = ibl_specular(ref_map, ref_lod, view_dir, t_normal, t_roughness) * ref_intensity;
		
		/* calculate indirect specular light */
		indirect_light += single_scatter * radiance;
		indirect_light += multi_scatter * irradiance;
		
		/* calculate indirect diffuse light */
		indirect_light += diffuse * (1 - single_scatter - multi_scatter) * irradiance * t_occlusion;
	#endif
	
	#ifdef DEFERRED_RENDERING
		/* output G-Buffers in deferred rendering */
		g_color = vec4(diffuse, t_occlusion);
		g_normal = vec4(pack_normal(t_normal), 0.);
		g_material = vec4(specular_f0, t_roughness);
		g_light = vec4(indirect_light, 0.);
	#endif
	
	#ifdef FORWARD_RENDERING
		/* create new material object */
		Material material;
		material.color = diffuse;
		material.f0 = specular_f0;
		material.roughness = t_roughness;
		
		/* create new geometry object */
		Geometry geometry;
		geometry.position = v_world_pos;
		geometry.view_dir = view_dir;
		geometry.normal = t_normal;
		
		/* output color in forward rendering */
		out_color = vec4(light_process(material, geometry, indirect_light, t_occlusion), t_color.w);
	#endif
}
