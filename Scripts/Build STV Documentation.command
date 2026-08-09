#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCES_DIR="$ROOT_DIR/Sources"
DOCUMENTATION_DIR="$ROOT_DIR/Documentation"
DOCC_CATALOG="$DOCUMENTATION_DIR/STV.docc"
BUILD_DIR="$ROOT_DIR/.build/documentation"
SYMBOL_GRAPH_DIR="$BUILD_DIR/symbol-graphs"
ARCHIVE_PATH="$DOCUMENTATION_DIR/STV.doccarchive"

mkdir -p "$DOCUMENTATION_DIR" "$SYMBOL_GRAPH_DIR"

if [ -e "$ARCHIVE_PATH" ]; then
    /usr/bin/trash "$ARCHIVE_PATH"
fi

SDKROOT="$(/usr/bin/xcrun --sdk iphoneos --show-sdk-path)"

/usr/bin/xcrun clang \
    -extract-api \
    --product-name=STV \
    -x objective-c-header \
    -target arm64-apple-ios16.0 \
    -isysroot "$SDKROOT" \
    -fmodules \
    -fobjc-arc \
    -I "$SOURCES_DIR" \
    -D SWIFT_PACKAGE=1 \
    "$SOURCES_DIR"/*.h \
    -o "$SYMBOL_GRAPH_DIR/STV.symbols.json"

/usr/bin/xcrun docc convert "$DOCC_CATALOG" \
    --additional-symbol-graph-dir "$SYMBOL_GRAPH_DIR" \
    --fallback-display-name STV \
    --fallback-bundle-identifier ie.dgapps.STV \
    --fallback-default-module-kind Framework \
    --platform 'name=iOS,version=16.0' \
    --emit-lmdb-index \
    --output-path "$ARCHIVE_PATH" \
    --warnings-as-errors \
    --analyze \
    --experimental-documentation-coverage \
    --coverage-summary-level brief

printf 'Built %s\n' "$ARCHIVE_PATH"
