/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SENTRYADAPTER_H
#define SENTRYADAPTER_H

#include <QObject>
#include <QTimer>

#ifdef MZ_ANDROID
#  include "platforms/android/androidwatchdog.h"
#endif

/**
 * @brief Watchdog for the UI thread
 * If not kicked in time, it will report a Sentry Event
 *
 *
 */
class SentryWatchdog final : public QObject {
 public:
  explicit SentryWatchdog(QObject* parent);
  ~SentryWatchdog();

  /**
   * @brief Crates a new Platform Specific Sentry-Watchdog
   * If there is no platform implementation, will return
   * nullptr!
   *
   * @param parent - QObject-Parent
   * @return nullptr|SentryWatchdog*
   */
  static SentryWatchdog* create(QObject* parent) {
#ifdef MZ_ANDROID
    return new Sentry::AndroidWatchDog(parent);
#endif
    return nullptr;
  }

  void start(int anr_timeout_in_ms = 4000);
  void stop();

  slots:
  void kick();

 signals:
  void timeout();

 private:
  /**
   * @brief This function should dispatch the runnable to the UI Thread.
   * It's not important if that runnable is sync or async executed.
   *
   * @param runnable
   */
  virtual void runOnUIThread(std::function<void()> runnable);

  QTimer m_watchdogTimer;
  QPointer<QTimer> m_uiThreadTimer;
};
#endif  // SENTRYADAPTER_H
