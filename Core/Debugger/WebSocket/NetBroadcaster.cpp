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

#include "Common/TimeUtil.h"
#include "Core/Debugger/WebSocket/NetBroadcaster.h"

std::mutex NetBroadcaster::mutex_;
std::vector<NetRequestInfo> NetBroadcaster::pendingRequests_;

NetBroadcaster::NetBroadcaster() {}
NetBroadcaster::~NetBroadcaster() {}

void NetBroadcaster::RecordRequest(const std::string &url, const std::string &method, const std::string &range, size_t size, double duration, int status) {
	std::lock_guard<std::mutex> guard(mutex_);
	NetRequestInfo info;
	info.url = url;
	info.method = method;
	info.range = range;
	info.size = size;
	info.timestamp = time_now_d();
	info.duration = duration;
	info.status = status;
	pendingRequests_.push_back(info);

	// Limit buffer size
	if (pendingRequests_.size() > 100) {
		pendingRequests_.erase(pendingRequests_.begin());
	}
}

struct DebuggerNetEvent {
	const NetRequestInfo &r;

	operator std::string() {
		JsonWriter j;
		j.begin();
		j.writeString("event", "net.request");
		j.writeString("url", r.url);
		j.writeString("method", r.method);
		j.writeString("range", r.range);
		j.writeInt("size", (int)r.size);
		j.writeFloat("timestamp", r.timestamp);
		j.writeFloat("duration", r.duration);
		j.writeInt("status", r.status);
		j.end();
		return j.str();
	}
};

void NetBroadcaster::Broadcast(net::WebSocketServer *ws) {
	std::lock_guard<std::mutex> guard(mutex_);
	for (const auto &req : pendingRequests_) {
		ws->Send(DebuggerNetEvent{req});
	}
	pendingRequests_.clear();
}

DebuggerSubscriber *WebSocketNetInit(DebuggerEventHandlerMap &map) {
	return new NetBroadcaster();
}
