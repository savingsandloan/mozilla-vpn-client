//
// Created by Basti on 18/01/2023.
//

#include "AndroidAutofill.h"

#include "authenticationinapp/authenticationinapp.h"
#include "androidutils.h"

#include "shared/constants.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


namespace{
  AndroidAutofill* s_instance = nullptr;
  constexpr auto CLASS_NAME = "org/mozilla/firefox/vpn/qt/QtAutofillHelper";
}


AndroidAutofill* AndroidAutofill::instance() {
  if(!s_instance){
    s_instance = new AndroidAutofill();
  }
  return s_instance;
}

AndroidAutofill::AndroidAutofill() {
  AndroidUtils::runOnAndroidThreadSync([]() {
    JNINativeMethod methods[]{
        {"onAutofill", "(Ljava/lang/String;)V",
         reinterpret_cast<void*>(onAutofill)}
    };
    QJniEnvironment env;
    jclass objectClass = env.findClass(CLASS_NAME);
    env->RegisterNatives(objectClass, methods,
                         sizeof(methods) / sizeof(methods[0]));
  });


  connect(AuthenticationInApp::instance(), &AuthenticationInApp::stateChanged,
          this, [this](){
            auto state = AuthenticationInApp::instance()->state() ;
            if(state == AuthenticationInApp::StateStart ){
              startAutoFillSession(Constants::fxaUrl());
            }
          });
  connect(AuthenticationInApp::instance(), &AuthenticationInApp::completed, this,
          [this](){
            commit("sstreich@example.com","test_test");
          });

}

void AndroidAutofill::cancel() {
  QJniObject::callStaticMethod<void>(
      CLASS_NAME,
      "cancel",
      "()V"
  );
}

void AndroidAutofill::commit(const QString username, const QString password) {
  auto jUser = QJniObject::fromString(username);

  auto jPassword = QJniObject::fromString(password);
  QJniObject::callStaticMethod<void>(
        CLASS_NAME,
        "commit",
        "(Ljava/lang/String;Ljava/lang/String;)V",
        jUser.object(),
        jPassword.object()
      );
}

void AndroidAutofill::startAutoFillSession(const QString fxa) {
  QJsonObject params;
  params["fxa_url"]=fxa;
  QJsonDocument doc(params);
  auto string = doc.toJson(QJsonDocument::Compact);
  auto jniString = QJniObject::fromString(string);
  QJniObject::callStaticMethod<void>(
      CLASS_NAME,
      "start", "(Ljava/lang/String;)V",
      jniString.object());
}

void AndroidAutofill::onAutofill(JNIEnv* env, jobject thiz, jstring data) {
  Q_UNUSED(thiz);
  QJsonObject autoFill =
      AndroidUtils::getQJsonObjectFromJString(env, data);

  if(autoFill.isEmpty()){
    return;
  }
  auto password = autoFill["password"];
  auto username = autoFill["username"];

  if(!password.isString()){
    return;
  }
  if(!username.isString()){
    return;
  }
  auto auth = AuthenticationInApp::instance();

  switch(auth->state()){
    case AuthenticationInApp::StateSignUp:
      // Set the Password and continue
      auth->setPassword(password.toString());
      auth->signUp();
      break;
    case AuthenticationInApp::StateSignIn:
      // Set the Password and continue
      auth->setPassword(password.toString());
      auth->signIn();
      break;
    case AuthenticationInApp::StateStart:
      auth->checkAccount(username.toString());
      //auth->setPassword(password.toString()); TODO: make this possible.
      break;
    default:
      // This should not happen ?
      // If we get an autofill response when the app is in a non
      // fillable state, let's ignore it. c:
      break;
  }
}