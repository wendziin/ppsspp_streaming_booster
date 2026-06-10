// Copyright (c) 2018- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#include <vector>
#include <mutex>
#include "Core/Debugger/WebSocket/WebSocketUtils.h"

struct NetRequestInfo {
	std::string url;
	std::string method;
	std::string range;
	size_t size;
	double timestamp;
	double duration;
	int status;
};

class NetBroadcaster : public DebuggerSubscriber {
public:
	NetBroadcaster();
	~NetBroadcaster();

	void Broadcast(net::WebSocketServer *ws) override;

	static void RecordRequest(const std::string &url, const std::string &method, const std::string &range, size_t size, double duration, int status);

private:
	static std::mutex mutex_;
	static std::vector<NetRequestInfo> pendingRequests_;
};

DebuggerSubscriber *WebSocketNetInit(DebuggerEventHandlerMap &map);
