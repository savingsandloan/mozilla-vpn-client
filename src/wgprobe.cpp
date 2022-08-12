/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "wgprobe.h"
#include "probe.h"
#include "logger.h"
#include "mozillavpn.h"



namespace {
Logger logger(LOG_MAIN ,"wg-probe");
}  // namespace

/**
 * @brief Attempts to do a wireguard handshake with a given port - returns whether a handshake was established
 * 
 * @param hop The Hop to Check
 * @return true A handshake was sucessfully executed 
 * @return false The handshake failed. Either no response from the server or an error happened.
 */
bool WgProbe::probeHop(const HopConnection& hop){

  QByteArray address = QString("%0:%1").arg(hop.m_server.ipv4AddrIn()).arg(hop.m_server.choosePort()).toLocal8Bit();

  logger.debug() << address;

  QByteArray server_key = hop.m_server.publicKey().toLocal8Bit();
  QByteArray client_key = MozillaVPN::instance()->keys()->privateKey().toLocal8Bit();
  logger.debug() << "Starting to test handshake to ->" << hop.m_server.ipv4AddrIn();
  auto result = probe_wireguard_connection(address.data(),server_key.data(),client_key.data());

  switch (result)
  {
  case ProbeResult::Ok:
    logger.debug() << "Handshake to " <<hop.m_server.hostname() << " was success";
    break;
  case ProbeResult::HandshakeFailure:
          logger.debug() << "Handshake to " <<hop.m_server.hostname() << " failed";
          return false;
  case ProbeResult::NoResponse:
    logger.error() << "Handshake to " <<hop.m_server.hostname() << " did not get a response from the server";
    break;
  case ProbeResult::BoringtunError:
    logger.error() << "Handshake to " <<hop.m_server.hostname() << " yielded in a wireguard-error";
    break;
  case ProbeResult::FFIError:
    logger.error() << "Handshake to " <<hop.m_server.hostname() << " yielded in a ffi-error";
    break;
  case ProbeResult::NetworkError:
    logger.error() << "Handshake to " <<hop.m_server.hostname() <<  " yielded in a network-error";
    break;
  case ProbeResult::KeyError:
    logger.error() << "Handshake to " <<hop.m_server.hostname() <<  " yielded in a network-key";
    break;
  case ProbeResult::UnexpectedBoringtunState:
    logger.error() << "Handshake to " <<hop.m_server.hostname() <<  " yielded in a wierd boring tun state";
    // Still if the state is wierd - if boring tun i-e want's us to send stuff into the local network
    // That reqiures that we have passed the handshake. Meaning all still good.
    return true;  
  }
  return result == ProbeResult::Ok;
}
