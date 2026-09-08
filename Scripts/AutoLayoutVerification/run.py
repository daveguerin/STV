#!/usr/bin/env python3
"""Build and launch an isolated, real UIKit consumer of the local STV package.

This is a rendering fixture, not SetList and not an automated UI-test verdict.
All output stays in a temporary directory; no app project is modified.
"""
import argparse
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile


def run(command, **kwargs):
    print("+ " + " ".join(str(arg) for arg in command), flush=True)
    subprocess.run([str(arg) for arg in command], check=True, **kwargs)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--device", required=True, help="Available iOS Simulator UDID")
    parser.add_argument("--mode", choices=["normal", "keyboard", "date", "embedded-date"],
                        default="normal")
    parser.add_argument("--no-sidebar", action="store_true")
    parser.add_argument("--reuse-build", action="store_true",
                        help="Reuse this fixture's existing STV build; omit after source edits")
    args = parser.parse_args()
    fixture = Path(__file__).resolve().parent
    package = fixture.parent.parent
    output = Path(tempfile.gettempdir()) / "STVAutoLayoutVerification"
    derived = output / "DerivedData"
    app = output / "STVLayoutDemo.app"
    app.mkdir(parents=True, exist_ok=True)
    if not args.reuse_build:
        with (output / "build.log").open("w") as log:
            run(["xcodebuild", "-scheme", "STV", "-configuration", "Debug",
                 "-destination", "generic/platform=iOS Simulator",
                 "-derivedDataPath", derived, "build"], cwd=package,
                stdout=log, stderr=subprocess.STDOUT)
    object_file = derived / "Build/Products/Debug-iphonesimulator/STV.o"
    if not object_file.is_file():
        parser.error("No built STV object. Run without --reuse-build.")
    sdk = subprocess.check_output(
        ["xcrun", "--sdk", "iphonesimulator", "--show-sdk-path"], text=True).strip()
    architecture = platform.machine()
    if architecture not in {"arm64", "x86_64"}:
        parser.error(f"Unsupported simulator host architecture: {architecture}")
    shutil.copy2(fixture / "Info.plist", app / "Info.plist")
    run(["xcrun", "clang", "-target", f"{architecture}-apple-ios16.0-simulator",
         "-isysroot", sdk, "-fobjc-arc", "-fmodules", "-I", package / "Sources",
         fixture / "main.m", object_file, "-o", app / "STVLayoutDemo",
         "-framework", "UIKit", "-framework", "Foundation", "-framework", "CoreData",
         "-framework", "QuartzCore", "-framework", "CoreGraphics"])
    run(["codesign", "--force", "--sign", "-", app])
    run(["xcrun", "simctl", "bootstatus", args.device, "-b"])
    run(["xcrun", "simctl", "install", args.device, app])
    launch = ["xcrun", "simctl", "launch", "--terminate-running-process", args.device,
              "ie.dgapps.STVLayoutVerification"]
    if args.mode != "normal":
        launch.append("--" + args.mode)
    if args.no_sidebar:
        launch.append("--no-sidebar")
    run(launch)
    print(f"Launched fixture: {app}\nBuild log: {output / 'build.log'}")
    print("Inspect Simulator after the requested editor appears; capture with simctl io screenshot.")


if __name__ == "__main__":
    main()
