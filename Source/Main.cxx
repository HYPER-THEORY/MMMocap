#include "QuickPose.h"
#include "4DALoader.h"
#include "ShelfLoader.h"
#include "OneRoom.h"
#include "Visualizer2D.h"
#include "MathUtils.h"

#include "fmt/format.h"

#undef DEBUG
#include "ink/Mainloop.h"

#include <fstream>

#define INK_SET_SHADER_PATH(p) Ink::ShaderCache::set_include_path(p "include/");\
							   Ink::ShaderLib::set_library_path(p "library/");

#define DATASET_DIR "/Volumes/Elements/DataSet/Shelf"

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;
constexpr int HIGH_DPI = 1;

constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH << HIGH_DPI;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT << HIGH_DPI;

constexpr float MAX_EPIPOLAR_DISTANCE = 0.1f;

const Ink::Vec3 RED = {10.4, 1, 1};
const Ink::Vec3 GREEN = {1, 3.8, 1};
const Ink::Vec3 WHITE = {2, 2, 2};

QuickPose quickpose;
MultiViews multiviews;

MultiPersonPose computedMultiPersonPose;
MultiPersonPoses multiPersonPoses4DA;
MultiPersonPoses multiPersonPosesGT;

/* Incorrect conversion */
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

void correctShelfAtBody25(MultiPersonPose& multiPersonPose) {
	for (auto& pose : multiPersonPose) {
		Ink::Vec3 faceDir = (pose.jointPos[1] - pose.jointPos[8])
			.cross(pose.jointPos[2] - pose.jointPos[5]).normalize();
		Ink::Vec3 zDir = {0, 0, 1};
		Ink::Vec3 shoulderCenter = (pose.jointPos[2] + pose.jointPos[5]) * 0.5f;
		Ink::Vec3 headCenter = (pose.jointPos[17] + pose.jointPos[18]) * 0.5f;
		
		if (pose.hasJoint[17] ^ pose.hasJoint[18]) {
			Ink::Vec3 ear;
			if (pose.hasJoint[17]) ear = pose.jointPos[17];
			if (pose.hasJoint[18]) ear = pose.jointPos[18];
			Ink::Vec3 v1 = pose.jointPos[0] - shoulderCenter;
			Ink::Vec3 v2 = {0, 0, 1};
			Ink::Vec3 vn = v1.cross(v2).normalize();
			headCenter = ear - (ear - shoulderCenter).dot(vn) * vn;
		}
		
		if (pose.hasJoint[2] && pose.hasJoint[5]) {
			pose.jointPos[1] = shoulderCenter + (headCenter - shoulderCenter) * 0.5f;
			pose.jointPos[0] = pose.jointPos[1] + faceDir * 0.125f + zDir * 0.145f;
		}
		
//		std::cout << (headCenter - shoulderCenter).normalize().dot({0, 0, 1}) << std::endl;
		
//		Ink::Vec3 coco0 = pose.jointPos[0];
//		Ink::Vec3 neck = (pose.jointPos[5] + pose.jointPos[2]) / 2;
//		Ink::Vec3 head_bottom = (neck + pose.jointPos[0]) / 2;
//		Ink::Vec3 head_center = (pose.jointPos[17] + pose.jointPos[18]) / 2;
//		Ink::Vec3 head_top = head_bottom + (head_center - head_bottom) * 2;
//
//		pose.jointPos[1] = (pose.jointPos[2] + pose.jointPos[5]) / 2;
//		pose.jointPos[0] = pose.jointPos[1] + (coco0 - pose.jointPos[1]) * Ink::Vec3(0.78, 0.5, 1.5);
//		pose.jointPos[1] = pose.jointPos[1] + (coco0 - pose.jointPos[1]) * Ink::Vec3(0.3, 0.4, 0.6);
	}
}

