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
    -scheme "DistortionVST - Standalone Plugin" \
    -configuration "${CONFIG}" \
    build \
    CONFIGURATION_BUILD_DIR="${PROJECT_DIR}/dist"

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    PRODUCT_PATH="${PROJECT_DIR}/dist/DistortionVST.app"
    if [ -d "${PRODUCT_PATH}" ]; then
        echo "Output: ${PRODUCT_PATH}"
    else
        ## If the app is not found, list contents of dist for debugging
        echo "Build completed but app not found at expected location"
        echo "Checking dist folder..."
        ls -la "${PROJECT_DIR}/dist/" 2>/dev/null || echo "Dist folder empty"
    fi
else
    echo "✗ Build failed!"
    exit 1
fi
