#include "SDL.h"
#include "SDL_syswm.h"
#include <SDL_vulkan.h>
#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "spirv-headers/spirv.h"
#include <vector>
#include <set>
#include <array>
#include <iostream>
#include <algorithm>

#include "utilities.h"
#include "VulkanPP.h"


int win_width = 1280;
int win_height = 720;

VkResult res;
VkDevice device;
uint32_t frameIndex;
std::vector<VkFence> fences;
std::vector<VkCommandBuffer> commandBuffers;
VkCommandBuffer commandBuffer;
VkImage image;
VkSwapchainKHR swapchain;
VkSemaphore imageAvailableSemaphore;
VkSemaphore renderingFinishedSemaphore;
uint32_t swapchainImageCount;
std::vector<VkImage> swapchainImages;
VkQueue graphicsQueue;
VkQueue presentQueue;
VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
VkRenderPass render_pass;
std::vector<VkFramebuffer> swapchainFramebuffers;
VkExtent2D swapchainSize;
VkPhysicalDevice physical_devices;
VkSurfaceKHR surface;
std::vector<VkPhysicalDevice> physicalDevices;
uint32_t physicalDeviceCount = 0;
uint32_t graphics_QueueFamilyIndex;
uint32_t present_QueueFamilyIndex;
std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
VkSurfaceCapabilitiesKHR surfaceCapabilities;
std::vector<VkSurfaceFormatKHR> surfaceFormats;
VkSurfaceFormatKHR surfaceFormat;

std::vector<VkSubpassDependency> dependencies(1);
std::vector<VkAttachmentDescription> attachments(2);
std::vector<VkImageView> sc_attachments(2);

bool getSupportedDepthFormat(VkFormat* depthFormat)
{
    std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
    };

    for (const auto& format : depthFormats)
    {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physical_devices, format, &formatProps);
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            *depthFormat = format;
            return true;
        }
    }

    return false;
}

VkImageView createImageView(VkImage& imager, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = imager;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    res = vkCreateImageView(device, &viewInfo, nullptr, &imageView);
    CHECK_VULKAN(res);

    return imageView;
}

//global findMemoryType
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physical_devices, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}


void AcquireNextImage()
{
    res = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                          imageAvailableSemaphore,
                            VK_NULL_HANDLE,
                            &frameIndex);
    CHECK_VULKAN(res);

    res = vkWaitForFences(device, 1, &fences[frameIndex], VK_FALSE, UINT64_MAX);
    CHECK_VULKAN(res);

    res = vkResetFences(device, 1, &fences[frameIndex]);
    CHECK_VULKAN(res);

    commandBuffer = commandBuffers[frameIndex];
    image = swapchainImages[frameIndex];
}

void ResetCommandBuffer()
{
    res = vkResetCommandBuffer(commandBuffer, 0);
    CHECK_VULKAN(res);
}

void EndCommandBuffer()
{
    vkEndCommandBuffer(commandBuffer);
}

void BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil)
{
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass        = render_pass;
    render_pass_info.framebuffer       = swapchainFramebuffers[frameIndex];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swapchainSize;
    render_pass_info.clearValueCount   = 1;

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = clear_color;
    clearValues[1].depthStencil = clear_depth_stencil;

    render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    render_pass_info.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void BeginCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    CHECK_VULKAN(res);
}

void QueueSubmit()
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitDestStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderingFinishedSemaphore;
    res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences[frameIndex]);
    CHECK_VULKAN(res);
}

void QueuePresent()
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderingFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &frameIndex;
    res = vkQueuePresentKHR(presentQueue, &presentInfo);
    CHECK_VULKAN(res);

    res = vkQueueWaitIdle(presentQueue);
    CHECK_VULKAN(res);
}


//global createImage
void createImage(uint32_t width, uint32_t height,
                 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties, VkImage& imager,
                 VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult rslt = vkCreateImage(device, &imageInfo, nullptr, &imager);
    CHECK_VULKAN(rslt);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, imager, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    res = vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
    CHECK_VULKAN(res);

    res = vkBindImageMemory(device, imager, imageMemory, 0);
    CHECK_VULKAN(res);

}


