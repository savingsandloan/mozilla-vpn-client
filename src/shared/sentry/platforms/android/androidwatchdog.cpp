/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "androidwatchdog.h"

#include <android/log.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <QObject>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <thread>

#include "../../../platforms/android/androidcommons.h"

struct TimerData {
  timer_t timerId;
  std::function<void()> callback;
  int timeout;
};
void timerCallback(union sigval sv) {
  TimerData* timerData = reinterpret_cast<TimerData*>(sv.sival_ptr);
  timerData->callback();
}

void AndroidWatchDog::startWorker(int kick_timeout) {
  __android_log_print(ANDROID_LOG_INFO, "TimerWorker", "Timer Starting?");
  TimerData* timerData = new TimerData();
  timerData->callback = [&]() {
    __android_log_print(ANDROID_LOG_INFO, "TimerWorker", "Timer expired!");
    kick();
  };
  timerData->timeout = kick_timeout;

  struct sigevent sev;
  memset(&sev, 0, sizeof(sev));
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_notify_function = timerCallback;
  sev.sigev_value.sival_ptr = timerData;

  if (timer_create(CLOCK_REALTIME, &sev, &timerData->timerId) == 0) {
    struct itimerspec its;
    its.it_value.tv_sec = kick_timeout;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = kick_timeout;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerData->timerId, 0, &its, nullptr) != 0) {
      __android_log_print(ANDROID_LOG_ERROR, "TimerWorker",
                          "Failed to set timer: %s", strerror(errno));
    }
  } else {
    __android_log_print(ANDROID_LOG_ERROR, "TimerWorker",
                        "Failed to create timer: %s", strerror(errno));
  }
}
