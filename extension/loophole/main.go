package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"math/rand"
	"strconv"
	"strings"
)

import (
	"net"
	"sync"
	"tailscale.com/net/socks5"
)

/*
*
* Small Socks5 Proxy trying it's best to route traffic to the
* real adapter, bypassing the vpn
*
*  usage: loophole.exe -auth -p 8080
*  -auth - Will generate a new user and password for this session,
*    Basic Auth using those credentials will be enforced
*  -p <number> Will try to bind itself to the given port, otherwise a random will be send.
*
*
* In Case of successfull start it will print on stdout everything needed to connect to it
* i.e  $: {"Type":"ready","Username":"","Password":"","Port":56860}
*
*
 */
func main() {
	var port = flag.Int("p", getRandomPort(), "Preferred Port")
	var auth = flag.Bool("auth", false, "Require Auth")
	flag.Parse()

	ln, err := net.Listen("tcp", "localhost:"+strconv.Itoa(*port))
	if err != nil {
		MessageError{
			Message: "Failed to listen on" + strconv.Itoa(*port),
			Code:    2,
		}.print()

		return
	}

	// The Dialer is responsible for
	// setting up any outgoing connection.
	// The Impl is different for each OS
	dial := getDialer()
	// Optionally set a random user and password
	var username, password string
	if *auth {
		username, password = randString(10), randString(10)
	}

	// Create the socks5 Server
	server := socks5.Server{
		Dialer:   dial.DialContext, // TODO: maybe use an interface, given dial might be custom.
		Username: username,
		Password: password,
	}

	// Run the Serve function in a goroutine
	// so that we can continue to do i/o
	wg := sync.WaitGroup{}
	go func() {
		wg.Add(1)
		err = server.Serve(ln)
		if err != nil {
			MessageError{
				Message: "Failed to create the Proxy",
				Code:    3,
			}.print()

		}
		wg.Done()
	}()

	MessageReady{
		Username: username,
		Password: password,
		Port:     *port,
	}.print()
	wg.Wait()
}

// MessageReady - JSON Message that will be sent whenever we are ready to
// accept connections
type MessageReady struct {
	Type     string
	Username string
	Password string
	Port     int
}

func (r MessageReady) print() {
	r.Type = "ready"
	b, err := json.Marshal(r)
	if err != nil {
		return
	}
	fmt.Println(string(b))
}

// MessageError - JSON Message that will be sent whenever we are ready to
// accept connections
type MessageError struct {
	Type    string // Optional
	Code    int    // Error Code
	Message string
}

func (r MessageError) print() {
	r.Type = "error"
	b, err := json.Marshal(r)
	if err != nil {
		return
	}
	fmt.Println(string(b))
}

func getRandomPort() int {
	return 49152 + rand.Intn(65535-49152)
}

const possibleLetters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

func randString(n int) string {
	buffer := strings.Builder{}
	buffer.Grow(n)
	for i := 0; i < n; i++ {
		index := rand.Intn(len(possibleLetters))
		buffer.WriteByte(possibleLetters[index])
	}
	return buffer.String()
}
