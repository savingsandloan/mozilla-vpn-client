/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "extensionhandler.h"

#include <QHostAddress>
#include <QTcpSocket>

#include "leakdetector.h"
#include "logger.h"
#include "extensionconnection.h"

namespace {
Logger logger("ExtensionHandler");
}

constexpr int SERVER_PORT = 8754;

ExtensionHandler::ExtensionHandler() {
  MZ_COUNT_CTOR(ExtensionHandler);

  logger.debug() << "Creating the server";

  if (!listen(QHostAddress::LocalHost, SERVER_PORT)) {
    logger.error() << "Failed to listen on port" << SERVER_PORT;
    return;
  }

  connect(this, &ExtensionHandler::newConnection, this,
          &ExtensionHandler::newConnectionReceived);
}

ExtensionHandler::~ExtensionHandler() { MZ_COUNT_DTOR(ExtensionHandler); }

void ExtensionHandler::newConnectionReceived() {
  QTcpSocket* child = nextPendingConnection();
  Q_ASSERT(child);

  ExtensionConnection* connection = new ExtensionConnection(this, child);
  connect(child, &QTcpSocket::disconnected, connection, &QObject::deleteLater);
}
