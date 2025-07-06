
/*

Xbox vulkan starter with ashes exposing VK1.0 by mmonpower.
the structure is sound the main.cpp is bloated for demo purposes only.
the ashes dll can aslo be swapped by DZN in ICD mode so realy all you need to do is edit the JSON ICD.

*/

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <cstdlib>
#include <memory>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <Unknwn.h>
#include <dxgiformat.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"

#ifndef VKAPI_PTR
#define VKAPI_PTR __stdcall
#endif

typedef void* VkInstance;
typedef int   VkResult;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef void* VkQueue;
typedef void* VkSurfaceKHR;
typedef void* VkSwapchainKHR;
typedef void* VkImage;
typedef void* VkBuffer;
typedef void* VkDeviceMemory;
typedef void* VkShaderModule;
typedef void* VkFence;
typedef void* VkCommandPool;
typedef void* VkCommandBuffer;

static const unsigned int VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO = 28;
static const unsigned int VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO = 43;
static const unsigned int VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO = 44;
static const unsigned int VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER = 35;
static const unsigned int VK_STRUCTURE_TYPE_SUBMIT_INFO = 4;

/* enums / flags */
static const unsigned int VK_QUEUE_FAMILY_IGNORED = 0xffffffff;
static const unsigned int VK_COMMAND_BUFFER_LEVEL_PRIMARY = 0;
static const unsigned int VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001;
static const unsigned int VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001;
static const unsigned int VK_IMAGE_LAYOUT_UNDEFINED = 0;
static const unsigned int VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 6;
static const unsigned int VK_IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002;
static const unsigned int VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001;
static const unsigned int VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00001000;
static const unsigned int VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00000080;
static const unsigned int VK_ACCESS_TRANSFER_WRITE_BIT = 0x00080000;

const uint32_t VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR = 0x00000001;

/* helpers */
struct VkClearColorValue { float float32[4]; };
struct VkImageSubresourceRange { unsigned int aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount; };
struct VkImageMemoryBarrier { unsigned int sType; const void* pNext; unsigned int srcAccessMask, dstAccessMask, oldLayout, newLayout, srcQueueFamilyIndex, dstQueueFamilyIndex; VkImage image; VkImageSubresourceRange subresourceRange; };
struct VkCommandBufferAllocateInfo { unsigned int sType; const void* pNext; VkCommandPool commandPool; unsigned int level; unsigned int commandBufferCount; };
struct VkCommandBufferBeginInfo { unsigned int sType; const void* pNext; unsigned int flags; const void* pInheritanceInfo; };
struct VkSubmitInfo { unsigned int sType; const void* pNext; unsigned int waitSemaphoreCount; const void* pWaitSemaphores; const void* pWaitDstStageMask; unsigned int commandBufferCount; const VkCommandBuffer* pCommandBuffers; unsigned int signalSemaphoreCount; const void* pSignalSemaphores; };


HINSTANCE g_hInstance = nullptr;

using namespace winrt;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace Windows::UI;

// ----------------------------------------------------------------------------
// Vulkan constants and types 
// ----------------------------------------------------------------------------
static const int VK_SUCCESS = 0;
static const int VK_ERROR_INCOMPATIBLE_DRIVER = -9;
static const unsigned int VK_STRUCTURE_TYPE_APPLICATION_INFO = 0x00000001;
static const unsigned int VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 0x00000002;
static const unsigned int VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR = 1000009000;
static const unsigned int VK_STRUCTURE_TYPE_APPLICATION_INFO_1_0 = 0x00400000;
static const unsigned int VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR = 1000001000;
static const unsigned int VK_STRUCTURE_TYPE_PRESENT_INFO_KHR = 1000001002;


struct VkApplicationInfo {
    unsigned int sType;
    void* pNext;
    const char* pApplicationName;
    unsigned int applicationVersion;
    const char* pEngineName;
    unsigned int engineVersion;
    unsigned int apiVersion;
};

