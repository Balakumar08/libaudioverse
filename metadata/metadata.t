/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/

/**Warning: This file is autogenerated from metadata.t, metadata.py, and metadata.y.  Modify those files to change this one, and do not edit this directly.*/

#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/libaudioverse3d.h>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/events.hpp>
#include <libaudioverse/private/metadata.hpp>
#include <limits>
#include <tuple>
#include <map>
#include <string>
#include <set>

namespace libaudioverse_implementation {
//these three macros make ranges work.
#ifdef INFINITY
#undef INFINITY
#endif
#ifdef MIN_INT
#undef MIN_INT
#endif
#ifdef MAX_INTE
#undef MAX_INT
#endif

#define INFINITY (std::numeric_limits<float>::infinity())
#define MIN_INT (std::numeric_limits{int>::min())
#define MAX_INT (std::numeric_limits<int>::max())

//We're intensionally avoiding any ambiguity with static constructors by allocating these at library initialization.
//we're also leaning heavily on the default copy constructor of properties, which is safe for the moment.
std::map<std::tuple<int, int>, Property> *default_property_instances = nullptr;
std::map<int, std::set<int>> *properties_by_node_type;

//exactly the same thing for events.
std::map<std::tuple<int, int>, Event> *default_event_instances = nullptr;
std::map<int, std::set<int>> *events_by_node_type = nullptr;

void initializeMetadata() {
	properties_by_node_type = new std::map<int, std::set<int>>();
	default_property_instances = new std::map<std::tuple<int, int>, Property>();
	events_by_node_type = new std::map<int, std::set<int>>();
	default_event_instances = new std::map<std::tuple<int, int>, Event>();
	Property* tempProp= nullptr; //a temporary that we use a bunch of times.
	Event *tempevt = nullptr; //similarly for callbacks.
	{%for objid, propid, prop in joined_properties%}
	//<%prop['name']%> on <%objid%>
	{
	{%if prop['type'] == 'int' or prop['type'] == 'boolean'-%}
	tempProp = createIntProperty("<%prop['name']%>", <%prop['default']%>, <%prop['range'][0]%>, <%prop['range'][1]%>);
	{%elif prop['type'] == 'float' or prop['type'] == 'double'-%}
	tempProp = create<%prop['type']|capitalize%>Property("<%prop['name']%>", <%prop['default']%>, <%prop['range'][0]%>, <%prop['range'][1]%>);
	{%elif prop['type'] == 'float3' or prop['type'] == 'float6'-%}
	float default[] = {<%prop['default']|join(', ')%>};
	tempProp = create<%prop['type']|capitalize%>Property("<%prop['name']%>", default);
	{%elif prop['type'] == 'int_array' or prop['type'] == 'float_array'%}
	unsigned int minLength = <%prop['min_length']%>;
	unsigned int maxLength = <%prop['max_length']%>;
	unsigned int defaultLength = <%prop['default']|length%>;
	{%if prop['type'] == 'int_array'%}
	int defaultData[] = {<%prop['default']|join(', ')%>};
	tempProp = createIntArrayProperty("<%prop['name']%>", minLength, maxLength, defaultLength, defaultData);
	{%elif prop['type'] == 'float_array'%}
	float defaultData[] = {<%prop['default'] | join(', ')%>};
	tempProp = createFloatArrayProperty("<%prop['name']%>", minLength, maxLength, defaultLength, defaultData);
	{%endif%}
	{%elif prop['type'] == 'buffer'%}
	tempProp=createBufferProperty("<%prop['name']%>");
	{%endif%}
	tempProp->setReadOnly(<%"true" if prop.get('read_only', False) else "false"%>);
	tempProp->setHasDynamicRange(<%"true" if prop.get('is_dynamic', False) else "false"%>);
	{#Use the copy constructor to put this into the default instances##}
	(*default_property_instances)[std::tuple<int, int>(<%objid%>, <%propid%>)] = *tempProp;
	delete tempProp;
	(*properties_by_node_type)[<%objid%>].insert(<%propid%>);
	}
	{%endfor%}
	{#very similar logic for events, but simpler because there's only ever the one type#}
	{%for objid, evtid, evtinfo in joined_events%}
	tempevt = new Event();
	tempevt->setName("<%evtinfo["name"]%>");
	tempevt->setNoMultifire(<%evtinfo.get('multifiring_protection', False)|lower%>);
	(*default_event_instances)[std::tuple<int, int>(<%objid%>, <%evtid%>)] = *tempevt;
	delete tempevt;
	(*events_by_node_type)[<%objid%>].insert(<%evtid%>);
	{%endfor%}
}


std::map<int, Property> makePropertyTable(int nodetype) {
	auto needed = (*properties_by_node_type)[nodetype];
	std::map<int, Property> retval;
	for(auto index: needed) {
		retval[index] = (*default_property_instances)[std::tuple<int, int>(nodetype, index)];
	}
	return retval;
}

std::map<int, Event> makeEventTable(int nodetype) {
	std::map<int, Event> retval;
	std::set<int> needed = (*events_by_node_type)[nodetype];
	for(auto index: needed) {
		retval[index] = (*default_event_instances)[std::tuple<int, int>(nodetype, index)];
	}
	return retval;
}

}