/**
 * Copyright (C) 2022-2023 Hypertheory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "OneRoom.h"

#define PATH_S "../Assets/standard/"
#define PATH_G "../Assets/vr-exhibition-gallery-baked/"

Ink::Euler Utils3D::makeEuler(const Ink::Vec3& from, const Ink::Vec3& to) {
	Ink::Vec4 quat;
	float r = from.dot(to) + 1;
	if (r < 0.0001) {
		r = 0;
		if (fabsf(from.x) > fabsf(from.z)) {
			quat = {-from.y, from.x, 0, r};
		} else {
			quat = {0, -from.z, from.y, r};
		}
	} else {
		quat = {
			from.y * to.z - from.z * to.y,
			from.z * to.x - from.x * to.z,
			from.x * to.y - from.y * to.x,
			r,
		};
	}
	quat = quat.normalize();
	
	float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;
	float m11 = 1 - (yy + zz);
	float m12 = xy - wz;
	float m22 = 1 - (xx + zz);
	float m32 = yz + wx;
	float m13 = xz + wy;
	float m23 = yz - wx;
	float m33 = 1 - (xx + yy);
	
	Ink::Euler euler;
	euler.y = asinf(m13 < -1 ? -1 : m13 > 1 ? 1 : m13);
	if (fabsf(m13) < 0.9999) {
		euler.x = atan2f(-m23, m33);
		euler.z = atan2f(-m12, m11);
	} else {
		euler.x = atan2f(m32, m22);
		euler.z = 0;
	}
	return euler;
}

void OneRoom::prepareResources() {
	probes["Ground"] = Ink::ReflectionProbe(1.5, 1024);
	
	meshes["Sphere"] = Ink::Loader::load_obj(PATH_S "Sphere.obj").meshes[0];
	meshes["Sphere"].create_tangents();
	
	meshes["Cylinder"] = Ink::Loader::load_obj(PATH_S "Cylinder.obj").meshes[0];
	meshes["Cylinder"].create_tangents();
	
	meshes["Gallery"] = Ink::Loader::load_obj(PATH_G "source/2.obj").meshes[0];
	meshes["Gallery"].create_tangents();
	
	images["WallL2"] = Ink::Loader::load_image(PATH_G "textures/wall_l2.png");
	images["WallL2"].flip_vertical();
	
	images["WallR2"] = Ink::Loader::load_image(PATH_G "textures/wall_r2.png");
	images["WallR2"].flip_vertical();
	
	images["WallBack2"] = Ink::Loader::load_image(PATH_G "textures/wall_back2.png");
	images["WallBack2"].flip_vertical();
	
	materials["Material.001"] = Ink::Material();
	materials["Material.001"].color = {0.560, 0.570, 0.580};
	materials["Material.001"].roughness = 0.10;
	materials["Material.001"].metalness = 0.25;
	materials["Material.001"].reflection_probe = &probes["Ground"];
	
	materials["Material.002"] = Ink::Material();
	materials["Material.002"].visible = false;
	
	materials["Material.003"] = Ink::Material();
	materials["Material.003"].emissive_intensity = 1.2;
	materials["Material.003"].emissive = {1, 1, 1};
	materials["Material.003"].emissive_map = &images["WallR2"];
	
	materials["Material.004"] = Ink::Material();
	materials["Material.004"].emissive_intensity = 1.2;
	materials["Material.004"].emissive = {1, 1, 1};
	materials["Material.004"].emissive_map = &images["WallL2"];
	
	materials["Material.006"] = Ink::Material();
	materials["Material.006"].emissive_intensity = 1.2;
	materials["Material.006"].emissive = {1, 1, 1};
	materials["Material.006"].emissive_map = &images["WallBack2"];
	
	materials["Material.005"] = Ink::Material();
	materials["Material.005"].side = Ink::DOUBLE_SIDE;
	materials["Material.005"].emissive = {1, 1, 1};
	
	materials["Light.001"] = Ink::Material();
	materials["Light.001"].side = Ink::DOUBLE_SIDE;
	materials["Light.001"].emissive = {2, 2, 2};
	
	instances["Gallery"] = Ink::Instance();
	instances["Gallery"].mesh = &meshes["Gallery"];
	instances["Gallery"].scale = {0.2, 0.2, 0.2};
	
	if (!enableRealtimeReflection) {
		materials["Material.001"].emissive = {0.560, 0.570, 0.580};
		materials["Material.001"].reflection_probe = nullptr;
	}
	
	scene.add(&instances["Gallery"]);
	
	scene.set_material("Material.001", &materials["Material.001"]);
	scene.set_material("Material.002", &materials["Material.002"]);
	scene.set_material("Material.003", &materials["Material.003"]);
	scene.set_material("Material.004", &materials["Material.004"]);
	scene.set_material("Material.006", &materials["Material.006"]);
	scene.set_material("Material.005", &materials["Material.005"]);
	scene.set_material("light.001", &materials["Light.001"]);
	
	renderer.load_scene(scene);
	renderer.load_mesh(meshes["Sphere"]);
	renderer.load_mesh(meshes["Cylinder"]);
}

void OneRoom::setupPipeline(int width, int height) {
	viewport = Ink::Gpu::Rect(width, height);
	
	int width2 = viewport.width * 2;
	int height2 = viewport.height * 2;
	
	maps["DepthMap"].init_2d(width2, height2, Ink::TEXTURE_D24_UNORM);
	maps["DepthMap"].set_filters(Ink::TEXTURE_LINEAR, Ink::TEXTURE_LINEAR);
	
	maps["PostMap0"].init_2d(width2, height2, Ink::TEXTURE_R16G16B16_SFLOAT);
	maps["PostMap0"].set_filters(Ink::TEXTURE_LINEAR, Ink::TEXTURE_LINEAR);
	
	maps["PostMap1"].init_2d(width2, height2, Ink::TEXTURE_R16G16B16_SFLOAT);
	maps["PostMap1"].set_filters(Ink::TEXTURE_LINEAR, Ink::TEXTURE_LINEAR);
	
	targets["BaseTarget"].set_texture(maps["PostMap0"], 0);
	targets["BaseTarget"].set_depth_texture(maps["DepthMap"]);
	
	targets["PostTarget0"].set_texture(maps["PostMap0"], 0);
	
	targets["PostTarget1"].set_texture(maps["PostMap1"], 0);
	
	renderer.set_rendering_mode(Ink::FORWARD_RENDERING);
	renderer.set_clear_color({0.5, 0.5, 0.5, 1});
	renderer.set_viewport(Ink::Gpu::Rect(width2, height2));
	
	viewer = Ink::Viewer(new Ink::PerspCamera(75 * Ink::DEG_TO_RAD, 1.77, 0.05, 500));
	viewer.set_position({4, 1, 0});
	viewer.set_direction({1, 0, 0});
	
	bloomPass = Ink::BloomPass(width2, height2);
	bloomPass.init();
	bloomPass.threshold = 1.0;
	bloomPass.radius = 0.5;
	bloomPass.intensity = 0.5;
	
	toneMapPass = Ink::ToneMapPass();
	toneMapPass.init();
	toneMapPass.exposure = 1.0;
	toneMapPass.mode = Ink::ACES_FILMIC_TONE_MAP;
	
	copyPass = Ink::CopyPass();
	copyPass.init();
	
	fxaaPass = Ink::FXAAPass();
	fxaaPass.init();
	
	renderer.set_target(&targets["BaseTarget"]);
	
	bloomPass.set_texture(&maps["PostMap0"]);
	bloomPass.set_target(&targets["PostTarget1"]);
	
	toneMapPass.set_texture(&maps["PostMap1"]);
	toneMapPass.set_target(&targets["PostTarget0"]);
	
	fxaaPass.set_texture(&maps["PostMap0"]);
	fxaaPass.set_target(&targets["PostTarget1"]);
	
	copyPass.set_texture(&maps["PostMap1"]);
}

void OneRoom::update(float deltaTime) {
	if (Ink::Window::is_pressed(1) && viewer.speed == 0) {
		viewer.speed = 1;
		viewer.sensitivity = 0.001;
		Ink::Window::set_cursor_locked(true);
		Ink::Window::set_cursor_visible(false);
		
		int width = Ink::Window::get_size().first;
		int height = Ink::Window::get_size().second;
		Ink::Window::set_cursor_position(width / 2, height / 2);
	}
	
	if (Ink::Window::is_pressed(SDLK_ESCAPE)) {
		viewer.speed = 0;
		viewer.sensitivity = 0;
		Ink::Window::set_cursor_locked(false);
		Ink::Window::set_cursor_visible(true);
	}
	
	viewer.update(deltaTime);
	
	probes["Ground"].position = viewer.get_camera()->position;
	probes["Ground"].position.y = -probes["Ground"].position.y;
	
	Ink::Renderer::update_scene(scene);
}

void OneRoom::render(const Ink::Camera* camera) {
	Ink::Gpu::Rect viewport2;
	viewport2.width = viewport.width * 2;
	viewport2.height = viewport.height * 2;
	
	if (enableRealtimeReflection) {
		materials["Material.001"].visible = false;
		renderer.set_tone_map(Ink::LINEAR_TONE_MAP, 0.7);
		renderer.update_probe(scene, probes["Ground"]);
		materials["Material.001"].visible = true;
		renderer.set_tone_map(Ink::LINEAR_TONE_MAP, 1);
	}
	
	Ink::RenderPass::set_viewport(viewport2);
	
	auto* availableCamera = camera;
	if (availableCamera == nullptr) {
		availableCamera = viewer.get_camera();
	}
	
	renderer.clear();
	renderer.render(scene, *availableCamera);
	bloomPass.render();
	toneMapPass.render();
	fxaaPass.render();
	
	Ink::RenderPass::set_viewport(viewport);
	
	copyPass.render();
}

void OneRoom::setJoint(const Ink::Vec3& pos, const Ink::Vec3& color) {
	std::string colorName = color.to_string(3);
	materials[colorName].emissive = color;
	
	std::string instanceName = "Joint" + std::to_string(jointNumber++);
	if (instances.count(instanceName) == 0) {
		instances[instanceName] = Ink::Instance();
		instances[instanceName].mesh = &meshes["Sphere"];
		instances[instanceName].scale = {0.03, 0.03, 0.03};
		scene.add(&instances[instanceName]);
	}
	instances[instanceName].visible = true;
	instances[instanceName].position = pos;
	
	scene.set_material("default", instances[instanceName], &materials[colorName]);
}

void OneRoom::setBone(const Ink::Vec3& pos1, const Ink::Vec3& pos2, const Ink::Vec3& color) {
	std::string colorName = color.to_string(3);
	materials[colorName].emissive = color;
	
	std::string instanceName = "Bone" + std::to_string(boneNumber++);
	if (instances.count(instanceName) == 0) {
		instances[instanceName] = Ink::Instance();
		instances[instanceName].mesh = &meshes["Cylinder"];
		instances[instanceName].scale = {0.02, 1.00, 0.02};
		scene.add(&instances[instanceName]);
	}
	Ink::Vec3 direction = pos1 - pos2;
	Ink::Euler rotation = Utils3D::makeEuler({0, 1, 0}, direction.normalize());
	instances[instanceName].visible = true;
	instances[instanceName].position = (pos1 + pos2) * 0.5;
	instances[instanceName].rotation = rotation;
	instances[instanceName].scale.y = direction.magnitude();
	
	scene.set_material("default", instances[instanceName], &materials[colorName]);
}

void OneRoom::cleanUpJointsAndBones() {
	for (int i = 0; i < jointNumber; ++i) {
		instances["Joint" + std::to_string(i)].visible = false;
	}
	jointNumber = 0;
	for (int i = 0; i < boneNumber; ++i) {
		instances["Bone" + std::to_string(i)].visible = false;
	}
	boneNumber = 0;
}

void OneRoom::setCamera(const Ink::Vec3& pos, const Ink::Vec3& dir) {
	viewer.set_position(pos);
	viewer.set_direction(dir);
}

void OneRoom::setRealtimeReflection(bool enable) {
	enableRealtimeReflection = enable;
}

const Ink::Vec3 OneRoom::DEFAULT_COLOR = Ink::ColorUtils::to_rgb(0x16193B);

bool OneRoom::enableRealtimeReflection = true;

int OneRoom::jointNumber = 0;

int OneRoom::boneNumber = 0;

Ink::Gpu::Rect OneRoom::viewport;

Ink::Scene OneRoom::scene;

Ink::Viewer OneRoom::viewer;

Ink::Renderer OneRoom::renderer;

Ink::BloomPass OneRoom::bloomPass;

Ink::ToneMapPass OneRoom::toneMapPass;

Ink::FXAAPass OneRoom::fxaaPass;

Ink::CopyPass OneRoom::copyPass;

std::unordered_map<std::string, Ink::Mesh> OneRoom::meshes;

std::unordered_map<std::string, Ink::Image> OneRoom::images;

std::unordered_map<std::string, Ink::Material> OneRoom::materials;

std::unordered_map<std::string, Ink::ReflectionProbe> OneRoom::probes;

std::unordered_map<std::string, Ink::Instance> OneRoom::instances;

std::unordered_map<std::string, Ink::Gpu::Texture> OneRoom::maps;

std::unordered_map<std::string, Ink::Gpu::RenderTarget> OneRoom::targets;