struct VkInstanceCreateInfo {
    unsigned int sType;
    void* pNext;
    unsigned int flags;
    VkApplicationInfo* pApplicationInfo;
    unsigned int enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    unsigned int enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
};

struct VkPhysicalDeviceProperties {
    unsigned int apiVersion;
    unsigned int driverVersion;
    unsigned int vendorID;
    unsigned int deviceID;
    unsigned int deviceType;
    char deviceName[256];
};

struct VkPhysicalDeviceFeatures {
    unsigned char dummyFeatureFlags[16];
};

struct VkPhysicalDeviceMemoryProperties {
    unsigned int memoryHeapCount;
    unsigned long long memoryHeaps[16];
    unsigned int memoryTypeCount;
    unsigned int memoryTypes[16];
};

struct VkDeviceQueueCreateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    unsigned int queueFamilyIndex;
    unsigned int queueCount;
    const float* pQueuePriorities;
};

struct VkDeviceCreateInfo {
    unsigned int                   sType;
    const void* pNext;
    unsigned int                   flags;
    unsigned int                   queueCreateInfoCount;
    const VkDeviceQueueCreateInfo* pQueueCreateInfos;
    unsigned int                   enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    unsigned int                   enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
    void* pEnabledFeatures;
};

struct VkWin32SurfaceCreateInfoKHR {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    HINSTANCE hinstance;
    HWND hwnd;
};

struct VkSurfaceCapabilitiesKHR {
    unsigned int minImageCount;
    unsigned int maxImageCount;
    unsigned int currentExtent[2];
    unsigned int minImageExtent[2];
    unsigned int maxImageExtent[2];
    unsigned int maxImageArrayLayers;
    unsigned int supportedTransforms;
    unsigned int currentTransform;
    unsigned int supportedCompositeAlpha;
    unsigned int supportedUsageFlags;
};

struct VkSurfaceFormatKHR {
    unsigned int format;
    unsigned int colorSpace;
};

struct VkSwapchainCreateInfoKHR {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    VkSurfaceKHR surface;
    unsigned int minImageCount;
    unsigned int imageFormat;
    unsigned int imageColorSpace;
    unsigned int imageExtent[2];
    unsigned int imageArrayLayers;
    unsigned int imageUsage;
    unsigned int imageSharingMode;
    unsigned int queueFamilyIndexCount;
    const unsigned int* pQueueFamilyIndices;
    unsigned int preTransform;
    unsigned int compositeAlpha;
    unsigned int presentMode;
    unsigned char clipped;
    VkSwapchainKHR oldSwapchain;
};

struct VkPresentInfoKHR {
    unsigned int sType;
    const void* pNext;
    unsigned int waitSemaphoreCount;
    const void* pWaitSemaphores;
    unsigned int swapchainCount;
    VkSwapchainKHR* pSwapchains;
    unsigned int* pImageIndices;
    void* pResults;
};

struct VkBufferCreateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    unsigned long long size;
    unsigned int usage;
    unsigned int sharingMode;
};

struct VkMemoryRequirements {
    unsigned long long size;
    unsigned long long alignment;
    unsigned int        memoryTypeBits;
};

struct VkMemoryAllocateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned long long allocationSize;
    unsigned int memoryTypeIndex;
};

struct VkShaderModuleCreateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    size_t       codeSize;
    const void* pCode;
};

struct VkFenceCreateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
};

struct VkCommandPoolCreateInfo {
    unsigned int sType;
    const void* pNext;
    unsigned int flags;
    unsigned int queueFamilyIndex;
};


