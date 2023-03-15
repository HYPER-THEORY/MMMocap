#include "QuickPose.h"
#include "4DALoader.h"
#include "ShelfLoader.h"
#include "OneRoom.h"

#undef DEBUG
#include "External/Ink/Mainloop.h"

#define INK_SET_SHADER_PATH(p) Ink::ShaderCache::set_include_path(p "include/");\
							   Ink::ShaderLib::set_library_path(p "library/");

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;
constexpr int HIGH_DPI = 1;

constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH << HIGH_DPI;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT << HIGH_DPI;

constexpr float MAX_EPIPOLAR_DISTANCE = 0.1f;

const Ink::Vec3 RED = {10.4, 1, 1};
const Ink::Vec3 GREEN = {1, 3.8, 1};
const Ink::Vec3 YELLOW = {5.7, 2.4, 1};

QuickPose quickpose;
MultiViews multiviews;

MultiPersonPose computedMultiPersonPose;
MultiPersonPoses multiPersonPoses;
MultiPersonPoses multiPersonPosesGT;

std::vector<std::vector<bool> > peopleEvals;

void shelfToBody25(MultiPersonPoses& multiPersonPoses) {
	constexpr int jointMapping[] = {
		13, 12, 8, 7, 6, 9, 10, 11, 0, 2, 1, 0, 3, 4, 5,
	};
	for (auto& multiPersonPose : multiPersonPoses) {
		for (auto& pose : multiPersonPose) {
			if (pose.hasJoint.empty()) continue;
			Pose newPose;
			newPose.ID = pose.ID;
			newPose.hasJoint.resize(25);
			newPose.jointPos.resize(25);
			for (int type = 0; type < 15; ++type) {
				int mappingType = jointMapping[type];
				if (pose.hasJoint[mappingType]) {
					newPose.hasJoint[type] = true;
					newPose.jointPos[type] = pose.jointPos[mappingType];
				} else {
					newPose.hasJoint[type] = false;
				}
			}
			if (pose.hasJoint[2] && pose.hasJoint[3]) {
				newPose.hasJoint[8] = true;
				newPose.jointPos[8] = (pose.jointPos[2] + pose.jointPos[3]) * 0.5;
			} else {
				newPose.hasJoint[8] = false;
			}
			pose = newPose;
		}
	}
}

/* TODO: Be the real Body25 */
void skel19ToBody25(MultiPersonPoses& multiPersonPoses) {
	constexpr int jointMapping[] = {
		4, 1, 5, 11, 15, 6, 12, 16, 0, 2, 7, 13, 3, 8, 14,
	};
	for (auto& multiPersonPose : multiPersonPoses) {
		for (auto& pose : multiPersonPose) {
			if (pose.hasJoint.empty()) continue;
			Pose newPose;
			newPose.ID = pose.ID;
			newPose.hasJoint.resize(25);
			newPose.jointPos.resize(25);
			for (int type = 0; type < 15; ++type) {
				int mappingType = jointMapping[type];
				if (pose.hasJoint[mappingType]) {
					newPose.hasJoint[type] = true;
					newPose.jointPos[type] = pose.jointPos[mappingType];
				} else {
					newPose.hasJoint[type] = false;
				}
			}
			pose = newPose;
		}
	}
}

Ink::Vec3 mapping(const Ink::Vec3& pos) {
	return Ink::Vec3(pos.x, pos.z, pos.y) + Ink::Vec3(1, 0.1, 0);
}

void test() {
	MultiViews multiviews = T4DALoader::loadDataset("../Dataset/shelf");
	
	auto multiview = multiviews[100];
	multiview.computeEpipolar(MAX_EPIPOLAR_DISTANCE);
	
	Joint& joint1 = multiview.views[0].joints[1][1];
	Joint& joint2 = multiview.views[0].joints[8][3];
	Joint& joint3 = multiview.views[1].joints[8][0];
	
	/* PAF test: should be 0.995882 */
	std::cout << multiview.views[0].getPAF(joint1, joint2) << std::endl;
	
	/* Epipolar test: should be 0.806705 */
	std::cout << multiview.getEpipolar(joint2, joint3) << std::endl;
}