void skel19ToBody25(MultiPersonPoses& multiPersonPoses) {
	constexpr int jointMapping[] = {
		4, 1, 5, 11, 15, 6, 12, 16, 0, 2, 7, 13, 3, 8, 14, -1, -1, 9, 10, 17, -1, -1, 18, -1, -1
	};
	for (auto& multiPersonPose : multiPersonPoses) {
		for (auto& pose : multiPersonPose) {
			if (pose.hasJoint.empty()) continue;
			Pose newPose;
			newPose.ID = pose.ID;
			newPose.hasJoint.resize(25);
			newPose.jointPos.resize(25);
			for (int type = 0; type < 25; ++type) {
				int mappingType = jointMapping[type];
				if (mappingType != -1 && pose.hasJoint[mappingType]) {
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

void coco17ToBody25(MultiPersonPoses& multiPersonPoses) {
	constexpr int jointMapping[] = {
		0, -1, 6, 8, 10, 5, 7, 9, -1, 12, 14, 16, 11, 13, 15, 1, 2, 3, 4, -1, -1, -1, -1, -1, -1
	};
	for (auto& multiPersonPose : multiPersonPoses) {
		for (auto& pose : multiPersonPose) {
			if (pose.hasJoint.empty()) continue;
			Pose newPose;
			newPose.ID = pose.ID;
			newPose.hasJoint.resize(25);
			newPose.jointPos.resize(25);
			for (int type = 0; type < 25; ++type) {
				int mappingType = jointMapping[type];
				if (mappingType != -1 && pose.hasJoint[mappingType]) {
					newPose.hasJoint[type] = true;
					newPose.jointPos[type] = pose.jointPos[mappingType];
				} else {
					newPose.hasJoint[type] = false;
				}
			}
			if (newPose.hasJoint[2] && newPose.hasJoint[5]) {
				newPose.hasJoint[1] = true;
				newPose.jointPos[1] = (newPose.jointPos[2] + newPose.jointPos[5]) * 0.5;
			}
			if (newPose.hasJoint[9] && newPose.hasJoint[12]) {
				newPose.hasJoint[8] = true;
				newPose.jointPos[8] = (newPose.jointPos[9] + newPose.jointPos[12]) * 0.5;
			}
			pose = newPose;
		}
	}
}

Ink::Vec3 mapping(const Ink::Vec3& pos) {
	return Ink::Vec3(pos.x, pos.z, -pos.y) + Ink::Vec3(1, 0.1, 0);
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
	
	/* Ray Intersect test: should be 1.6 */
	const Ink::Ray* rays[2];
	rays[0] = new Ink::Ray({0, 0, 0}, {0, 1, 0});
	rays[1] = new Ink::Ray({2, 0, 0}, {0, 0, 1});
	float confs[2] = {0.5, 2};
	std::cout << MathUtils::multiRayIntersect(rays, confs, 2).to_string() << std::endl;
}

void prepare() {
	quickpose.initBody25();
	
	multiviews = T4DALoader::loadDataset("../Dataset/shelf");
	multiPersonPosesGT = T4DALoader::loadGroundTruth("../Dataset/shelf/gt.txt");
	
//	for (int i = 0; i < 5; ++i) {
//		std::cout << multiviews[0].views[i].camera->R.to_string(6);
//		std::cout << multiviews[0].views[i].camera->t.to_string(6) << std::endl;
//		std::cout << multiviews[0].views[i].camera->K.to_string(6) << std::endl;
//	}
	
//	float L01 = 0, L12 = 0, L23 = 0, L34 = 0, L15 = 0, L56 = 0, L67 = 0;
//	float L18 = 0, L89 = 0, L910 = 0, L1011 = 0, L812 = 0, L1213 = 0, L1314 = 0;
//	float L217 = 0, L518 = 0;
//
//	for (auto& multiPersonPose : multiPersonPosesGT) {
//		for (auto& personPose : multiPersonPose) {
//			L01 = fmax(L01, personPose.jointPos[4].distance(personPose.jointPos[1]));
//			L12 = fmax(L12, personPose.jointPos[1].distance(personPose.jointPos[5]));
//			L23 = fmax(L23, personPose.jointPos[5].distance(personPose.jointPos[11]));
//			L34 = fmax(L34, personPose.jointPos[11].distance(personPose.jointPos[15]));
//			L15 = fmax(L15, personPose.jointPos[1].distance(personPose.jointPos[6]));
//			L56 = fmax(L56, personPose.jointPos[6].distance(personPose.jointPos[12]));
//			L67 = fmax(L67, personPose.jointPos[12].distance(personPose.jointPos[16]));
//			L18 = fmax(L18, personPose.jointPos[1].distance(personPose.jointPos[0]));
//			L89 = fmax(L89, personPose.jointPos[0].distance(personPose.jointPos[2]));
//			L910 = fmax(L910, personPose.jointPos[2].distance(personPose.jointPos[7]));
//			L1011 = fmax(L1011, personPose.jointPos[7].distance(personPose.jointPos[13]));
//			L812 = fmax(L812, personPose.jointPos[0].distance(personPose.jointPos[3]));
//			L1213 = fmax(L1213, personPose.jointPos[3].distance(personPose.jointPos[8]));
//			L1314 = fmax(L1314, personPose.jointPos[8].distance(personPose.jointPos[14]));
//			L217 = fmax(L217, personPose.jointPos[5].distance(personPose.jointPos[9]));
//			L518 = fmax(L518, personPose.jointPos[6].distance(personPose.jointPos[10]));
//		}
//	}
//
//	quickpose.setMaxBoneLength(0,  1,  L01   + 0.1f); std::cout << "L01: " << L01 << std::endl;
//	quickpose.setMaxBoneLength(1,  2,  L12   + 0.1f); std::cout << "L12: " << L12 << std::endl;
//	quickpose.setMaxBoneLength(2,  3,  L23   + 0.1f); std::cout << "L23: " << L23 << std::endl;
//	quickpose.setMaxBoneLength(3,  4,  L34   + 0.1f); std::cout << "L34: " << L34 << std::endl;
//	quickpose.setMaxBoneLength(1,  5,  L15   + 0.1f); std::cout << "L15: " << L15 << std::endl;
//	quickpose.setMaxBoneLength(5,  6,  L56   + 0.1f); std::cout << "L56: " << L56 << std::endl;
//	quickpose.setMaxBoneLength(6,  7,  L67   + 0.1f); std::cout << "L67: " << L67 << std::endl;
//	quickpose.setMaxBoneLength(1,  8,  L18   + 0.1f); std::cout << "L18: " << L18 << std::endl;
//	quickpose.setMaxBoneLength(8,  9,  L89   + 0.1f); std::cout << "L89: " << L89 << std::endl;
//	quickpose.setMaxBoneLength(9,  10, L910  + 0.1f); std::cout << "L910: " << L910 << std::endl;
//	quickpose.setMaxBoneLength(10, 11, L1011 + 0.1f); std::cout << "L1011: " << L1011 << std::endl;
//	quickpose.setMaxBoneLength(8,  12, L812  + 0.1f); std::cout << "L812: " << L812 << std::endl;
//	quickpose.setMaxBoneLength(12, 13, L1213 + 0.1f); std::cout << "L1213: " << L1213 << std::endl;
//	quickpose.setMaxBoneLength(13, 14, L1314 + 0.1f); std::cout << "L1314: " << L1314 << std::endl;
//	quickpose.setMaxBoneLength(2,  17, L217  + 0.1f); std::cout << "L217: " << L217 << std::endl;
//	quickpose.setMaxBoneLength(5,  18, L518  + 0.1f); std::cout << "L518: " << L518 << std::endl;
	
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
	
	quickpose.setMaxBoneLength(2, 17, L01 + L12 + 0.1f);
	quickpose.setMaxBoneLength(5, 18, L01 + L15 + 0.1f);
	
	multiPersonPosesGT = ShelfLoader::loadGroundTruth("../Dataset/shelf/shelf.gt.txt");
	shelfToBody25(multiPersonPosesGT);
	
	multiPersonPoses4DA = T4DALoader::loadGroundTruth("../Dataset/shelf/skel.txt");
	skel19ToBody25(multiPersonPoses4DA);
	
//	for (int i = 300; i <= 600; ++i) {
//		std::ifstream stream("/Users/hypertheory/Library/Containers/com.tencent.xinWeChat/Data/Library/"
//			"Application Support/com.tencent.xinWeChat/2.0b4.0.9/e9b7052fc37304807a644d9ce27a5c66/Message/MessageTemp/"
//			"acd947aa36f62520592385a34bb55869/File/SystemShelf3D/" + std::to_string(i) + ".txt", std::fstream::in);
//
//		if (stream.fail()) std::cout << "Fail\n";
//
//		int size = 0;
//		stream >> size;
//
//		multiPersonPoses4DA[i - 300].resize(size);
//
//		for (int j = 0; j < size; ++j) {
//			multiPersonPoses4DA[i - 300][j].hasJoint.resize(17);
//			multiPersonPoses4DA[i - 300][j].jointPos.resize(17);
//			for (int k = 0; k < 17; ++k) {
//				multiPersonPoses4DA[i - 300][j].hasJoint[k] = true;
//				auto& vec = multiPersonPoses4DA[i - 300][j].jointPos[k];
//				stream >> vec.x >> vec.y >> vec.z;
//			}
//		}
//
//		stream.close();
//	}
//	coco17ToBody25(multiPersonPoses4DA);
}

bool isComputed = true;

void execute(int frame) {
	if (isComputed) {
		auto& multiview = multiviews[frame];
		multiview.computeEpipolar(MAX_EPIPOLAR_DISTANCE);
		computedMultiPersonPose = quickpose.compute(multiview);
		correctShelfAtBody25(computedMultiPersonPose);
	} else {
		computedMultiPersonPose = multiPersonPoses4DA[frame];
		correctShelfAtBody25(computedMultiPersonPose);
	}
//	std::cout << "Count: " << quickpose.count << std::endl;
}

void evaluate(int frame) {
	static float accumA1 = 0;
	static float totalA1 = 0;
	
	static float accumA2 = 0;
	static float totalA2 = 0;
	
	static float accumA3 = 0;
	static float totalA3 = 0;
	
	std::vector<int> boneA = {
		5, 2, 6, 3, 12, 9, 13, 10, 1, 1, 1, 1, 8, 8, 2, 5
	};
	std::vector<int> boneB = {
		6, 3, 7, 4, 13, 10, 14, 11, 0, 8, 2, 5, 9, 12, 17, 18
	};
	
	auto& multiPersonPoseGT = multiPersonPosesGT[frame + 300];
	auto& multiPersonPose = computedMultiPersonPose;
	
	OneRoom::cleanUpJointsAndBones();
	
	for (auto& pose : multiPersonPose) {
		if (pose.hasJoint.empty()) continue;
		for (int type = 0; type < 15; ++type) {
			if (!pose.hasJoint[type]) continue;
			OneRoom::setJoint(mapping(pose.jointPos[type]));
		}
		if (pose.hasJoint[17]) {
			OneRoom::setJoint(mapping(pose.jointPos[17]));
		}
		if (pose.hasJoint[18]) {
			OneRoom::setJoint(mapping(pose.jointPos[18]));
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
		if (poseGT.ID == 4) continue;
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
			if (bone >= 10) {
				OneRoom::setBone(posA, posB, WHITE);
				continue;
			}
			OneRoom::setBone(posA, posB, boneTests[bone] ? GREEN : RED);
		}
		
		for (int bone = 0; bone < boneSize; ++bone) {
			if (bone >= 10) continue;
			if (poseGT.ID == 1) {
				accumA1 += boneTests[bone];
				totalA1 += 1.;
			} else if (poseGT.ID == 2) {
				accumA2 += boneTests[bone];
				totalA2 += 1.;
			} else if (poseGT.ID == 3) {
				accumA3 += boneTests[bone];
				totalA3 += 1.;
			}
		}
	}
	
	float PCPA1 = accumA1 / totalA1;
	float PCPA2 = accumA2 / totalA2;
	float PCPA3 = accumA3 / totalA3;
	float PCPAvg = (PCPA1 + PCPA2 + PCPA3) / 3.;
	
	std::cout << "Frame: " << frame << std::string(4 - std::to_string(frame).size(), ' ');
	std::cout << " | PCP | A1: " << (isnan(PCPA1) ? "N/A     " : std::to_string(PCPA1));
	std::cout << " | A2: " << (isnan(PCPA2) ? "N/A     " : std::to_string(PCPA2));
	std::cout << " | A3: " << (isnan(PCPA3) ? "N/A     " : std::to_string(PCPA3));
	std::cout << " | Avg: " << (isnan(PCPAvg) ? "N/A     " : std::to_string(PCPAvg));
	std::cout << "\n";
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
	t.fps = 30;
	t.show_cursor = false;
	t.lock_cursor = true;
	t.borderless = true;
}

void load() {
	registerErrorCallback();
	
	INK_SET_SHADER_PATH("./External/ink/shaders/");
	
	OneRoom::setRealtimeReflection(false);
	OneRoom::prepareResources();
	OneRoom::setupPipeline(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	
	prepare();
	
//	cameras.resize(5);
//
//	Ink::Vec3 p = {0.594279 + 1., 0.971974, 2.624511};
//	Ink::Vec3 dn = {0.758863, -0.045434, 0.649664};
//	Ink::Vec3 un = -Ink::Vec3{-0.018862, -0.998678, -0.047810};
//	Ink::Vec3 rn = -dn.cross(un).normalize();
//
//	cameras[0].viewing = {
//		rn.x      , rn.y      , rn.z      , -p.dot(rn),
//		un.x      , un.y      , un.z      , -p.dot(un),
//		dn.x      , dn.y      , dn.z      , -p.dot(dn),
//		0         , 0         , 0         , 1         ,
//	};
//
//	cameras[0].projection = {
//		0.736286, 0.000000, 0.000000, 0.000000,
//		0.000000, 1.303225, 0.000000, 0.000000,
//		0.000000, 0.000000, -1.000200, -0.100010,
//		0.000000, 0.000000, -1.000000, 0.000000,
//	};
}

void update(float dt) {
	static int frameIndex = 259;
	static bool needsUpdate = true;
	
	if (Ink::Window::is_pressed(SDLK_TAB)) {
		needsUpdate = true;
		isComputed = !isComputed;
	}
	
	if (needsUpdate) {
		std::cout << (isComputed ? "Quickpose now\n" : "4DAssociation now\n");
		execute(frameIndex);
		evaluate(frameIndex);
		needsUpdate = false;
		Ink::Window::set_title("Frame: " + std::to_string(frameIndex));
	}
	
	OneRoom::update(dt);
	OneRoom::render();
	
	if (Ink::Window::is_pressed(SDLK_RIGHT) || Ink::Window::is_down(SDLK_SPACE)) {
		frameIndex = (frameIndex + 1) % 300;
		needsUpdate = true;
	}
	
	if (Ink::Window::is_pressed(SDLK_LEFT) || Ink::Window::is_down(SDLK_BACKSPACE)) {
		frameIndex = (frameIndex + 299) % 300;
		needsUpdate = true;
	}
	
	if (Ink::Window::is_down(SDLK_1)) {
		Visualizer2D::visualize(computedMultiPersonPose, multiviews[frameIndex].views[0],
								DATASET_DIR + fmt::format("/Camera{}/img_{:0>6d}.png", 0, frameIndex + 300));
	}
	
	if (Ink::Window::is_pressed(SDLK_2)) {
		Visualizer2D::visualize(computedMultiPersonPose, multiviews[frameIndex].views[1],
								DATASET_DIR + fmt::format("/Camera{}/img_{:0>6d}.png", 1, frameIndex + 300));
	}
	
	if (Ink::Window::is_pressed(SDLK_3)) {
		Visualizer2D::visualize(computedMultiPersonPose, multiviews[frameIndex].views[2],
								DATASET_DIR + fmt::format("/Camera{}/img_{:0>6d}.png", 2, frameIndex + 300));
	}
	
	if (Ink::Window::is_pressed(SDLK_4)) {
		Visualizer2D::visualize(computedMultiPersonPose, multiviews[frameIndex].views[3],
								DATASET_DIR + fmt::format("/Camera{}/img_{:0>6d}.png", 3, frameIndex + 300));
	}
	
	if (Ink::Window::is_pressed(SDLK_5)) {
		Visualizer2D::visualize(computedMultiPersonPose, multiviews[frameIndex].views[4],
								DATASET_DIR + fmt::format("/Camera{}/img_{:0>6d}.png", 4, frameIndex + 300));
	}
}

void quit() {}
