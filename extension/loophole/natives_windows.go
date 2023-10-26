package main

import (
	"net"
)

/**
* Does not create a specific dial context on windows
* the vpn client will use split tunneling to make sure the
* dialer is using the "real" network device :)
**/
func getDialer() net.Dialer {
	return net.Dialer{}
}
