#pragma once

#include <config/config.h>
#include <server/control_handler_if.h>
#include <kj/async-io.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern "C"
{
#include "libartnet/artnet.h"
}

namespace DmxHueNode {

class ArtnetServer : private kj::TaskSet::ErrorHandler
{
public:
	explicit ArtnetServer(const Config&, kj::AsyncIoContext&, IControlHandler&);
	~ArtnetServer();

	void StartListening();

private:
	void PollSocket();
	std::optional<std::string> FindIpAddress();
	void taskFailed(kj::Exception&& exception) override;

	// libartnet handlers
	static int FirmwareHandler(artnet_node n, int ubea, uint16_t *data, int length, void *d);
	static int DmxHandler(artnet_node n, void* packet, void* data);

private:
	const Config& mConfig;
	kj::AsyncIoContext& mAsio;
	kj::Timer& mTimer;
	kj::TaskSet mTasks{*this};
	IControlHandler& mHandler;

	artnet_node mNode {NULL};
	int mNodeSd;
	struct timeval mTv;
	fd_set mReadFds;
};

}