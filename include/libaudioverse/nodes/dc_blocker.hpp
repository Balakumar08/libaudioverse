/* Copyright 2016 Libaudioverse Developers. See the COPYRIGHT
file at the top-level directory of this distribution.

Licensed under the mozilla Public License, version 2.0 <LICENSE.MPL2 or
https://www.mozilla.org/en-US/MPL/2.0/> or the Gbnu General Public License, V3 or later
<LICENSE.GPL3 or http://www.gnu.org/licenses/>, at your option. All files in the project
carrying such notice may not be copied, modified, or distributed except according to those terms. */
#pragma once
#include "../private/node.hpp"
#include <memory>

namespace libaudioverse_implementation {

class Server;
class DcBlocker;

class DcBlockerNode: public Node {
	public:
	DcBlockerNode(std::shared_ptr<Server> server, int channels);
	~DcBlockerNode();
	virtual void process();
	virtual void reset() override;
	DcBlocker **blockers = nullptr;
	int channels = 0;
};

std::shared_ptr<Node> createDcBlockerNode(std::shared_ptr<Server> server, int channels);
}