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

#pragma once

#include "External/Ink/Ink.h"

class Utils3D {
public:
	static Ink::Euler makeEuler(const Ink::Vec3& from, const Ink::Vec3& to);
};

class OneRoom {
public:
	static Ink::Vec3 DEFAULT_COLOR;
	
	static void prepareResources();
	
	static void setupPipeline(int width, int height);
	
	static void update(float deltaTime);
	
	static void render();
	
	static void setJoint(const Ink::Vec3& pos, const Ink::Vec3& color = DEFAULT_COLOR);
	
	static void setBone(const Ink::Vec3& posA, const Ink::Vec3& posB, const Ink::Vec3& color = DEFAULT_COLOR);
	
	static void cleanUpJointsAndBones();
	
	static void setCamera(const Ink::Vec3& pos, const Ink::Vec3& dir);
	
private:
	static int jointNumber;
	
	static int boneNumber;
	
	static Ink::Gpu::Rect viewport;
	
	static Ink::Scene scene;
	
	static Ink::Viewer viewer;
	
	static Ink::Renderer renderer;
	
	static Ink::BloomPass bloomPass;
	
	static Ink::ToneMapPass toneMapPass;
	
	static Ink::FXAAPass fxaaPass;
	
	static Ink::CopyPass copyPass;
	
	static std::unordered_map<std::string, Ink::Mesh> meshes;
	
	static std::unordered_map<std::string, Ink::Image> images;
	
	static std::unordered_map<std::string, Ink::Material> materials;
	
	static std::unordered_map<std::string, Ink::ReflectionProbe> probes;
	
	static std::unordered_map<std::string, Ink::Instance*> instances;
	
	static std::unordered_map<std::string, Ink::Gpu::Texture> maps;
	
	static std::unordered_map<std::string, Ink::Gpu::RenderTarget> targets;
};
