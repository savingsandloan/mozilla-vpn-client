/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LINUXUTILS_H
#define LINUXUTILS_H

#include <QObject>

class LinuxUtils final {
 public:
  static QString findCgroupPath(const QString& type);
  static QString findCgroup2Path();
  static QString gnomeShellVersion();
  static QString kdeFrameworkVersion();

 private:
  LinuxUtils() = default;
  ~LinuxUtils() = default;

  Q_DISABLE_COPY(LinuxUtils)
};

#endif  // LINUXUTILS_H
