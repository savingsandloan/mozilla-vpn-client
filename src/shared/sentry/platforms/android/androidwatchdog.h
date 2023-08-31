/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ANDROID_WATCHDOG_H
#define ANDROID_WATCHDOG_H

#include <QObject>

#include "../../../platforms/android/androidcommons.h"
#include "../../sentrywatchdog.h"

namespace Sentry {
class AndroidWatchDog final : public SentryWatchdog {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(AndroidWatchDog)

  AndroidWatchDog(QObject* parent) : SentryWatchdog(parent) {}
  AndroidWatchDog::~AndroidWatchDog(){};

  override void runOnUIThread(std::function<void()> runnable) {
    AndroidCommons::runOnAndroidThreadSync(runnable);
  }
};
}  // namespace Sentry
#endif  // ANDROID_WATCHDOG_H
