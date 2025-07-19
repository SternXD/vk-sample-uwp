## UWP Vulkan Sample

This is a basic sample demonstrating Vulkan graphics API working on UWP (Universal Windows Platform) via Mesa3D's DZN driver. The DZN driver translates Vulkan API calls to DirectX 12, enabling Vulkan applications to run on UWP platforms including Xbox Series X|S.

## Architecture

The project uses the following components:
- **SDL2** - For basic window management (UWP-compatible)
- **vulkan-1.dll** - Standard Vulkan loader
- **vulkan_dzn.dll** - Mesa3D's DZN driver (Vulkan-to-D3D12 translation layer)
- **uwp_icd.x86_64.json** - ICD (Installable Client Driver) configuration file

## Building and Running

1. Open `uwp_vk_sample.sln` in Visual Studio 2022
2. Ensure you have the Windows 10 SDK (19041 or later)
3. Build the project for x64 Release configuration
4. Deploy to local machine or Xbox Series device

The application will:
1. Create an SDL window
2. Load Vulkan loader and DZN driver
3. Create a Vulkan instance and enumerate GPUs
4. Set up a rendering surface and swapchain
5. Render a red frame to demonstrate successful Vulkan operation

## Dependencies

All required dependencies are included in the `deps/` folder:
- SDL2 library and headers
- Vulkan DZN driver (`vulkan_dzn.dll`)
- Vulkan loader (`vulkan-1.dll`)
- ICD configuration (`uwp_icd.x86_64.json`)

## Platform Support

- Windows 10/11 (UWP applications)
- Xbox Series X|S
- Any device supporting DirectX 12 (via DZN translation)

## Technical Notes

- Uses manual DLL loading via `LoadPackagedLibrary()` for UWP compatibility
- Bypasses SDL's Vulkan integration (not available in UWP) and uses native Win32 surface creation
- Employs `ICoreWindowInterop` to obtain HWND in UWP context
- Demonstrates the "black magic" technique for getting native window handles in UWP

### Special Thanks

- [moonpower](https://github.com/momo-AUX1) for the [initial base](https://github.com/momo-AUX1/ashes-vk-uwp) for this repo
- Mesa3D team for the DZN driver implementation
