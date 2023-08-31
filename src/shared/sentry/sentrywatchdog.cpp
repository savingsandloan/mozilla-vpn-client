/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sentrywatchdog.h"

#include <QDateTime>

#ifdef MZ_ANDROID
#  include "platforms/android/androidwatchdog.h"
#endif

namespace {
qint64 watchdog_kick = 6;

}  // namespace

SentryWatchdog::SentryWatchdog(QObject* parent) : QObject(parent) {
  m_watchdogTimer.setSingleShot(true);
}

SentryWatchdog::~SentryWatchdog() { stop(); }

void SentryWatchdog::start(int anr_timeout_in_ms) {
  int kick_timeout = anr_timeout_in_ms / 4;

  startWorker(kick_timeout);
  m_watchdogTimer.start(anr_timeout_in_ms);
}

void SentryWatchdog::stop() {
  m_watchdogTimer.stop();
  stopWorker();
}

void SentryWatchdog::kick() {
  if (!m_watchdogTimer.isActive()) {
    // In case we have already fired timeout, due to
    // late response, ignore the kick.
    return;
  }
  auto interval = m_watchdogTimer.interval();
  if (interval == 0) {
    // We should not have kick
    Q_ASSERT(false);
    return;
  }
  watchdog_kick = QDateTime::currentMSecsSinceEpoch();
  // start(), will reset the timer
  m_watchdogTimer.start(interval);
}

SentryWatchdog* SentryWatchdog::create(QObject* parent) {
#ifdef MZ_ANDROID
  return new AndroidWatchDog(parent);
#endif
  return nullptr;
}
