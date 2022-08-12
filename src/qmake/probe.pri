# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

macos|win* {
    win* {
      LIBNAME=probe.lib
    } else {
      LIBNAME=libprobe.a
    }

    probeCrate.input = PROBE_CRATE
    probeCrate.output = ${QMAKE_FILE_IN}/target/x86_64-apple-darwin/release/$$LIBNAME
    probeCrate.commands = @echo Building probe rust create staticlib \
        && cd ${QMAKE_FILE_IN} \
        && cargo build --release --target x86_64-apple-darwin
    probeCrate.clean = ${QMAKE_FILE_OUT}
    probeCrate.CONFIG = target_predeps
    probeCrate.depends += ${QMAKE_FILE_IN}/Cargo.toml ${QMAKE_FILE_IN}/src/lib.rs
    probeCrate.variable_out = LIBS

    QMAKE_EXTRA_COMPILERS += probeCrate
    PROBE_CRATE = $$PWD/../../probe
    DEFINES += MVPN_WIREGUARD_PROBE

    
    HEADERS += $$PWD/../../probe/probe.h

    HEADERS += wgprobe.h
    SOURCES += wgprobe.cpp

}

