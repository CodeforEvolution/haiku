/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef EFI_NETWORK_H
#define EFI_NETWORK_H

#include <boot/net/Ethernet.h>
#include <boot/net/NetStack.h>

#include <efi/protocol/simple-network.h>


class SNP : public EthernetInterface {
public:
								SNP();
	virtual						~SNP();
	
			status_t			Init();

	virtual	mac_addr_t			MACAddress() const;

	virtual	void*				AllocateSendReceiveBuffer(size_t size);
	virtual	void				FreeSendReceiveBuffer(void* buffer);

	virtual	ssize_t				Send(const void* buffer, size_t size);
	virtual	ssize_t				Receive(void* buffer, size_t size);

private:
	efi_simple_network_protocol*	fEFISimpleNetwork;
	mac_addr_t					fMACAddress;
};


#endif // EFI_NETWORK_H
