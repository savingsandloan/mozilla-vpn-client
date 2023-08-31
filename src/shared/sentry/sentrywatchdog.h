/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SentryWatchdog_H
#define SentryWatchdog_H

#include <QObject>
#include <QPointer>
#include <QTimer>

/**
 * @brief Watchdog for the UI thread
 * If not kicked in time, it will report a Sentry Event
 *
 *
 */
class SentryWatchdog : public QObject {
  Q_OBJECT
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
  static SentryWatchdog* create(QObject* parent);
  void start(int anr_timeout_in_ms = 4000);
  void stop();

  Q_SLOT void kick();
  Q_SIGNAL void timeout(qint64 last_seen_timestamp);

 private:
  /**
   * @brief This function should dispatch a runnable to the UI Thread.
   * It must call kick() every {kick_timeout}ms.
   *
   * @param kick_timeout_in_ms
   */
  virtual void startWorker(int kick_timeout_in_ms) = 0;
  // Should stop the worker
  virtual void stopWorker() = 0;

  QTimer m_watchdogTimer;
};
#endif  // SentryWatchdog_H