void prepare() {
	quickpose.initBody25();
	
	multiviews = T4DALoader::loadDataset("../Dataset/shelf");
	multiPersonPosesGT = T4DALoader::loadGroundTruth("../Dataset/shelf/gt.txt");
	
	float L01 = 0, L12 = 0, L23 = 0, L34 = 0, L15 = 0, L56 = 0, L67 = 0;
	float L18 = 0, L89 = 0, L910 = 0, L1011 = 0, L812 = 0, L1213 = 0, L1314 = 0;
	
	for (auto& multiPersonPose : multiPersonPosesGT) {
		for (auto& personPose : multiPersonPose) {
			L01 = fmax(L01, personPose.jointPos[13].distance(personPose.jointPos[12]));
			L12 = fmax(L12, personPose.jointPos[12].distance(personPose.jointPos[8]));
			L23 = fmax(L23, personPose.jointPos[8].distance(personPose.jointPos[7]));
			L34 = fmax(L34, personPose.jointPos[7].distance(personPose.jointPos[6]));
			L15 = fmax(L15, personPose.jointPos[12].distance(personPose.jointPos[9]));
			L56 = fmax(L56, personPose.jointPos[9].distance(personPose.jointPos[10]));
			L67 = fmax(L67, personPose.jointPos[10].distance(personPose.jointPos[11]));
			L18 = fmax(L18, personPose.jointPos[12].distance(personPose.jointPos[14]));
			L89 = fmax(L89, personPose.jointPos[14].distance(personPose.jointPos[2]));
			L910 = fmax(L910, personPose.jointPos[2].distance(personPose.jointPos[1]));
			L1011 = fmax(L1011, personPose.jointPos[1].distance(personPose.jointPos[0]));
			L812 = fmax(L812, personPose.jointPos[14].distance(personPose.jointPos[3]));
			L1213 = fmax(L1213, personPose.jointPos[3].distance(personPose.jointPos[4]));
			L1314 = fmax(L1314, personPose.jointPos[4].distance(personPose.jointPos[5]));
		}
	}
	
	quickpose.setMaxBoneLength(0,  1,  L01   + 0.1f); std::cout << "L01: " << L01 << std::endl;
	quickpose.setMaxBoneLength(1,  2,  L12   + 0.1f); std::cout << "L12: " << L12 << std::endl;
	quickpose.setMaxBoneLength(2,  3,  L23   + 0.1f); std::cout << "L23: " << L23 << std::endl;
	quickpose.setMaxBoneLength(3,  4,  L34   + 0.1f); std::cout << "L34: " << L34 << std::endl;
	quickpose.setMaxBoneLength(1,  5,  L15   + 0.1f); std::cout << "L15: " << L15 << std::endl;
	quickpose.setMaxBoneLength(5,  6,  L56   + 0.1f); std::cout << "L56: " << L56 << std::endl;
	quickpose.setMaxBoneLength(6,  7,  L67   + 0.1f); std::cout << "L67: " << L67 << std::endl;
	quickpose.setMaxBoneLength(1,  8,  L18   + 0.1f); std::cout << "L18: " << L18 << std::endl;
	quickpose.setMaxBoneLength(8,  9,  L89   + 0.1f); std::cout << "L89: " << L89 << std::endl;
	quickpose.setMaxBoneLength(9,  10, L910  + 0.1f); std::cout << "L910: " << L910 << std::endl;
	quickpose.setMaxBoneLength(10, 11, L1011 + 0.1f); std::cout << "L1011: " << L1011 << std::endl;
	quickpose.setMaxBoneLength(8,  12, L812  + 0.1f); std::cout << "L812: " << L812 << std::endl;
	quickpose.setMaxBoneLength(12, 13, L1213 + 0.1f); std::cout << "L1213: " << L1213 << std::endl;
	quickpose.setMaxBoneLength(13, 14, L1314 + 0.1f); std::cout << "L1314: " << L1314 << std::endl;
	
	multiPersonPosesGT = ShelfLoader::loadGroundTruth("../Dataset/shelf/shelf.gt.txt");
	shelfToBody25(multiPersonPosesGT);
	
//	multiPersonPoses = T4DALoader::loadGroundTruth("../Dataset/shelf/skel.txt");
//	skel19ToBody25(multiPersonPoses);
}

