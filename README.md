# DistortionVST

A simple Distortion VST using the JUCE framework and the Steinberg SDK.

<img width="377" alt="Capture" src="https://user-images.githubusercontent.com/38998582/145701453-17c89ccd-0599-44c7-aa5f-6dca9388b8b0.PNG">

And of course, because it's a VST you can easily import it into your editor of choice and apply more effects to the chain like Chorus and Reverb!

<img width="497" alt="VST chain" src="https://github.com/JasonThomasII/DistortionVST/assets/38998582/61f62550-107b-43cf-8ded-b599a8200b02">

## macOS Setup & Building

### Prerequisites

Before building this project, ensure you have the following installed:

1. **Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

2. **Xcode (Full IDE - optional but recommended)**
   - Download from the App Store or [Apple Developer website](https://developer.apple.com/download/)

3. **Projucer** (JUCE Project Manager)
   - Download from [juce.com](https://juce.com/download)
   - Required for modifying the project structure or regenerating Xcode projects

4. **JUCE Framework**
   - The project expects JUCE to be located at `~/Documents/JUCE`
   - Clone from [github.com/juce-framework/juce](https://github.com/juce-framework/juce)
   ```bash
   git clone https://github.com/juce-framework/juce.git ~/Documents/JUCE
   ```
   Or download the JUCE framework with Projucer.

### Building the Project

#### Quick Build (Command Line)

The easiest way to build is using the provided build script:

```bash
cd /path/to/DistortionVST

# Build Standalone Application
./build.sh debug                 # Debug version (default)
./build.sh release               # Release version (optimized)
./build.sh debug standalone      # Explicit standalone build
./build.sh release standalone    # Release standalone

# Build VST3 Plugin
./build.sh debug vst3            # VST3 debug build
./build.sh release vst3          # VST3 release build (optimized)
```

**Output locations:**
- Standalone: `./dist/DistortionVST.app`
- VST3 Plugin: `./dist/DistortionVST.vst3` (copy this to `~/Library/Audio/Plug-Ins/VST3/` to use in DAWs on MacOS)

#### Building in Xcode IDE

1. Open the project:
   ```bash
   open Builds/MacOSX/DistortionVST.xcodeproj
   ```

2. Select the "DistortionVST - Standalone Plugin" scheme from the dropdown

3. Press `Cmd + B` to build, or `Cmd + R` to build and run

#### Build with VS Code Tasks

If using VS Code, you can build using the configured tasks:

- Press `Cmd + Shift + P` and type "Tasks: Run Task" to see all available tasks:
  - "Build Standalone (Debug)" - for development
  - "Build Standalone (Release)" - for production/optimized build
  - "Clean Build" - to clean build artifacts

- Or press `Cmd + Shift + B` to run the default build task (Debug)

### Running the Application

#### Standalone Application

After building the standalone version, you can run the application in several ways:

##### From Command Line

```bash
open ./dist/DistortionVST.app
```

##### From Finder

Navigate to the `dist` folder and double-click `DistortionVST.app`

##### From Xcode

Build using the "DistortionVST - Standalone Plugin" scheme and press `Cmd + R`

#### VST3 Plugin

After building the VST3 version, you need to install it to use in your DAW:

1. Build the VST3 plugin:
   ```bash
   ./build.sh release vst3
   ```

2. Copy to the VST3 plugins directory:
   ```bash
   cp -r ./dist/DistortionVST.vst3 ~/Library/Audio/Plug-Ins/VST3/
   ```

3. Restart your DAW (Digital Audio Workstation) - it should now recognize the plugin

**Supported DAWs:** Any DAW that supports VST3 on macOS (Logic Pro, Ableton Live, Reaper, Cubase, etc.)

### Output Location

Built artifacts are placed in the `dist/` folder at the project root:

```
dist/
├── DistortionVST.app           # Standalone application
├── libDistortionVST.a          # Static library (used for linking)
└── ... (other build artifacts)
```

### Cleaning Build Artifacts

To remove all build artifacts:

```bash
rm -rf dist/
rm -rf build/
```

Or use the VS Code task: "Clean Build"

### Modifying the Project

If you need to modify the project structure or add new files:

1. Open the project in Projucer:
   ```bash
   /path/to/Projucer DistortionVST.jucer
   ```

2. Make your changes in Projucer

3. Export/regenerate the Xcode project

4. Rebuild using one of the methods above

### Troubleshooting

**Build fails with "scheme not found"**
- Ensure you've installed Xcode Command Line Tools: `xcode-select --install`
- The project expects the scheme "DistortionVST - Standalone Plugin"

**JUCE not found**
- Verify JUCE is installed at `~/Documents/JUCE`
- Update the path in the Xcode project if your JUCE installation is elsewhere

**Permission denied on build.sh**
- Make the script executable: `chmod +x build.sh`

**Deprecation warnings during build**
- These are expected and come from older JUCE APIs. The project still builds and runs correctly.