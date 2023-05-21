#! /usr/bin/env python3
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys
import os.path
import subprocess
import argparse

parser = argparse.ArgumentParser(
    description='Generate Glean telemetry definitions from YAML sources')
parser.add_argument('-o', '--output', metavar='DIR', type=str, action='store',
    default=os.path.join(os.getcwd(), 'glean', 'telemetry'),
    help='Output directory for generated files')
args = parser.parse_args()


def camelize(string):
    output = ''
    first = True
    for chunk in string.split('_'):
        if first:
            output += chunk
            first = False
        else:
            output += chunk[0].upper()
            output += chunk[1:]
    return output


## Find the input Yaml files relative to the script location.
rootpath = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))

pings_files = []
metrics_files = []
for project in os.listdir(os.path.join(rootpath, "src", "apps")):
    telemetry_path = os.path.join(rootpath, "src", "apps", project, "telemetry")
    if os.path.isdir(telemetry_path):
        pings_file = os.path.join(telemetry_path, "pings_deprecated.yaml")
        if os.path.isfile(pings_file):
            pings_files.append(pings_file)

        metrics_file = os.path.join(telemetry_path, "metrics_deprecated.yaml")
        if os.path.isfile(metrics_file):
            metrics_files.append(metrics_file)

telemetry_path = os.path.join(rootpath, "src", "shared", "telemetry")
if os.path.isdir(telemetry_path):
    pings_file = os.path.join(telemetry_path, "pings_deprecated.yaml")
    if os.path.isfile(pings_file):
        pings_files.append(pings_file)

    metrics_file = os.path.join(telemetry_path, "metrics_deprecated.yaml")
    if os.path.isfile(metrics_file):
        metrics_files.append(metrics_file)

if not metrics_files:
    exit("No metrics found!")

if not pings_files:
    exit("No pings found!")

## Create the output directory if it doesn't already exist.
os.makedirs(args.output, exist_ok=True)

print("Generating the QRC file...")
output = open(os.path.join(args.output, "telemetry.qrc"), "w")
output.write("""<!-- AUTOGENERATED! DO NOT EDIT -->
<RCC>
    <qresource prefix="/glean/telemetry">
        <file>pings.js</file>
        <file>sample.js</file>
        <file>qmldir</file>
    </qresource>
</RCC>
""")
output.close()

print("Generating the JS modules...")
try:
    subprocess.call([sys.executable, "-m", "glean_parser", "translate"] + metrics_files + pings_files +
                    ["-f", "javascript", "-o", args.output, "--option", "platform=qt",
                    "--option", "version=0.30"])
except Exception as e:
    print("glean_parser failed. Is it installed? Try with:\n\tpip3 install -r requirements.txt --user")
    print(e)
    exit(1)
