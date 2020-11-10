/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMMANDSELECT_H
#define COMMANDSELECT_H

#include "command.h"

class ServerData;

class CommandSelect : public Command
{
public:
    CommandSelect();

    int run(QStringList &tokens) override;

private:
    bool pickServer(const QString &hostname, ServerData &serverData);
};

#endif // COMMANDSELECT_H
