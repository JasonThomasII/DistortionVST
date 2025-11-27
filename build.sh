#!/bin/bash
# Simple build script for DistortionVST
# Usage: ./build.sh [Debug|Release]

CONFIG="${1:-Debug}"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
XCODE_PROJECT="${PROJECT_DIR}/Builds/MacOSX/DistortionVST.xcodeproj"

echo "Building DistortionVST (${CONFIG})..."
echo "Project: ${XCODE_PROJECT}"

# Check if xcodebuild is available
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: xcodebuild not found. Please install Xcode Command Line Tools."
    echo "Run: xcode-select --install"
    exit 1
fi

# Build using xcodebuild
xcodebuild \
    -project "${XCODE_PROJECT}" \
    -scheme "DistortionVST - Standalone" \
    -configuration "${CONFIG}" \
    -derivedDataPath "${PROJECT_DIR}/build/derived_data" \
    build

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    PRODUCT_PATH="${PROJECT_DIR}/build/derived_data/Build/Products/${CONFIG}/DistortionVST.app"
    echo "Output: ${PRODUCT_PATH}"
else
    echo "✗ Build failed!"
    exit 1
fi
