# Dependencies

This folder contains all the runtime and development dependencies for the UWP Vulkan sample.

## Runtime Dependencies (bin/)

- **vulkan-1.dll** - Standard Vulkan loader library
- **vulkan_dzn.dll** - Mesa3D DZN (Dozen) driver for Vulkan-to-DirectX12 translation (https://github.com/SternXD/mesa-uwp)
- **uwp_icd.x86_64.json** - Vulkan ICD configuration file that registers the DZN driver
- **SDL2.dll** - SDL2 library for cross-platform window management

## Development Dependencies

### Headers (include/)
- **SDL2/** - SDL2 development headers for window management and input
- **glad/** - GLAD OpenGL/Vulkan function loader headers
- **KHR/** - Khronos platform-specific headers

### Libraries (lib/)
- **SDL2.lib** - SDL2 import library for linking
- **vulkan_dzn.lib** - DZN driver import library

## Architecture

The DZN driver enables Vulkan API support on UWP by translating Vulkan calls to DirectX 12. This allows Vulkan applications to run on Xbox Series X|S and other UWP platforms that don't have native Vulkan drivers.