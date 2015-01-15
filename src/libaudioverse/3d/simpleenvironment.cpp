/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/

#include <libaudioverse/3d/sources.hpp>
#include <libaudioverse/3d/simpleenvironment.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/creators.hpp>
#include <libaudioverse/private/simulation.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/private/hrtf.hpp>
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/libaudioverse3d.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <vector>

LavSimpleEnvironmentNode::LavSimpleEnvironmentNode(std::shared_ptr<LavSimulation> simulation, std::shared_ptr<LavHrtfData> hrtf): LavEnvironmentBase(Lav_NODETYPE_SIMPLE_ENVIRONMENT, simulation)  {
	this->hrtf = hrtf;
	mixer = createMixerNode(simulation, 1, 8);
	limiter = createHardLimiterNode(simulation, 8);
	for(int i = 0; i < 8; i++) {
		limiter->setInput(i, mixer, i);
	}

	environment.world_to_listener_transform = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	configureSubgraph(nullptr, limiter);
}

std::shared_ptr<LavSimpleEnvironmentNode> createSimpleEnvironmentNode(std::shared_ptr<LavSimulation> simulation, std::shared_ptr<LavHrtfData> hrtf) {
	auto retval = std::shared_ptr<LavSimpleEnvironmentNode>(new LavSimpleEnvironmentNode(simulation, hrtf), LavNodeDeleter);
	simulation->associateNode(retval);
	return retval;
}

void LavSimpleEnvironmentNode::willProcessParents() {
	//update the matrix.
	const float* pos = getProperty(Lav_3D_POSITION).getFloat3Value();
	const float* atup = getProperty(Lav_3D_ORIENTATION).getFloat6Value();
	environment.world_to_listener_transform = glm::lookAt(
		glm::vec3(pos[0], pos[1], pos[2]),
		//this is a point in 3d space, not a unit vector indicating direction.
		glm::vec3(pos[0]+atup[0], pos[1]+atup[1], pos[2]+atup[2]),
		glm::vec3(atup[3], atup[4], atup[5]));

	//give the new environment to the sources.
	//this is a set of weak pointers.
	decltype(sources) needsRemoval; //for source cleanup below.
	for(auto i: sources) {
		auto tmp = i.lock();
		if(tmp == nullptr) {
			needsRemoval.insert(i);
			continue;
		}
		tmp->update(environment);
	}
	//do cleanup of dead sources.
	for(auto i: needsRemoval) sources.erase(i);
}

std::shared_ptr<LavNode> LavSimpleEnvironmentNode::createPannerNode() {
	auto pan = createMultipannerNode(simulation, hrtf);
	unsigned int slot = panners.size();
	panners.push_back(pan);
	//expand the mixer by one parent.
	mixer->getProperty(Lav_MIXER_MAX_PARENTS).setIntValue(panners.size());
	for(unsigned int i = 0; i < 8; i++) {
		mixer->setInput(slot*8+i, pan, i);
	}
	return pan;
}

void LavSimpleEnvironmentNode::registerSourceForUpdates(std::shared_ptr<LavSourceNode> source) {
	sources.insert(source);
}

//begin public api

Lav_PUBLIC_FUNCTION LavError Lav_createSimpleEnvironmentNode(LavSimulation* simulation, const char*hrtfPath, LavNode** destination) {
	PUB_BEGIN
	LOCK(*simulation);
	auto hrtf = std::make_shared<LavHrtfData>();
	if(std::string(hrtfPath) != "default") {
		hrtf->loadFromFile(hrtfPath, simulation->getSr());
	} else {
		hrtf->loadFromDefault(simulation->getSr());
	}
	auto retval = createSimpleEnvironmentNode(incomingPointer<LavSimulation>(simulation), hrtf);
	*destination = outgoingPointer<LavNode>(retval);
	PUB_END
}