static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanReportFunc(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData)
{
    std::cout << "VULKAN VALIDATION: " << msg << std::endl;
    return VK_FALSE;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    MADE_IT("Initialized SDL");

    SDL_Window* window = SDL_CreateWindow("SDL2 + Vulkan",
             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height,
             SDL_WINDOW_VULKAN);

    VulkanPP v(window);

    MADE_IT("Created Window");

    uint32_t extension_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);
    std::cout << "num extensions: " << extension_count << "\n";

    std::vector<const char*> extension_names(extension_count);

    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extension_names.data());

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "SDL2 + Vulkan";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "None";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char *> layer_names {};
    layer_names.push_back("VK_LAYER_KHRONOS_validation");

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = extension_names.size();
    create_info.ppEnabledExtensionNames = extension_names.data();
    create_info.enabledLayerCount = layer_names.size();
    create_info.ppEnabledLayerNames = layer_names.data();

    VkInstance instance;
    res = vkCreateInstance(&create_info, nullptr, &instance);
    CHECK_VULKAN(res);

    MADE_IT("Created Vulkan Instance");

    auto SDL2_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)SDL_Vulkan_GetVkGetInstanceProcAddr();

    VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};
    debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugCallbackCreateInfo.pfnCallback = VulkanReportFunc;

    VkDebugReportCallbackEXT debugCallback;
    res = SDL2_vkCreateDebugReportCallbackEXT(instance, &debugCallbackCreateInfo, nullptr, &debugCallback);
    CHECK_VULKAN(res);

    MADE_IT("Installed debug shim");

    auto xr = SDL_Vulkan_CreateSurface(window, instance, &surface);
    if (xr==SDL_FALSE) {
        throw std::runtime_error("No surface for you!");
    }

    MADE_IT("Created surface");

    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    physicalDevices.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    std::cout << "There are " << physicalDeviceCount << " physical devices" << std::endl;

    physical_devices = physicalDevices[0];

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    uint32_t queueFamilyCount;

    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices, &queueFamilyCount, nullptr);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices, &queueFamilyCount, queueFamilyProperties.data());

    std::cout << "Total queue family properties is " << queueFamilyCount << std::endl;

    int graphicIndex = -1;
    int presentIndex = -1;

    int i = 0;
    for(const auto& queueFamily : queueFamilyProperties)
    {
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicIndex = i;
        }

        VkBool32 presentSupport = false;
        res = vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices, i, surface, &presentSupport);
        CHECK_VULKAN(res);
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            presentIndex = i;
        }

        if(graphicIndex != -1 && presentIndex != -1)
        {
            break;
        }

        i++;
    }

    graphics_QueueFamilyIndex = graphicIndex;
    present_QueueFamilyIndex = presentIndex;

    std::cout << "GraphicsIdx: " << graphics_QueueFamilyIndex << " PresentIdx: " << present_QueueFamilyIndex << std::endl;

    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const float queue_priority[] = { 1.0f };

    std::set<uint32_t> uniqueQueueFamilies = { graphics_QueueFamilyIndex, present_QueueFamilyIndex };

    float queuePriority = queue_priority[0];
    for(unsigned int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphics_QueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    //https://en.wikipedia.org/wiki/Anisotropic_filtering
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    {
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = layer_names.size();
        createInfo.ppEnabledLayerNames = layer_names.data();
        res = vkCreateDevice(physical_devices, &createInfo, nullptr, &device);
        CHECK_VULKAN(res);
    }

    MADE_IT("Created Device");

    vkGetDeviceQueue(device, graphics_QueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, present_QueueFamilyIndex, 0, &presentQueue);

    MADE_IT("Created queues");

    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_devices, surface,&surfaceCapabilities);
    CHECK_VULKAN(res);

    MADE_IT("Gathered surfaces");

    uint32_t surfaceFormatsCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_devices, surface,
                                         &surfaceFormatsCount,
                                         nullptr);
    CHECK_VULKAN(res);

    std::cout << "Surface formats count - " << surfaceFormatsCount << std::endl;

    surfaceFormats.resize(surfaceFormatsCount);
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_devices, surface,
                                         &surfaceFormatsCount,
                                         surfaceFormats.data());
    CHECK_VULKAN(res);

    if(surfaceFormats[0].format != VK_FORMAT_B8G8R8A8_UNORM)
    {
        throw std::runtime_error("surfaceFormats[0].format != VK_FORMAT_B8G8R8A8_UNORM");
    }

    MADE_IT("Got the surface formats");

    surfaceFormat = surfaceFormats[0];
    int width,height = 0;
    SDL_Vulkan_GetDrawableSize(window, &width, &height);
    width = CLAMP(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
    height = CLAMP(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    std::cout << "Heignt: " << height << " Width: " << width << std::endl;

    swapchainSize.width = width;
    swapchainSize.height = height;

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    std::cout << "Needed image count : " << imageCount << std::endl;

    VkSwapchainCreateInfoKHR createInfo_cik = {};
    createInfo_cik.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo_cik.surface = surface;
    createInfo_cik.minImageCount = surfaceCapabilities.minImageCount;
    createInfo_cik.imageFormat = surfaceFormat.format;
    createInfo_cik.imageColorSpace = surfaceFormat.colorSpace;
    createInfo_cik.imageExtent = swapchainSize;
    createInfo_cik.imageArrayLayers = 1;
    createInfo_cik.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {graphics_QueueFamilyIndex, present_QueueFamilyIndex};
    if (graphics_QueueFamilyIndex != present_QueueFamilyIndex)
    {
        createInfo_cik.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo_cik.queueFamilyIndexCount = 2;
        createInfo_cik.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo_cik.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo_cik.preTransform = surfaceCapabilities.currentTransform;
    createInfo_cik.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo_cik.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo_cik.clipped = VK_TRUE;

    res = vkCreateSwapchainKHR(device, &createInfo_cik, nullptr, &swapchain);
    CHECK_VULKAN(res);

    MADE_IT("Created swapchain");

    res = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    CHECK_VULKAN(res);
    swapchainImages.resize(swapchainImageCount);
    res = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
    CHECK_VULKAN(res);
    std::vector<VkImageView> swapchainImageViews;

    std::cout << "Swap chain image count " << swapchainImages.size() << std::endl;

    for (auto swapchainImage : swapchainImages)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchainImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = surfaceFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        res = vkCreateImageView(device, &viewInfo, nullptr, &imageView);
        CHECK_VULKAN(res);
        swapchainImageViews.push_back(imageView);
    }

    MADE_IT("Created imageview");

    VkFormat depthFormat;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;

    bool validDepthFormat = getSupportedDepthFormat(&depthFormat);

    std::cout << "Valid Depth Format - " << validDepthFormat << " - " << depthFormat << std::endl;

    createImage(swapchainSize.width, swapchainSize.height,
                depthFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                depthImage, depthImageMemory);
    VkImageView depthImageView = createImageView(depthImage,VK_FORMAT_D32_SFLOAT_S8_UINT,
                                                 VK_IMAGE_ASPECT_DEPTH_BIT);

    {
        attachments[0].format = surfaceFormat.format;
        attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments[1].format = depthFormat;
        attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorReference = {};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthReference = {};
        depthReference.attachment = 1;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;
        subpassDescription.pDepthStencilAttachment = &depthReference;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = nullptr;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = nullptr;
        subpassDescription.pResolveAttachments = nullptr;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass);
        CHECK_VULKAN(res);
    }

    MADE_IT("Created render pass");

    swapchainFramebuffers.resize(swapchainImageViews.size());

    i=0;
    for (auto& swapchainImageView : swapchainImageViews)
    {
        sc_attachments[0] = swapchainImageView;
        sc_attachments[1] = depthImageView;

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = sc_attachments.data();
        framebufferInfo.width = swapchainSize.width;
        framebufferInfo.height = swapchainSize.height;
        framebufferInfo.layers = 1;

        VkFramebuffer fbuff;
        res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &fbuff);
        CHECK_VULKAN(res);

        std::cout << "FB: " << i << " @ " << fbuff << std::endl;

        swapchainFramebuffers[i++] = fbuff;
    }

    MADE_IT("Created swapchain views");

    VkCommandPool commandPool;
    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = graphics_QueueFamilyIndex;
        res = vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
        CHECK_VULKAN(res);
    }

    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = swapchainImageCount;

        commandBuffers.resize(swapchainImageCount);
        res = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data());
        CHECK_VULKAN(res);
    }

    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        res = vkCreateSemaphore(device, &createInfo, nullptr, &imageAvailableSemaphore);
        CHECK_VULKAN(res);
    }


    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        res = vkCreateSemaphore(device, &createInfo, nullptr, &renderingFinishedSemaphore);
        CHECK_VULKAN(res);
    }

    {
        fences.resize(swapchainImageCount);
        for (i = 0; i < swapchainImageCount; i++) {
            VkFenceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            res = vkCreateFence(device, &createInfo, nullptr, &fences[i]);
            CHECK_VULKAN(res);
        }
    }

    SDL_Event event;
    bool running = true;
    float r = 0;
    float g = 0;
    float b = 0;
    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        r+=0.02;
        g+=0.01;
        b+=0.01;

        if (r>1) r=0;
        if (g>1) g=0;
        if (b>1) b=0;

        AcquireNextImage();

        ResetCommandBuffer();

        BeginCommandBuffer();
        {
             VkClearColorValue clear_color = {r, g, b, 1.0f};
             VkClearDepthStencilValue clear_depth_stencil = {1.0f, 0};

             BeginRenderPass(clear_color, clear_depth_stencil);
             {

             }

             EndRenderPass();
        }
        EndCommandBuffer();

        QueueSubmit();
        QueuePresent();
    }

    MADE_IT("End SDL");
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}