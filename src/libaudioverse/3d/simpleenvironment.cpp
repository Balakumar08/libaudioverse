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

namespace libaudioverse_implementation {

SimpleEnvironmentNode::SimpleEnvironmentNode(std::shared_ptr<Simulation> simulation, std::shared_ptr<HrtfData> hrtf): EnvironmentBase(Lav_OBJTYPE_SIMPLE_ENVIRONMENT_NODE, simulation)  {
	this->hrtf = hrtf;
	int channels = getProperty(Lav_ENVIRONMENT_OUTPUT_CHANNELS).getIntValue();
	output = createGainNode(simulation);
	output->resize(channels, channels);
	output->appendInputConnection(0, channels);
	output->appendOutputConnection(0, channels);
	appendOutputConnection(0, channels);
	setOutputNode(output);
	environment.world_to_listener_transform = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
}

std::shared_ptr<SimpleEnvironmentNode> createSimpleEnvironmentNode(std::shared_ptr<Simulation> simulation, std::shared_ptr<HrtfData> hrtf) {
	auto retval = std::shared_ptr<SimpleEnvironmentNode>(new SimpleEnvironmentNode(simulation, hrtf), ObjectDeleter(simulation));
	simulation->associateNode(retval);
	return retval;
}

void SimpleEnvironmentNode::willProcessParents() {
	if(werePropertiesModified(this, Lav_ENVIRONMENT_OUTPUT_CHANNELS)) outputChannelsChanged();
	if(werePropertiesModified(this, Lav_3D_POSITION, Lav_3D_ORIENTATION)) {
		//update the matrix.
		//Important: look at the glsl constructors. Glm copies them, and there is nonintuitive stuff here.
		const float* pos = getProperty(Lav_3D_POSITION).getFloat3Value();
		const float* atup = getProperty(Lav_3D_ORIENTATION).getFloat6Value();
		auto at = glm::vec3(atup[0], atup[1], atup[2]);
		auto up = glm::vec3(atup[3], atup[4], atup[5]);
		auto right = glm::cross(at, up);
		auto m = glm::mat4(
		right.x, up.x, -at.x, 0,
		right.y, up.y, -at.y, 0,
		right.z, up.z, -at.z, 0,
		0, 0, 0, 1);
		//Above is a rotation matrix, which works presuming the player is at (0, 0).
		//Pass the translation through it, so that we can bake the translation in.
		auto posvec = m*glm::vec4(pos[0], pos[1], pos[2], 1.0f);
		//[column][row] because GLSL.
		m[3][0] = -posvec.x;
		m[3][1] = -posvec.y;
		m[3][2] = -posvec.z;
		environment.world_to_listener_transform = m;
		//this debug code left in case this is still all broken.
		/*printf("\n%f %f %f %f\n", m[0][0], m[1][0], m[2][0], m[3][0]);
		printf("%f %f %f %f\n", m[0][1], m[1][1], m[2][1], m[3][1]);
		printf("%f %f %f %f\n", m[0][2], m[1][2], m[2][2], m[3][2]);
		printf("%f %f %f %f\n\n", m[0][3], m[1][3], m[2][3], m[3][3]);*/
	}
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

std::shared_ptr<Node> SimpleEnvironmentNode::createPannerNode() {
	auto pan = createMultipannerNode(simulation, hrtf);
	pan->connect(0, output, 0);
	return pan;
}

void SimpleEnvironmentNode::registerSourceForUpdates(std::shared_ptr<SourceNode> source) {
	sources.insert(source);
}

void SimpleEnvironmentNode::outputChannelsChanged() {
	int channels = getProperty(Lav_ENVIRONMENT_OUTPUT_CHANNELS).getIntValue();
	output->resize(channels, channels);
	getOutputConnection(0)->reconfigure(0, channels);
	output->getOutputConnection(0)->reconfigure(0, channels);
	output->getInputConnection(0)->reconfigure(0, channels);
}

//begin public api

Lav_PUBLIC_FUNCTION LavError Lav_createSimpleEnvironmentNode(LavHandle simulationHandle, const char*hrtfPath, LavHandle* destination) {
	PUB_BEGIN
	auto simulation = incomingObject<Simulation>(simulationHandle);
	LOCK(*simulation);
	auto hrtf = std::make_shared<HrtfData>();
	if(std::string(hrtfPath) != "default") {
		hrtf->loadFromFile(hrtfPath, simulation->getSr());
	} else {
		hrtf->loadFromDefault(simulation->getSr());
	}
	auto retval = createSimpleEnvironmentNode(simulation, hrtf);
	*destination = outgoingObject<Node>(retval);
	PUB_END
}

}