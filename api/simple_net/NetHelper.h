#pragma once
#include "NetConfig.h"
#include "NetProtocol.h"

#define DEFAULT_SND_TIMEOUT 5000
#define DEFAULT_RCV_TIMEOUT 5000
#define HEARTBEAT_TIME 1000
#define RECV_BUFF_SIZE 1024*1024 //1MB


POCO_DECLARE_EXCEPTION(, SimpleNetException, NetException)

//handle exception begin
#define EXCEPTION_BEGIN      \
int error_code(0);                                 \
std::string error_msg;                             \
try{                                               \

//handle exception end
#define EXCEPTION_END        \
}                                                  \
catch (Poco::Net::ConnectionResetException& e){    \
	error_code = SN_NETWORK_DISCONNECTED;          \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::Net::ConnectionAbortedException& e){  \
	error_code = SN_NETWORK_DISCONNECTED;          \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::TimeoutException& e){                 \
    error_code = SN_NETWORK_TIMEOUT;               \
    error_msg = e.displayText();                   \
}                                                  \
catch (SimpleNetException& e){                     \
	error_code = e.code();                         \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::Exception& e){                        \
	error_code = SN_NETWORK_ERROR;                 \
	error_msg = e.displayText();                   \
}                                                  \
if (error_code != 0){                              \
	LOG(ERROR) << "error code : " <<               \
		NetHelper::StrError(error_code)            \
		<< "  error msg : " << error_msg;          \
}


enum NetError
{
	SN_NETWORK_ERROR = 1000,
	SN_NETWORK_DISCONNECTED,
	SN_NETWORK_TIMEOUT,
	SN_PAYLOAD_TOO_BIG,
	SN_FRAME_ERROR,
};


struct _TCP_HEADER;
class NetHelper
{
public:
	//if recv buff is small, cause PayloadTooBigException
	NetHelper(int recvlen = RECV_BUFF_SIZE);

	//if recv buff is small, cause PayloadTooBigException
	NetHelper(const StreamSocket& socket, int recvlen = RECV_BUFF_SIZE);
	virtual ~NetHelper();

	//event
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnError(int error_code, const std::string& error_msg) = 0;
	virtual void OnRecvFrame(const byte* data, int len, int type) = 0;

	//send string
	void SendFrameString(const std::string& data);

	//send binary
	void SendFrameBinary(const byte* data, int len);

	//send string or binary
	void SendFrame(const byte* data, int len, int type);

	//return msglen
	//int RecvFrame(byte* data, int len, int* type);

	//local address
	std::string Address();

	//remote address
	std::string RemoteAddress();
	
	bool IsConnected() const;

	bool IsCalled() const;

	static const char* StrError(int err);
protected:
	void close();
	void sendFrame(int msgtype, int frametype, const byte* data, int len);
	//return msglen
	int recvFrame(int* msgtype, int* frametype, byte* data, int len);

	void sendAll(const byte* data, int len);
	void recvAll(byte* data, int len);
	void checkHeader(const _TCP_HEADER& header);
	void readEmptyBuffer();
protected:
	StreamSocket _socket;
	utils::Mutex _sendMutex;

	bool _connected;

	byte* _recvbuff;
	int _recvlen;  //default 1MB

	bool _called; //if event called.
};

inline void NetHelper::SendFrameString(const std::string& data){
	return SendFrame((byte*)data.c_str(), data.length(), FRAME_STRING);
}

inline void NetHelper::SendFrameBinary(const byte* data, int len){
	return SendFrame(data, len, FRAME_BINARY);
}

inline std::string NetHelper::Address(){
	return _socket.address().host().toString();
}

inline std::string NetHelper::RemoteAddress(){
	return _socket.peerAddress().host().toString();
}

inline bool NetHelper::IsConnected() const{
	return _connected;
}

inline bool NetHelper::IsCalled() const{
	return _called;
}

inline const char* NetHelper::StrError(int err){
	switch (err)
	{
	case SN_NETWORK_ERROR: return "SN_NETWORK_ERROR";
	case SN_NETWORK_DISCONNECTED: return "SN_NETWORK_DISCONNECTED";
	case SN_NETWORK_TIMEOUT: return "SN_NETWORK_TIMEOUT";
	case SN_PAYLOAD_TOO_BIG: return "SN_PAYLOAD_TOO_BIG";
	case SN_FRAME_ERROR: return "SN_FRAME_ERROR";
	default: assert(false); return "**SN_UNKNOW**";
	}
}

inline void NetHelper::close(){
	_socket.shutdown();
	_socket.close();
}




