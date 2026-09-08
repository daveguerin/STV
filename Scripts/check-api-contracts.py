#!/usr/bin/env python3
"""Compile STV's public headers and a Swift client without editing app projects.

Checks the real exported umbrella module plus standalone public-header imports.
Temporary compiler modules belong to a temporary directory, never Sources.
"""
import argparse
from pathlib import Path
import platform
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk', choices=['iphonesimulator', 'iphoneos'],
                        default='iphonesimulator')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    sources = root / 'Sources'
    sdk = subprocess.check_output(
        ['xcrun', '--sdk', args.sdk, '--show-sdk-path'], text=True).strip()
    simulator = args.sdk == 'iphonesimulator'
    arch = platform.machine() if simulator else 'arm64'
    target = f'{arch}-apple-ios16.0' + ('-simulator' if simulator else '')
    with tempfile.TemporaryDirectory(prefix='STVAPIContracts-') as temporary:
        work = Path(temporary)
        modulemap = work / 'module.modulemap'
        modulemap.write_text(
            f'module STV {{ umbrella header "{sources / "SensibleTableView.h"}" '
            'export * }\n')
        compiler = ['xcrun', 'clang', '-target', target, '-isysroot', sdk,
                    '-fobjc-arc', '-fmodules', '-fsyntax-only',
                    '-Werror=nullability-completeness', '-Werror=nonnull',
                    '-Werror=deprecated-declarations',
                    '-Werror=incompatible-pointer-types',
                    '-I', str(sources)]
        headers = sorted(sources.glob('*.h'))
        for header in headers:
            client = work / 'HeaderClient.m'
            client.write_text(f'#import "{header.name}"\n')
            subprocess.run(compiler + [str(client)], check=True)
        # Missing expected diagnostics are errors: removing audited contracts
        # must not silently turn this check green.
        consumers = sorted((root / 'Scripts/APIContracts').glob('*.m'))
        for consumer in consumers:
            subprocess.run(compiler + ['-Wno-error=nonnull',
                           '-Wno-error=incompatible-pointer-types',
                           '-Wnullable-to-nonnull-conversion',
                           '-Xclang', '-verify', str(consumer)], check=True)
        subprocess.run(['xcrun', 'swiftc', '-typecheck', '-warnings-as-errors',
                        '-target', target, '-sdk', sdk, '-I', str(work),
                        str(root / 'Scripts/APIContracts/SwiftConsumer.swift')],
                       check=True)
        print(f'PASS: {len(headers)} standalone public headers, {len(consumers)} diagnostic consumers, '
              f'and Swift consumer ({args.sdk})')


if __name__ == '__main__':
    main()
