/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AndroidVersionAPI.h"

#include <QJniEnvironment>
#include <QJniObject>

#include "androidutils.h"
#include "glean/generated/metrics.h"
#include "gleandeprecated.h"
#include "jni.h"
#include "leakdetector.h"
#include "logger.h"
#include "task.h"
#include "telemetry/gleansample.h"
#include "urlopener.h"

namespace {
Logger logger("AndroidVersionAPI");
AndroidVersionAPI* s_instance = nullptr;
constexpr auto CLASSNAME = "org.mozilla.firefox.vpn.qt.VPNVersionAPI";
}  // namespace

AndroidVersionAPI* AndroidVersionAPI::Instance() {
  if (s_instance == nullptr) {
    s_instance = new AndroidVersionAPI();
  }
  return s_instance;
}

AndroidVersionAPI::AndroidVersionAPI() {
  MZ_COUNT_CTOR(AndroidVersionAPI);

  // Hook together implementations for functions called by native code
  AndroidUtils::runOnAndroidThreadSync([]() {
    JNINativeMethod methods[]{{"onUpdateResult", "(Ljava/lang/String;)V",
                               reinterpret_cast<void*>(onJvmUpdateResult)}};
    QJniEnvironment env;
    jclass objectClass = env.findClass(CLASSNAME);
    if (objectClass == nullptr) {
      return;
    }
    env->RegisterNatives(objectClass, methods,
                         sizeof(methods) / sizeof(methods[0]));
  });
}

AndroidVersionAPI::~AndroidVersionAPI() { MZ_COUNT_DTOR(AndroidVersionAPI); }

void AndroidVersionAPI::requestUpdateInfo() {
  QJniObject::callStaticMethod<void>(CLASSNAME, "startUpdateInfo", "()V");
}

void AndroidVersionAPI::onJvmUpdateResult(JNIEnv* env, jobject thiz,
                                          jstring data) {
  auto json_string = AndroidUtils::getQStringFromJString(env, data);
  emit AndroidVersionAPI::Instance()->onUpdateResult(json_string);
}
