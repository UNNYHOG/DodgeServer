#ifndef _I_UDP_ASYNC_CLIENT_
#define _I_UDP_ASYNC_CLIENT_

#include "types.h"
#include "DataHandler.h"

class IUdpAsyncClient
{
public:

	static IUdpAsyncClient* NewClient (ushort self_port, const char* server_name, ushort server_port, 
		DataHandler rec_callback, DataHandler save_callback, DataHandler err_callback);

	virtual void StartSend (const void* data, size_t size) = 0;

protected:

	IUdpAsyncClient () {};

private:

	IUdpAsyncClient (const IUdpAsyncClient&);
	const IUdpAsyncClient& operator= (const IUdpAsyncClient&);
};

#endif