void execute(int frame) {
	auto& multiview = multiviews[frame];
	multiview.computeEpipolar(MAX_EPIPOLAR_DISTANCE);
	computedMultiPersonPose = quickpose.compute(multiview);
//	std::cout << "Count: " << quickpose.count << std::endl;
}

void evaluate(int frame) {
	static float accum = 0;
	static float total = 0;
	
	std::vector<int> boneA = {
		0, 1, 1, 1, 2, 3, 5, 6, 8, 8, 9, 10, 12, 13
	};
	std::vector<int> boneB = {
		1, 2, 5, 8, 3, 4, 6, 7, 9, 12, 10, 11, 13, 14
	};
	
	auto& multiPersonPoseGT = multiPersonPosesGT[frame + 300];
	auto& multiPersonPose = computedMultiPersonPose;
//	auto& multiPersonPose = multiPersonPoses[frame];
	
	OneRoom::cleanUpJointsAndBones();
	
	for (auto& pose : multiPersonPose) {
		if (pose.hasJoint.empty()) continue;
		for (int type = 0; type < 15; ++type) {
			if (!pose.hasJoint[type]) continue;
			OneRoom::setJoint(mapping(pose.jointPos[type]));
		}
		size_t boneSize = boneA.size();
		for (int bone = 0; bone < boneSize; ++bone) {
			if (!pose.hasJoint[boneA[bone]] || !pose.hasJoint[boneB[bone]]) continue;
			auto posA = mapping(pose.jointPos[boneA[bone]]);
			auto posB = mapping(pose.jointPos[boneB[bone]]);
			OneRoom::setBone(posA, posB, OneRoom::DEFAULT_COLOR);
		}
	}
	
	for (auto& poseGT : multiPersonPoseGT) {
		if (poseGT.hasJoint.empty()) continue;
		
		std::vector<bool> boneTests(boneA.size(), true);
		
		Pose* closestPose = nullptr;
		float minPoseDistance = std::numeric_limits<float>::max();
		for (auto& pose : multiPersonPose) {
			if (pose.hasJoint.empty()) continue;
			float poseDistance = 0;
			float poseNum = 0;
			for (int type = 0; type < 15; ++type) {
				if (pose.hasJoint[type] && poseGT.hasJoint[type]) {
					poseDistance += pose.jointPos[type].distance(poseGT.jointPos[type]);
					poseNum += 1.f;
				}
			}
			if (poseNum == 0) continue;
			poseDistance /= poseNum;
			if (poseDistance < minPoseDistance) {
				closestPose = &pose;
				minPoseDistance = poseDistance;
			}
		}
		
		if (closestPose != nullptr) {
			size_t boneSize = boneA.size();
			for (int bone = 0; bone < boneSize; ++bone) {
				if (!poseGT.hasJoint[boneA[bone]] || !poseGT.hasJoint[boneB[bone]]) continue;
				if (!closestPose->hasJoint[boneA[bone]] || !closestPose->hasJoint[boneB[bone]]) {
					boneTests[bone] = false;
					continue;
				}
				auto posA = closestPose->jointPos[boneA[bone]];
				auto posB = closestPose->jointPos[boneB[bone]];
				auto posGTA = poseGT.jointPos[boneA[bone]];
				auto posGTB = poseGT.jointPos[boneB[bone]];
				boneTests[bone] = posA.distance(posGTA) + posB.distance(posGTB) < posGTA.distance(posGTB);
			}
		}
		
		for (int type = 0; type < 15; ++type) {
			if (!poseGT.hasJoint[type]) continue;
			OneRoom::setJoint(mapping(poseGT.jointPos[type]));
		}
		size_t boneSize = boneA.size();
		for (int bone = 0; bone < boneSize; ++bone) {
			if (!poseGT.hasJoint[boneA[bone]] || !poseGT.hasJoint[boneB[bone]]) continue;
			auto posA = mapping(poseGT.jointPos[boneA[bone]]);
			auto posB = mapping(poseGT.jointPos[boneB[bone]]);
			if (bone == 8 || bone == 9 || bone == 1 || bone == 2) {
				OneRoom::setBone(posA, posB, YELLOW);
				continue;
			}
			OneRoom::setBone(posA, posB, boneTests[bone] ? GREEN : RED);
		}
		
		for (int bone = 0; bone < boneSize; ++bone) {
			if (bone == 8 || bone == 9 || bone == 1 || bone == 2 || bone == 0) continue;
			accum += boneTests[bone];
			total += 1.;
		}
	}
	
	if (total != 0.) {
		std::cout << "Frame: " << frame << std::string(4 - std::to_string(frame).size(), ' ');
		std::cout << "| PCP: " << accum / total << std::endl;
	}
}

