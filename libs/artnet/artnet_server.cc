#include "artnet_server.h"

#include <core/logger.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C"
{
#include "libartnet/packets.h"
}

namespace DmxHueNode {

static const LogModule LM_ARTNET {"ARTNET_SERVERR"};
static const kj::Duration POLL_SOCKET_FREQUENCY {500 * kj::NANOSECONDS};
static IControlHandler* controlHandlerPtr = nullptr; // so that we have access from the C library
static uint16_t listenUniverse = 0;


ArtnetServer::ArtnetServer(const Config& config, kj::AsyncIoContext& asio, IControlHandler& handler)
	: mConfig(config)
	, mAsio(asio)
	, mTimer(mAsio.provider->getTimer())
	, mHandler(handler)
{
	controlHandlerPtr = &mHandler;
	listenUniverse = mConfig.mDmxUniverse;
}

ArtnetServer::~ArtnetServer()
{
	if (mNode != NULL)
	{
		artnet_stop(mNode);
		artnet_destroy(mNode);
	}
}

void ArtnetServer::StartListening()
{
	std::optional<std::string> localIpAddress = FindIpAddress();
	const char* ip_addr = NULL;
	if (localIpAddress)
	{
		ip_addr = localIpAddress->c_str();
	}

	int verbose = 1;
	mNode = artnet_new(ip_addr, verbose);
	if (mNode == NULL)
	{
		LOG(ERROR, LM_ARTNET, "unable to create artnet node, stopping.")
		exit(1);
	}

	artnet_set_short_name(mNode, "Hue Node");
	artnet_set_long_name(mNode, "ArtNet Philips Hue Node");
	artnet_set_node_type(mNode, ARTNET_NODE);

	artnet_set_firmware_handler(mNode, FirmwareHandler, NULL);
	artnet_set_handler(mNode, ARTNET_DMX_HANDLER, DmxHandler, NULL);

	int port_id = 0; // first port
	uint8_t universe = 0x01; // subnet 0, universe 3

	if (artnet_start(mNode) != ARTNET_EOK)
	{
		LOG(ERROR, LM_ARTNET, "unable to start artnet node, stopping.")
		exit(1);
	}

	mNodeSd = artnet_get_sd(mNode);
	mTv.tv_sec = 0;
	mTv.tv_usec = 0;
	fcntl(mNodeSd, F_SETFL, O_NONBLOCK);

	PollSocket();
}

void ArtnetServer::PollSocket()
{
	mTasks.add(mTimer.afterDelay(POLL_SOCKET_FREQUENCY).then([this]() { PollSocket(); }));

	FD_ZERO(&mReadFds);
	FD_SET(mNodeSd, &mReadFds);

	int rv = select(mNodeSd + 1, &mReadFds, NULL, NULL, &mTv);
	if (rv != 1)
	{
		return;  // no data on socket
	}

	LOG(DEBUG, LM_ARTNET, "received data");
	int res = artnet_read(mNode, 0);
	if (res != ARTNET_EOK)
	{
		LOG(WARN, LM_ARTNET, "unable to read received artnet message successfully");
	}
};

int ArtnetServer::FirmwareHandler(artnet_node n, int ubea, uint16_t *data, int length, void *d)
{
	LOG(DEBUG, LM_ARTNET, "firmware handler got " << length << "words");
	return 0;
}

int ArtnetServer::DmxHandler(artnet_node n, void* packet, void* data)
{
	LOG(DEBUG, LM_ARTNET, "received dmx data");
	artnet_packet artnetPacket = (artnet_packet) packet;
	if (artnetPacket->data.admx.universe != listenUniverse)
	{
		LOG(DEBUG, LM_ARTNET, "received dmx data for wrong universe, expected: " << listenUniverse
				<< ", got " << artnetPacket->data.admx.universe);
		return 0;
	}

	uint8_t* dmxChannels = artnetPacket->data.admx.data;
	controlHandlerPtr->OnDmxMessage(dmxChannels);
	return 0;
}

// find the ip address by binding to google's dns server, and looking at our ip that way
std::optional<std::string> ArtnetServer::FindIpAddress()
{
	const char* google_dns_server = "8.8.8.8";
	int dns_port = 53;

	struct sockaddr_in serv;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	// socket could not be created
	if (sock < 0)
	{
		LOG(ERROR, LM_ARTNET, "unable to create socket to google dns server to find local ip of socket");
		return std::nullopt;
	}

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(google_dns_server);
	serv.sin_port = htons(dns_port);

	int err = connect(sock, (const struct sockaddr*)&serv, sizeof(serv));
	if (err < 0)
	{
		LOG(ERROR, LM_ARTNET, "unable to connect socket to google dns server to find local ip of socket");
		return std::nullopt;
	}

	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	err = getsockname(sock, (struct sockaddr*)&name, &namelen);

	char buffer[80];
	const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 80);
	close(sock);

	if(p != NULL)
	{
		LOG(INFO, LM_ARTNET, "found local ip: " << buffer);
		return buffer;
	}
	else
	{
		LOG(ERROR, LM_ARTNET, "unable to find local ip using google dns lookup");
		return std::nullopt;
	}
}

void ArtnetServer::taskFailed(kj::Exception&& exception)
{
	std::string reason = "Exception during task " + std::string(exception.getDescription().cStr());
	LOG(ERROR, LM_ARTNET, reason);
}

}