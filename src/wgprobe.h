/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WG_PROBE_H
#define WG_PROBE_H

#include <QObject>
#include "controller.h"

class WgProbe : QObject{
  Q_DISABLE_COPY_MOVE(WgProbe)

 public:
   
   static void onHandshakeFailed(const HopConnection& hop);

   static bool probeHop(const HopConnection& hop);

   static constexpr auto BACKOFF_TIME = 300;
};

#endif  // WG_PROBE_H