// ----------------------------------------------------------------------------
// Loader function pointer types
// ----------------------------------------------------------------------------
typedef void* (VKAPI_PTR* PFN_vkGetInstanceProcAddr)(VkInstance, const char*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateInstance)(const VkInstanceCreateInfo*, const void*, VkInstance*);
typedef void (VKAPI_PTR* PFN_vkDestroyInstance)(VkInstance, const void*);
typedef VkResult(VKAPI_PTR* PFN_vkEnumeratePhysicalDevices)(VkInstance, unsigned int*, VkPhysicalDevice*);
typedef void (VKAPI_PTR* PFN_vkGetPhysicalDeviceProperties)(VkPhysicalDevice, VkPhysicalDeviceProperties*);
typedef void (VKAPI_PTR* PFN_vkGetPhysicalDeviceFeatures)(VkPhysicalDevice, VkPhysicalDeviceFeatures*);
typedef void (VKAPI_PTR* PFN_vkGetPhysicalDeviceMemoryProperties)(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*, const void*, VkSurfaceKHR*);
typedef void (VKAPI_PTR* PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR*);
typedef VkResult(VKAPI_PTR* PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(VkPhysicalDevice, VkSurfaceKHR, unsigned int*, VkSurfaceFormatKHR*);
typedef VkResult(VKAPI_PTR* PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(VkPhysicalDevice, VkSurfaceKHR, unsigned int*, unsigned int*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateSwapchainKHR)(VkDevice, const VkSwapchainCreateInfoKHR*, const void*, VkSwapchainKHR*);
typedef VkResult(VKAPI_PTR* PFN_vkGetSwapchainImagesKHR)(VkDevice, VkSwapchainKHR, unsigned int*, VkImage*);
typedef VkResult(VKAPI_PTR* PFN_vkAcquireNextImageKHR)(VkDevice, VkSwapchainKHR, unsigned long long, void*, VkFence*, unsigned int*);
typedef VkResult(VKAPI_PTR* PFN_vkQueuePresentKHR)(VkQueue, const VkPresentInfoKHR*, VkResult*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateDevice)(VkPhysicalDevice, const VkDeviceCreateInfo*, const void*, VkDevice*);
typedef void (VKAPI_PTR* PFN_vkGetDeviceQueue)(VkDevice, unsigned int, unsigned int, VkQueue*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateBuffer)(VkDevice, const VkBufferCreateInfo*, const void*, VkBuffer*);
typedef void (VKAPI_PTR* PFN_vkGetBufferMemoryRequirements)(VkDevice, VkBuffer, VkMemoryRequirements*);
typedef VkResult(VKAPI_PTR* PFN_vkAllocateMemory)(VkDevice, const VkMemoryAllocateInfo*, const void*, VkDeviceMemory*);
typedef VkResult(VKAPI_PTR* PFN_vkBindBufferMemory)(VkDevice, VkBuffer, VkDeviceMemory, unsigned long long);
typedef VkResult(VKAPI_PTR* PFN_vkCreateShaderModule)(VkDevice, const VkShaderModuleCreateInfo*, const void*, VkShaderModule*);
typedef VkResult(VKAPI_PTR* PFN_vkCreateFence)(VkDevice, const VkFenceCreateInfo*, const void*, VkFence*);
typedef VkResult(VKAPI_PTR* PFN_vkWaitForFences)(VkDevice, unsigned int, const VkFence*, unsigned int, unsigned long long);
typedef VkResult(VKAPI_PTR* PFN_vkCreateCommandPool)(VkDevice, const VkCommandPoolCreateInfo*, const void*, VkCommandPool*);
typedef VkResult(VKAPI_PTR* PFN_vkAllocateCommandBuffers)(VkDevice, const VkCommandBufferAllocateInfo*, VkCommandBuffer*);
typedef VkResult(VKAPI_PTR* PFN_vkBeginCommandBuffer)(VkCommandBuffer, const VkCommandBufferBeginInfo*);
typedef VkResult(VKAPI_PTR* PFN_vkEndCommandBuffer)(VkCommandBuffer);
typedef VkResult(VKAPI_PTR* PFN_vkQueueSubmit)(VkQueue, unsigned int, const void*, void*);
typedef VkResult(VKAPI_PTR* PFN_vk_icdNegotiateLoaderICDInterfaceVersion)(unsigned int*);

typedef VkResult(VKAPI_PTR* PFN_vkQueueWaitIdle)(VkQueue);
typedef void    (VKAPI_PTR* PFN_vkCmdClearColorImage)(VkCommandBuffer, VkImage,
    unsigned int ,
    const VkClearColorValue*,
    unsigned int ,
    const VkImageSubresourceRange*);
typedef void    (VKAPI_PTR* PFN_vkCmdPipelineBarrier)(VkCommandBuffer,
    unsigned int ,
    unsigned int,
    unsigned int ,
    unsigned int, const void*,
    unsigned int, const void*,
    unsigned int, const VkImageMemoryBarrier*);
typedef void    (VKAPI_PTR* PFN_vkCmdClearColorImage)(VkCommandBuffer, VkImage, unsigned int, const VkClearColorValue*, unsigned int, const VkImageSubresourceRange*);
typedef void    (VKAPI_PTR* PFN_vkCmdPipelineBarrier)(VkCommandBuffer, unsigned int, unsigned int, unsigned int, unsigned int, const void*, unsigned int, const void*, unsigned int, const VkImageMemoryBarrier*);
typedef VkResult(VKAPI_PTR* PFN_vkQueueWaitIdle)(VkQueue);

// ----------------------------------------------------------------------------
// Logging helpers
// ----------------------------------------------------------------------------
static void logDebug(const std::string& msg) {
    std::wstringstream wss;
    wss << L"[DEBUG] " << msg.c_str() << L"\n";
    OutputDebugStringW(wss.str().c_str());
}
static void logInfo(const std::string& msg) {
    std::wstringstream wss;
    wss << L"[INFO] " << msg.c_str() << L"\n";
    OutputDebugStringW(wss.str().c_str());
}
static void logWarning(const std::string& msg) {
    std::wstringstream wss;
    wss << L"[WARNING] " << msg.c_str() << L"\n";
    OutputDebugStringW(wss.str().c_str());
}
static void logError(const std::string& msg) {
    std::wstringstream wss;
    wss << L"[ERROR] " << msg.c_str() << L"\n";
    OutputDebugStringW(wss.str().c_str());
}

// ----------------------------------------------------------------------------
// instance extensions for surfaces
// ----------------------------------------------------------------------------
static const char* instance_extensions[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_KHR_portability_enumeration"
};

static const char* device_extensions[] = {
    "VK_KHR_swapchain"
};

// ----------------------------------------------------------------------------
//  SPIR-V unused
// ----------------------------------------------------------------------------
static const unsigned int s_trivialSpirv[] =
{
    0x07230203,0x00010000,0x000d0008,0x00000007,
    0x00020011,0x00000001,0x0006000b,0x00000001,
    0x4c534c47,0x6474732e, 0x3035342e,0x00000000,
    0x0003000e,0x00000000,0x00000000,0x00080006,
    0x00000001,0x00000000,0x475f4c47,0x4c474f4f,
    0x00000052,0x00000001,0x0003000e,0x00000000,
    0x00000001,0x00080006,0x00000001,0x00000000,
    0x475f4c47,0x4c474f4f,0x00000053,0x00000001,
    0x0003000e,0x00000000,0x00000002,0x00080006,
    0x00000001,0x00000000,0x475f4c47,0x4c474f4f,
    0x00000054,0x00000001,0x0003000e,0x00000000,
    0x00000003,0x00080006,0x00000001,0x00000000,
    0x475f4c47,0x4c474f4f,0x00000055,0x00000001,
    0x0003000e,0x00000000,0x00000004,0x00080006,
};

// ----------------------------------------------------------------------------
// Entry 
// ----------------------------------------------------------------------------
int SDL_main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logError(std::string("SDL initialization failed: ") + SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    if (!window) {
        logError(std::string("SDL window creation failed: ") + SDL_GetError());
        SDL_Quit();
        return -1;
    }
    logInfo("SDL window created.");

    try {
        std::string fullDir = "E:\\VK";
        std::string icd_json_path = "./ashesXBoxRenderer_icd.json";
        std::wstring w_icd_json_path(icd_json_path.begin(), icd_json_path.end());
        SetEnvironmentVariableW(L"VK_ICD_FILENAMES", w_icd_json_path.c_str());
        SetEnvironmentVariableW(L"VK_LOADER_DEBUG", L"all"); // NEEDED hack the entire magic relies on this thats why only C++ suports VK

        std::wstring baseLoaderName = L"vulkan-1.dll";
        HINSTANCE baseLoaderDll = LoadPackagedLibrary(baseLoaderName.c_str(), 0);
        if (!baseLoaderDll) {
            logError("Failed to load vulkan-1.dll");
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        logInfo("Loaded base loader DLL successfully.");

        std::wstring customICDName = L"ashesXBoxRenderer.dll";
        HINSTANCE customIcdDll = LoadPackagedLibrary(customICDName.c_str(), 0);
        if (!customIcdDll) {
            logError("Failed to load ashesD3D11Rendererd.dll");
            FreeLibrary(baseLoaderDll);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        logInfo("Loaded custom ICD DLL successfully.");

        auto vk_icdNegotiateLoaderICDInterfaceVersion = reinterpret_cast<PFN_vk_icdNegotiateLoaderICDInterfaceVersion>(
            GetProcAddress(customIcdDll, "vk_icdNegotiateLoaderICDInterfaceVersion")
            );
        if (vk_icdNegotiateLoaderICDInterfaceVersion) {
            unsigned int loader_version = 4;
            vk_icdNegotiateLoaderICDInterfaceVersion(&loader_version);
            logInfo("Negotiated ICD interface version successfully.");
        }

        auto vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
            GetProcAddress(baseLoaderDll, "vkGetInstanceProcAddr")
            );
        if (!vkGetInstanceProcAddr) {
            logError("vkGetInstanceProcAddr not found in loader.");
            FreeLibrary(baseLoaderDll);
            FreeLibrary(customIcdDll);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        logInfo("Retrieved vkGetInstanceProcAddr.");

        auto vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(
            vkGetInstanceProcAddr(nullptr, "vkCreateInstance")
            );
        if (!vkCreateInstance) {
            logError("Failed to get vkCreateInstance from loader.");
            FreeLibrary(baseLoaderDll);
            FreeLibrary(customIcdDll);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        logInfo("Retrieved vkCreateInstance pointer.");


        VkApplicationInfo app_info = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            "CustomApp",
            1,
            "NoEngine",
            1,
            VK_STRUCTURE_TYPE_APPLICATION_INFO_1_0
        };
        uint32_t extCount = sizeof(instance_extensions) / sizeof(instance_extensions[0]);
        VkInstanceCreateInfo instance_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    nullptr,
    VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    &app_info,
    0,
    nullptr,
    extCount,
    instance_extensions
        };

        VkInstance instance = nullptr;
        VkResult res = vkCreateInstance(&instance_info, nullptr, &instance);
        if (res != VK_SUCCESS) {
            logError("Failed to create Vulkan instance, code: " + std::to_string(res));
            FreeLibrary(baseLoaderDll);
            FreeLibrary(customIcdDll);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        logInfo("Vulkan instance created successfully.");


        struct
            __declspec(uuid("45D64A29-A63E-4CB6-B498-5781D298CB4F"))
            __declspec(novtable)
            ICoreWindowInterop : public IUnknown
        {
            virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* hwnd) = 0;
            virtual HRESULT STDMETHODCALLTYPE put_MessageHandled(unsigned char value) = 0;
        };



        HWND hwnd = []() -> HWND { //black magic technqiue to get an HWND on xbox.... I feel like the UWP devs would banish me for this.
            HWND h{};
            winrt::com_ptr<ICoreWindowInterop> interop{};
            winrt::check_hresult(
                winrt::get_unknown(CoreWindow::GetForCurrentThread())
                ->QueryInterface(interop.put())
            );
            winrt::check_hresult(interop->get_WindowHandle(&h));
            return 0;
            }();


        auto vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
            vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR")
            );
        VkSurfaceKHR surface = nullptr;
        if (vkCreateWin32SurfaceKHR) {
            VkWin32SurfaceCreateInfoKHR surfInfo = {};
            surfInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfInfo.flags = 1;
            surfInfo.hinstance = g_hInstance;
            surfInfo.hwnd = hwnd; //uneeded as of latest finally
            VkResult sRes = vkCreateWin32SurfaceKHR(instance, &surfInfo, nullptr, &surface);
            if (sRes == VK_SUCCESS && surface) {
                logInfo("Win32 surface creation succeeded.");
            }
            else {
                logWarning("Failed to create Win32 surface, code: " + std::to_string(sRes));
            }
        }
        else {
            logWarning("vkCreateWin32SurfaceKHR not found. Cannot create surface.");
        }

        auto vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
            vkGetInstanceProcAddr(instance, "vkQueuePresentKHR"));


        auto vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
            vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices")
            );
        VkPhysicalDevice firstGPU = nullptr;
        if (!vkEnumeratePhysicalDevices) {
            logWarning("vkEnumeratePhysicalDevices not found. No GPU listing.");
        }
        else {
            unsigned int gpuCount = 0;
            res = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
            if (res == VK_SUCCESS && gpuCount > 0) {
                logInfo("Found " + std::to_string(gpuCount) + " GPU(s).");
                std::unique_ptr<VkPhysicalDevice[]> gpus(new VkPhysicalDevice[gpuCount]);
                res = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.get());
                if (res == VK_SUCCESS) {
                    firstGPU = gpus[0];
                    auto vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
                        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties")
                        );
                    if (vkGetPhysicalDeviceProperties) {
                        VkPhysicalDeviceProperties props{};
                        vkGetPhysicalDeviceProperties(firstGPU, &props);
                        logInfo(std::string("GPU[0] Name: ") + props.deviceName);
                    }
                }
            }
        }

        if (firstGPU && surface) {

            auto vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(
                vkGetInstanceProcAddr(instance, "vkCreateDevice")
                );
            if (vkCreateDevice) {
                float qPriority = 1.0f;
                VkDeviceQueueCreateInfo qci = {};
                qci.sType = 0x29;
                qci.queueFamilyIndex = 0;
                qci.queueCount = 1;
                qci.pQueuePriorities = &qPriority;

                uint32_t deviceExtensionCount = sizeof(device_extensions) / sizeof(device_extensions[0]);
                VkDeviceCreateInfo dci = {};
                dci.sType = 0x2A;
                dci.queueCreateInfoCount = 1;
                dci.pQueueCreateInfos = &qci;
                dci.enabledExtensionCount = deviceExtensionCount;
                dci.ppEnabledExtensionNames = device_extensions;

                VkDevice device = nullptr;
                VkResult devRes = vkCreateDevice(firstGPU, &dci, nullptr, &device);
                if (devRes == VK_SUCCESS && device) {
                    logInfo("Logical device created successfully!");


                    auto vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(
                        vkGetInstanceProcAddr(instance, "vkGetDeviceQueue")
                        );
                    VkQueue deviceQueue = nullptr;
                    if (vkGetDeviceQueue) {
                        vkGetDeviceQueue(device, 0, 0, &deviceQueue);
                        if (deviceQueue) {
                            logInfo("Retrieved a device queue successfully.");
                        }
                    }

                    auto vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
                    auto vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
                    auto vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
                    auto vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetInstanceProcAddr(instance, "vkCreateSwapchainKHR"));
                    auto vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR"));
                    auto vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetInstanceProcAddr(instance, "vkAcquireNextImageKHR"));

                    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR && vkGetPhysicalDeviceSurfaceFormatsKHR && vkGetPhysicalDeviceSurfacePresentModesKHR && vkCreateSwapchainKHR && vkGetSwapchainImagesKHR && vkAcquireNextImageKHR) {
                        VkSurfaceCapabilitiesKHR surfaceCaps{};
                        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(firstGPU, surface, &surfaceCaps);

                        unsigned int formatCount;
                        vkGetPhysicalDeviceSurfaceFormatsKHR(firstGPU, surface, &formatCount, nullptr);
                        std::unique_ptr<VkSurfaceFormatKHR[]> surfaceFormats(new VkSurfaceFormatKHR[formatCount]);
                        vkGetPhysicalDeviceSurfaceFormatsKHR(firstGPU, surface, &formatCount, surfaceFormats.get());

                        unsigned int presentModeCount;
                        vkGetPhysicalDeviceSurfacePresentModesKHR(firstGPU, surface, &presentModeCount, nullptr);
                        std::unique_ptr<unsigned int[]> presentModes(new unsigned int[presentModeCount]);
                        vkGetPhysicalDeviceSurfacePresentModesKHR(firstGPU, surface, &presentModeCount, presentModes.get());
                        logInfo(std::string("Surface format count: ") + std::to_string(formatCount)); // the f k you mean 0???????
                        logInfo(std::string("Present mode count: ") + std::to_string(presentModeCount));


                        if (presentModeCount > 0) {
                            VkSwapchainCreateInfoKHR swapchainInfo{};
                            swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                            swapchainInfo.surface = surface;
                            swapchainInfo.minImageCount = surfaceCaps.minImageCount + 1;
                            if (surfaceCaps.maxImageCount > 0 && swapchainInfo.minImageCount > surfaceCaps.maxImageCount) {
                                swapchainInfo.minImageCount = surfaceCaps.maxImageCount;
                            }
                            logInfo(std::to_string(surfaceFormats[0].format));
                            logInfo(std::to_string(surfaceFormats[0].colorSpace));
                            swapchainInfo.imageFormat = surfaceFormats[0].format;
                            swapchainInfo.imageColorSpace = surfaceFormats[0].colorSpace;
                            swapchainInfo.imageExtent[0] = 1920; // i wonder if unlike mesa this supports 4K? 
                            swapchainInfo.imageExtent[1] = 1080;
                            swapchainInfo.imageArrayLayers = 1;
                            swapchainInfo.imageUsage = 0x10;
                            swapchainInfo.imageSharingMode = 0;
                            swapchainInfo.queueFamilyIndexCount = 0;
                            swapchainInfo.pQueueFamilyIndices = nullptr;
                            swapchainInfo.preTransform = surfaceCaps.currentTransform;
                            swapchainInfo.compositeAlpha = 0x1;
                            swapchainInfo.presentMode = presentModes[0];
                            swapchainInfo.clipped = 1;
                            swapchainInfo.oldSwapchain = nullptr;

                            VkSwapchainKHR swapchain = nullptr;
                            VkResult scRes = vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain);
                            if (scRes == VK_SUCCESS && swapchain) {
                                logInfo("Swapchain created successfully!");

                                unsigned int imageCount;
                                vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
                                if (imageCount > 0) {
                                    logInfo(std::string("Swapchain image count: ") + std::to_string(imageCount));
                                    std::unique_ptr<VkImage[]> swapchainImages(new VkImage[imageCount]);
                                    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.get());

                                    unsigned int imageIndex;
                                    VkResult acquireRes = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, nullptr, nullptr, &imageIndex);
                                    if (acquireRes == VK_SUCCESS) {
                                        logInfo("Successfully acquired a swapchain image.");

                                        auto vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(vkGetInstanceProcAddr(instance, "vkCreateCommandPool"));
                                        auto vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetInstanceProcAddr(instance, "vkAllocateCommandBuffers"));
                                        auto vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(vkGetInstanceProcAddr(instance, "vkBeginCommandBuffer"));
                                        auto vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(vkGetInstanceProcAddr(instance, "vkEndCommandBuffer"));
                                        auto vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vkGetInstanceProcAddr(instance, "vkCmdPipelineBarrier"));
                                        auto vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(vkGetInstanceProcAddr(instance, "vkCmdClearColorImage"));
                                        auto vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(vkGetInstanceProcAddr(instance, "vkQueueSubmit"));
                                        auto vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(vkGetInstanceProcAddr(instance, "vkQueueWaitIdle"));

                                        VkCommandPool cmdPool{};
                                        VkCommandPoolCreateInfo cpci{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, 0, 0 };
                                        vkCreateCommandPool(device, &cpci, nullptr, &cmdPool);

                                        VkCommandBuffer cmdBuf{};
                                        VkCommandBufferAllocateInfo cabi{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,nullptr,cmdPool,VK_COMMAND_BUFFER_LEVEL_PRIMARY,1 };
                                        vkAllocateCommandBuffers(device, &cabi, &cmdBuf);

                                        VkCommandBufferBeginInfo cbi{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,nullptr,VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,nullptr };
                                        vkBeginCommandBuffer(cmdBuf, &cbi);

                                        VkImageSubresourceRange subR{ VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1 };
                                        VkImageMemoryBarrier toDst{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,nullptr,0,VK_ACCESS_TRANSFER_WRITE_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,swapchainImages[imageIndex],subR };
                                        vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toDst);

                                        VkClearColorValue red{ {1.f,0.f,0.f,1.f} };
                                        vkCmdClearColorImage(cmdBuf, swapchainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &red, 1, &subR);

                                        VkImageMemoryBarrier toPresent{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,nullptr,VK_ACCESS_TRANSFER_WRITE_BIT,0,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,swapchainImages[imageIndex],subR };
                                        vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toPresent);

                                        vkEndCommandBuffer(cmdBuf);

                                        VkSubmitInfo si{ VK_STRUCTURE_TYPE_SUBMIT_INFO,nullptr,0,nullptr,nullptr,1,&cmdBuf,0,nullptr };
                                        vkQueueSubmit(deviceQueue, 1, &si, nullptr);
                                        vkQueueWaitIdle(deviceQueue);

                                        VkPresentInfoKHR pi{};
                                        pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                                        pi.swapchainCount = 1;
                                        pi.pSwapchains = &swapchain;
                                        pi.pImageIndices = &imageIndex;
                                        vkQueuePresentKHR(deviceQueue, &pi, nullptr);

                                        logInfo("Red frame presented!"); // CAN YOU SHWO IF YOIU SCCEEDED????
                                    }
                                }
                                else {
                                    logWarning("Failed to create swapchain, code: " + std::to_string(scRes)); // it should fail but i isnt tne fuclk kinda issue is thisq???????? could it be a swapchain issue?
                                }
                            }
                        }
                        else {
                            logWarning("Could not find suitable surface format or present mode.");
                        }
                    }
                    else {
                        logWarning("Swapchain related functions not found.");
                    }

                }
            }
        }


        auto vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(
            vkGetInstanceProcAddr(instance, "vkDestroyInstance")
            );
        if (vkDestroyInstance) {
            logInfo("Destroying Vulkan instance...");
            vkDestroyInstance(instance, nullptr);
            logInfo("Vulkan instance destroyed successfully.");
        }

        FreeLibrary(baseLoaderDll);
        FreeLibrary(customIcdDll);
        logInfo("All tests completed successfully!");
    }
    catch (std::exception& e) {
        logError(std::string("Exception: ") + e.what());
    }
    catch (...) {
        logError("Unknown exception occurred.");
    }


    bool running = true;
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}


int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    return SDL_WinRTRunApp(SDL_main, nullptr);
}