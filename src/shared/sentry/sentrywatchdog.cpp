/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sentrywatchdog.h"

SentryWatchdog::SentryWatchdog(QObject* parent) : QObject(parent) {
  connect(m_watchDogTimer, &QTimer::timeout, this, [&]() {
    stop();
    emit timeout();
  });
  m_watchDogTimer.setSingleShot(true);
}

SentryWatchdog::~SentryWatchdog() { stop(); }

SentryWatchdog::start(int anr_timeout_in_ms) {
  int kick_timeout = anr_timeout_in_ms / 4;

  if (!m_uiThreadTimer.isNull()) {
    m_uiThreadTimer->deleteLater();
  }

  runOnUIThread([&]() {
    auto uiThreadTimer = new QTimer();
    connect(uiThreadTimer, &QTimer::timeout, this, &SentryWatchdog::kick,
            Qt::QueuedConnection);
    m_uiThreadTimer = uiThreadTimer;
    uiThreadTimer.start(kick_timeout);
  });
}

SentryWatchdog::stop() {
  m_watchDogTimer.stop();
  if (!m_uiThreadTimer.isNull()) {
    m_uiThreadTimer->deleteLater();
  }
}

SentryWatchdog::kick() {
  auto interval = m_watchDogTimer.interval();
  if (!m_watchDogTimer.isActive()) {
    // In case we have already fired timeout, due to
    // late response, ignore the kick.
    return;
  }
  if (interval == 0) {
    // We should not have kick
    Q_ASSERT(false);
    return;
  }
  // start(), will reset the timer
  m_watchDogTimer.start(interval);
}
