#!/bin/bash
# Build script for DistortionVST
# Usage: ./build.sh [debug|release] [standalone|vst3]
# Examples:
#   ./build.sh debug standalone
#   ./build.sh release vst3
#   ./build.sh debug  (defaults to standalone)

CONFIG="${1:-debug}"
TARGET="${2:-standalone}"

# Normalize config to match Xcode expectations
if [[ "$CONFIG" == "debug" ]]; then
    CONFIG="Debug"
elif [[ "$CONFIG" == "release" ]]; then
    CONFIG="Release"
fi

# Select the appropriate scheme
case "$TARGET" in
    standalone)
        SCHEME="DistortionVST - Standalone Plugin"
        OUTPUT_NAME="DistortionVST.app"
        ;;
    vst3)
        SCHEME="DistortionVST - VST3"
        OUTPUT_NAME="DistortionVST.vst3"
        ;;
    *)
        echo "Error: Unknown target '$TARGET'"
        echo "Valid targets: standalone, vst3"
        exit 1
        ;;
esac

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
XCODE_PROJECT="${PROJECT_DIR}/Builds/MacOSX/DistortionVST.xcodeproj"

echo "Building DistortionVST - $TARGET ($CONFIG)..."
echo "Scheme: $SCHEME"
echo "Project: $XCODE_PROJECT"

# Check if xcodebuild is available
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: xcodebuild not found. Please install Xcode Command Line Tools."
    echo "Run: xcode-select --install"
    exit 1
fi

# Build using xcodebuild
xcodebuild \
    -project "${XCODE_PROJECT}" \
    -scheme "$SCHEME" \
    -configuration "$CONFIG" \
    build \
    CONFIGURATION_BUILD_DIR="${PROJECT_DIR}/dist"

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    PRODUCT_PATH="${PROJECT_DIR}/dist/${OUTPUT_NAME}"
    if [ -d "${PRODUCT_PATH}" ] || [ -f "${PRODUCT_PATH}" ]; then
        echo "Output: ${PRODUCT_PATH}"
    else
        echo "Build completed but output not found at expected location"
        echo "Checking dist folder..."
        ls -la "${PROJECT_DIR}/dist/" 2>/dev/null || echo "Dist folder empty"
    fi
else
    echo "✗ Build failed!"
    exit 1
fi
