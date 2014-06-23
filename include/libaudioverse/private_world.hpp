/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "libaudioverse.h"
#include "private_objects.hpp"

class LavSource;
class LavHrtfData;

class LavWorld: public LavObject {
	public:
	LavWorld(LavDevice* device, LavHrtfData* hrtf);
	void associateSource(LavSource* source);
	//call update on all sources.
	virtual void willProcessParents();
	private:
	std::vector<LavSource*> sources;
	unsigned int num_sources, max_sources;
	TmTransform camera_transform; //the camera transform.
	LavObject* mixer = nullptr, *limiter = nullptr;
	LavHrtfData *hrtf;
};
