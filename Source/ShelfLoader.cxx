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

#include "ShelfLoader.h"

#include "Ink.h"

#include <fstream>
#include <iostream>

MultiPersonPoses ShelfLoader::loadGroundTruth(const std::string& path) {
	std::ifstream stream(path, std::fstream::in);
	
	if (stream.fail()) {
		std::cerr << "ShelfLoader: Failed to load ground truth\n";
		return MultiPersonPoses();
	}
	
	size_t streamMax = std::numeric_limits<std::streamsize>::max();
	
	MultiPersonPoses multiPersonPoses;
	MultiPersonPose* curMultiPersonPose = nullptr;
	Pose* curPose = nullptr;
	
	while (!stream.eof()) {
		std::string keyword;
		stream >> keyword;
		
		if (keyword == "frame") {
			int frameIndex = 0;
			stream >> frameIndex;
			multiPersonPoses.emplace_back(MultiPersonPose());
			curMultiPersonPose = &multiPersonPoses.back();
		}
		
		else if (keyword == "p") {
			curMultiPersonPose->emplace_back(Pose());
			curPose = &curMultiPersonPose->back();
			stream >> curPose->ID;
		}
		
		else if (keyword == "v") {
			Ink::Vec3 pos;
			stream >> pos.x >> pos.y >> pos.z;
			curPose->hasJoint.emplace_back(true);
			curPose->jointPos.emplace_back(pos);
		}
		
		else {
			stream.ignore(streamMax, '\n');
		}
	}
	
	stream.close();
	
	return multiPersonPoses;
}
