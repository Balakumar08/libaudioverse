/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "private_threads.h"
#include "libaudioverse.h"
#ifdef __cplusplus
extern "C" {
#endif

Lav_PUBLIC_FUNCTION LavError initLavObject(unsigned int numInputs, unsigned int numOutputs, unsigned int numProperties, LavPropertyTableEntry* propertyTable, enum  Lav_NODETYPES type, unsigned int blockSize, void* mutex, LavObject *destination);
Lav_PUBLIC_FUNCTION LavError Lav_createObject(unsigned int numInputs, unsigned int numOutputs, unsigned int numProperties, LavPropertyTableEntry* propertyTable, enum  Lav_NODETYPES type, unsigned int blockSize, void* mutex, LavObject **destination);
Lav_PUBLIC_FUNCTION LavError objectProcessSafe(LavObject* obj);

#ifdef __cplusplus
}
#endif