/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AndroidVersionAPI_H
#define AndroidVersionAPI_H

#include <QObject>

#include "jni.h"

/**
 * @brief This is a Companion Class for VPNVersionAPI.kt
 *
 * It exposes a function to async query Google-Play to get new
 * updates.
 * On Completion it signals {onUpdateResult}
 *
 */
class AndroidVersionAPI final : public QObject {
  Q_DISABLE_COPY_MOVE(AndroidVersionAPI)
  Q_OBJECT
 public:
  ~AndroidVersionAPI();

  static AndroidVersionAPI* Instance();
  /**
   * @brief Requests a new update info task
   * completion is signalled via the {onUpdateResult} signal.
   */
  void requestUpdateInfo();

 signals:
  /*
   * @brief Signal Fired once a google play result is recieved
   *
   * @param data - Depending on the outcome of the task:
   * OK -> A JSON String representing a VPNVersionAPI.UpdateResult
   * Err -> An empty string
   */
  void onUpdateResult(QString data);

 private:
  AndroidVersionAPI();
  static void onJvmUpdateResult(JNIEnv* env, jobject thiz, jstring data);
};

#endif  // AndroidVersionAPI_H
