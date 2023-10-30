/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ExtensionHandler_H
#define ExtensionHandler_H

#include <QTcpServer>

class ExtensionHandler final : public QTcpServer {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(ExtensionHandler)

 public:
  ExtensionHandler();
  ~ExtensionHandler();

 private:
  void newConnectionReceived();
};

#endif  // ExtensionHandler_H
