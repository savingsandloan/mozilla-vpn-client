//
// Created by Basti on 18/01/2023.
//

#ifndef MOZILLA_VPN_ANDROIDAUTOFILL_H
#define MOZILLA_VPN_ANDROIDAUTOFILL_H

#include <QObject>
#include "jni.h"

class AndroidAutofill : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(AndroidAutofill)
  public:
    AndroidAutofill();
    static AndroidAutofill* instance();


    /**
     * Will start a new Autofill session, and ask pw managers to get
     * credentials.
     * @param fxa - URL for FxA, should be the instance where the user want's to login c:
     */
    void startAutoFillSession(const QString fxa, const QString QPath);

    /**
     * Should be called when authentification was successfull,
     * this will cause password managers to ask to record the used password/username
     * @param username
     * @param password
     */
    void commit(const QString username, const QString password);
    /**
     * Should be called when auth was not successfull,
     * will clear end the Autofill Session.
     */
    void cancel();

    signals:
     void autofillAvailable(QString username, QString password);

   private:
    static void onAutofill(JNIEnv* env, jobject thiz, jstring data);

};

#endif  // MOZILLA_VPN_ANDROIDAUTOFILL_H