void registerErrorCallback() {
	Ink::Error::set_callback([](const std::string& s) -> void {
		std::string message = Ink::Date::get_local().format() + ' ' + s + '\n';
		SDL_Window* sdlWindow = SDL_GL_GetCurrentWindow();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Runtime Error", message.c_str(), sdlWindow);
	});
}

void conf(Settings& t) {
	t.title = "QuickPose";
	t.width = WINDOW_WIDTH;
	t.height = WINDOW_HEIGHT;
	t.highdpi = HIGH_DPI;
	t.fps = 15;
	t.show_cursor = false;
	t.lock_cursor = true;
}

void load() {
	registerErrorCallback();
	
	INK_SET_SHADER_PATH("./External/ink/shaders/");
	
	OneRoom::prepareResources();
	OneRoom::setupPipeline(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	
	prepare();
}

void update(float dt) {
	static int frameIndex = 220;
	static bool needsUpdate = true;
	
	if (needsUpdate) {
		execute(frameIndex);
		evaluate(frameIndex);
		needsUpdate = false;
		Ink::Window::set_title("Frame: " + std::to_string(frameIndex));
	}
	
	OneRoom::update(dt);
	OneRoom::render();
	
	if (Ink::Window::is_down(SDLK_RIGHT) || Ink::Window::is_down(SDLK_SPACE)) {
		frameIndex = (frameIndex + 1) % 300;
		needsUpdate = true;
	}
	
	if (Ink::Window::is_down(SDLK_LEFT)) {
		frameIndex = (frameIndex + 299) % 300;
		needsUpdate = true;
	}
	
	if (Ink::Window::is_pressed(SDLK_1)) {
		OneRoom::setCamera(mapping(multiviews[0].views[1].camera->pos), {1, 0, 0});
	}
	
	if (Ink::Window::is_pressed(SDLK_2)) {
		OneRoom::setCamera(mapping(multiviews[0].views[2].camera->pos), {1, 0, 0});
	}
	
	if (Ink::Window::is_pressed(SDLK_3)) {
		OneRoom::setCamera(mapping(multiviews[0].views[3].camera->pos), {1, 0, 0});
	}
	
	if (Ink::Window::is_pressed(SDLK_4)) {
		OneRoom::setCamera(mapping(multiviews[0].views[4].camera->pos), {1, 0, 0});
	}
}

void quit() {}
