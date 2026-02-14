#include "Renderer.h"

#include <map>
#include <print>

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace sLink::renderer
{
    Renderer::Renderer()
        : m_Instance(nullptr), m_DebugMessenger(nullptr),
          m_PhysicalDevice(nullptr), m_Device(nullptr),
          m_GraphicsQueue(nullptr), m_Surface(nullptr),
          m_SwapChain(nullptr), m_SwapChainImageFormat(vk::Format::eUndefined),
          m_PrimaryCommandPool(nullptr),
          m_ImGuiCommandPool(nullptr), m_ImGuiDescriptorPool(nullptr),
          m_FramebufferResized(false), m_CurrentFrame(0), m_DirtyFrame(std::numeric_limits<uint32_t>::max()),
          m_DepthImage(nullptr), m_DepthImageMemory(nullptr),
          m_DepthImageView(nullptr),
          m_ColorImage(nullptr), m_ColorImageView(nullptr),
          m_ColorImageMemory(nullptr)
    {
    }

    void Renderer::init(window::Window &window)
    {
        createInstance();

        setupDebugMessenger();

        createSurface(window.getGLFWWindow());

        pickPhysicalDevice();

        createLogicalDevice();

        createSwapChain(window.getGLFWWindow());

        createImageViews();

        createColorResources();

        createDepthResources();

        createCommandPools();

        createImGuiDescriptorPool();

        createCommandBuffers();

        createSyncObjects();


        initImGuiBackend(window);
    }

    void Renderer::initImGuiBackend(window::Window &window)
    {
        ImGui_ImplGlfw_InitForVulkan(window.getGLFWWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo{};

        initInfo.Instance = *m_Instance;
        initInfo.PhysicalDevice = *m_PhysicalDevice;
        initInfo.Device = *m_Device;
        initInfo.QueueFamily = m_GraphicsQueueFamilyIndex;
        initInfo.Queue = *m_GraphicsQueue;
        initInfo.DescriptorPool = *m_ImGuiDescriptorPool;
        initInfo.MinImageCount = m_SwapChainImages.size();
        initInfo.ImageCount = m_SwapChainImages.size();
        initInfo.PipelineInfoMain.MSAASamples = static_cast<VkSampleCountFlagBits>(m_MsaaSamples);
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = nullptr;

        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;

        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = reinterpret_cast<const VkFormat
            *>(&
            m_SwapChainImageFormat);

        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.depthAttachmentFormat = *reinterpret_cast<const VkFormat
            *>(&m_DepthFormat);

        if (!ImGui_ImplVulkan_Init(&initInfo))
            throw std::runtime_error("Failed to initialize ImGui for Vulkan");
    }

    void Renderer::createInstance()
    {
        constexpr vk::ApplicationInfo appInfo(
            "sLink",
            VK_MAKE_VERSION(1, 0, 0),
            "vk",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_4
        );

        std::vector<const char *> requiredLayers;

        if (enableValidationLayers)
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());

        auto layerProperties = m_Context.enumerateInstanceLayerProperties();

        for (auto const &requiredLayer: requiredLayers)

            if (std::ranges::none_of(layerProperties,
                                     [requiredLayer](auto const &layerProperty)
                                     {
                                         return strcmp(layerProperty.layerName, requiredLayer) == 0;
                                     }))
                throw std::runtime_error("Required layer not supported: " + std::string(requiredLayer));


        auto requiredExtensions = getRequiredExtensions();

        auto extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (auto const &requiredExtension: requiredExtensions)
            if (std::ranges::none_of(extensionProperties,
                                     [requiredExtension](auto const &extensionProperty)
                                     {
                                         return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
                                     }))
                throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));

        vk::InstanceCreateInfo createInfo{
            {},
            &appInfo,
            static_cast<uint32_t>(requiredLayers.size()),
            requiredLayers.data(),
            static_cast<uint32_t>(requiredExtensions.size()),
            requiredExtensions.data()
        };

        m_Instance = vk::raii::Instance(m_Context, createInfo);
    }

    std::vector<const char *> Renderer::getRequiredExtensions() const
    {
        uint32_t glfwExtensionCount = 0;

        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
            extensions.push_back(vk::EXTDebugUtilsExtensionName);

        return extensions;
    }

    void Renderer::setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
            {},
            severityFlags,
            messageTypeFlags,
            &debugCallback
        };

        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    void Renderer::pickPhysicalDevice()
    {
        auto devices = vk::raii::PhysicalDevices(m_Instance);

        if (devices.empty())
            throw std::runtime_error("Failed to find GPUs with Vulkan support");

        const auto devIter = std::ranges::find_if(devices,
                                                  [&](auto const &device)
                                                  {
                                                      auto queueFamilies = device.getQueueFamilyProperties();
                                                      bool isSuitable =
                                                              device.getProperties().apiVersion >= VK_API_VERSION_1_3;
                                                      const auto qfpIter = std::ranges::find_if(queueFamilies,
                                                          [](vk::QueueFamilyProperties const &qfp)
                                                          {
                                                              return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) !=
                                                                     static_cast<vk::QueueFlags>(0);
                                                          });
                                                      isSuitable = isSuitable && (qfpIter != queueFamilies.end());
                                                      auto extensions = device.enumerateDeviceExtensionProperties();
                                                      bool found = true;
                                                      for (auto const &extension: deviceExtensions)
                                                      {
                                                          auto extensionIter = std::ranges::find_if(
                                                              extensions, [extension](auto const &ext)
                                                              {
                                                                  return strcmp(ext.extensionName, extension) == 0;
                                                              });
                                                          found = found && extensionIter != extensions.end();
                                                      }
                                                      isSuitable = isSuitable && found;
                                                      std::print("\n");
                                                      if (isSuitable)
                                                      {
                                                          m_PhysicalDevice = device;
                                                          m_MsaaSamples = getMaxUsableSampleCount();
                                                      }

                                                      return isSuitable;
                                                  });
        if (devIter == devices.end())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        std::multimap<int, vk::raii::PhysicalDevice> candidates;

        for (const auto &device: devices)
        {
            auto deviceProperties = device.getProperties();
            auto deviceFeatures = device.getFeatures();
            uint32_t score = 0;

            if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                score += 1000;

            score += deviceProperties.limits.maxImageDimension2D;

            if (!deviceFeatures.geometryShader)
                continue;

            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0)
            m_PhysicalDevice = std::move(candidates.rbegin()->second);
        else
            throw std::runtime_error("Failed to find a suitable GPU");
    }

    void Renderer::createLogicalDevice()
    {
        auto queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

        uint32_t queueIndex = ~0;

        for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
        {
            if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
                m_PhysicalDevice.getSurfaceSupportKHR(qfpIndex, m_Surface))
            {
                queueIndex = qfpIndex;
                break;
            }
        }

        m_GraphicsQueueFamilyIndex = queueIndex;

        if (queueIndex == ~0)
            throw std::runtime_error("Could not find a queue for graphics and present -> terminating");

        vk::PhysicalDeviceMaintenance7FeaturesKHR maintenance7Features{};
        maintenance7Features.maintenance7 = VK_TRUE;

        vk::PhysicalDeviceFeatures2 features2{};

        features2.features.samplerAnisotropy = vk::True;
        features2.features.sampleRateShading = vk::True;

        features2.features.multiDrawIndirect = vk::True;

        vk::PhysicalDeviceVulkan12Features features12{};

        features12.runtimeDescriptorArray = vk::True;

        vk::PhysicalDeviceVulkan13Features features13{};

        features13.dynamicRendering = true;
        features13.synchronization2 = true;

        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT featuresExt{};

        featuresExt.extendedDynamicState = true;


        vk::StructureChain featureChain = {
            features2,
            features12,
            features13,
            featuresExt,
            maintenance7Features
        };

        float queuePriority = 0.f;

        vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
            {},
            queueIndex,
            1,
            &queuePriority
        };

        vk::DeviceCreateInfo deviceCreateInfo{};

        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);

        m_GraphicsQueue = vk::raii::Queue(m_Device, queueIndex, 0);
    }

    void Renderer::createSurface(GLFWwindow *window)
    {
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(*m_Instance, window, nullptr, &surface))
            throw std::runtime_error("failed to create window surface");

        m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
    }

    vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    {
        for (const auto &format: availableFormats)
            if (format.format == vk::Format::eB8G8R8A8Unorm &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return format;

        return availableFormats[0];
    }

    vk::PresentModeKHR Renderer::chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR> &availablePresentModes)
    {
        /*for (const auto &presentMode: availablePresentModes)
			if (presentMode == vk::PresentModeKHR::eMailbox)
				return presentMode;*/

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()
        )
            return capabilities.currentExtent;

        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    void Renderer::createSwapChain(GLFWwindow *window)
    {
        auto surfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);

        m_SwapChainSurfaceFormat = chooseSwapSurfaceFormat(m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface));

        m_SwapChainExtent = chooseSwapExtent(surfaceCapabilities, window);

        m_SwapChainImageFormat = m_SwapChainSurfaceFormat.format;

        m_Viewport = vk::Viewport(0.f, 0.f,
                                  static_cast<float>(m_SwapChainExtent.width),
                                  static_cast<float>(m_SwapChainExtent.height),
                                  0.f, 1.f);

        m_Scissor = vk::Rect2D({0, 0},
                               m_SwapChainExtent);

        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

        minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
                            ? surfaceCapabilities.maxImageCount
                            : minImageCount;

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
            imageCount = surfaceCapabilities.maxImageCount;

        vk::SwapchainCreateInfoKHR swapChainCreateInfo{
            vk::SwapchainCreateFlagsKHR(),
            m_Surface,
            minImageCount,
            m_SwapChainSurfaceFormat.format,
            m_SwapChainSurfaceFormat.colorSpace,
            m_SwapChainExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive
        };

        swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapChainCreateInfo.presentMode = chooseSwapPresentMode(m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface));
        swapChainCreateInfo.clipped = true;
        swapChainCreateInfo.oldSwapchain = nullptr;

        m_SwapChain = vk::raii::SwapchainKHR(m_Device, swapChainCreateInfo);

        m_SwapChainImages = m_SwapChain.getImages();
    }

    void Renderer::createImageViews()
    {
        m_SwapChainImageViews.clear();

        vk::ImageViewCreateInfo imageViewCreateInfo
        {
            {},
            {},
            vk::ImageViewType::e2D,
            m_SwapChainImageFormat
        };

        imageViewCreateInfo.subresourceRange = {
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        };

        for (auto &image: m_SwapChainImages)
        {
            imageViewCreateInfo.image = image;
            m_SwapChainImageViews.emplace_back(m_Device, imageViewCreateInfo);
        }
    }

    void Renderer::createCommandPools()
    {
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_PrimaryCommandPool = vk::raii::CommandPool(m_Device, poolInfo);

        m_ImGuiCommandPool = vk::raii::CommandPool(m_Device, poolInfo);
    }

    void Renderer::recordCommandBuffer(uint32_t imageIndex)
    {
        auto &commandBuffer = m_CommandBuffers[m_CurrentFrame];

        commandBuffer.begin({});

        transitionImageLayout(
            imageIndex,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eTopOfPipe,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.f, 0.f);

        vk::RenderingAttachmentInfo colorAttachmentInfo(
            m_ColorImageView,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ResolveModeFlagBits::eAverage,
            m_SwapChainImageViews[imageIndex],
            vk::ImageLayout::eColorAttachmentOptimal
        );

        colorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentInfo.clearValue = m_ClearColor;

        vk::RenderingAttachmentInfo depthAttachmentInfo(
            m_DepthImageView,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        );

        depthAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachmentInfo.clearValue = clearDepth;

        vk::RenderingInfoKHR renderingInfo(
            vk::RenderingFlagBitsKHR::eContentsSecondaryCommandBuffers,
            m_Scissor,
            1, {}, 1,
            &colorAttachmentInfo,
            &depthAttachmentInfo
        );

        commandBuffer.beginRendering(renderingInfo);

        commandBuffer.executeCommands(*m_ImGuiCommandBuffers[m_CurrentFrame]);

        commandBuffer.endRendering();

        transitionImageLayout(
            imageIndex,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe
        );

        commandBuffer.end();
    }

    void Renderer::transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                         vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
                                         vk::PipelineStageFlags2 srcStageMask,
                                         vk::PipelineStageFlags2 dstStageMask)
    {
        vk::ImageMemoryBarrier2 barrier(
            srcStageMask, srcAccessMask,
            dstStageMask, dstAccessMask,
            oldLayout, newLayout,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            m_SwapChainImages[imageIndex],
            {
                vk::ImageAspectFlagBits::eColor,
                0, 1, 0, 1
            }
        );

        vk::DependencyInfo dependencyInfo;

        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        m_CommandBuffers[m_CurrentFrame].pipelineBarrier2(dependencyInfo);
    }

    void Renderer::transitionImageLayout(const vk::raii::Image &image,
                                         vk::ImageLayout oldLayout,
                                         vk::ImageLayout newLayout,
                                         uint32_t mipLevels
    )
    {
        auto aspectMask = vk::ImageAspectFlagBits::eColor;

        if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
            aspectMask = vk::ImageAspectFlagBits::eDepth;

        auto commandBuffer = beginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier;

        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;
        barrier.subresourceRange = {
            aspectMask,
            0, mipLevels, 0, 1
        };

        vk::PipelineStageFlags sourceStage;

        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==
                   vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else
            throw std::invalid_argument("Unsupported layout transition");

        commandBuffer->pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
        endSingleTimeCommands(*commandBuffer);
    }

    void Renderer::transitionImageLayout(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Image &image,
                                         vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                         uint32_t mipLevels,
                                         uint32_t baseMipLevel, vk::ImageAspectFlags aspectFlags)
    {
        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = *image;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseMipLevel = baseMipLevel;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;


        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==
                   vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout ==
                   vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                    vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        } else if (oldLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal && newLayout ==
                   vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eLateFragmentTests;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && newLayout ==
                   vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eFragmentShader;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==
                   vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else
            throw std::invalid_argument("Unsupported layout transition in transitionImageLayout");


        commandBuffer.pipelineBarrier(
            sourceStage,
            destinationStage,
            {},
            {},
            {},
            barrier
        );
    }

    void Renderer::copyBufferToImage(const vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width,
                                     uint32_t height)
    {
        auto commandBuffer = beginSingleTimeCommands();

        vk::BufferImageCopy region(
            0, 0, 0,
            {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            {0, 0, 0},
            {width, height, 1}
        );

        commandBuffer->copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});

        endSingleTimeCommands(*commandBuffer);
    }

    std::unique_ptr<vk::raii::CommandBuffer> Renderer::beginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo(
            m_PrimaryCommandPool,
            vk::CommandBufferLevel::ePrimary,
            1
        );

        auto commandBuffer = std::make_unique<vk::raii::CommandBuffer>
        (
            std::move(vk::raii::CommandBuffers(m_Device, allocInfo).front())
        );

        commandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        return commandBuffer;
    }

    void Renderer::endSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandBuffer;

        m_GraphicsQueue.submit(submitInfo);

        m_GraphicsQueue.waitIdle();
    }

    void Renderer::cleanupSwapChain()
    {
        m_ColorImageView = nullptr;
        m_ColorImage = nullptr;
        m_ColorImageMemory = nullptr;

        m_DepthImageView = nullptr;
        m_DepthImage = nullptr;
        m_DepthImageMemory = nullptr;

        m_SwapChainImageViews.clear();
        m_SwapChain = nullptr;
    }

    void Renderer::rebuildSwapChain(window::Window &window)
    {
        createSwapChain(window.getGLFWWindow());

        createImageViews();

        createColorResources();

        createDepthResources();
    }

    vk::SampleCountFlagBits Renderer::getMaxUsableSampleCount()
    {
        vk::PhysicalDeviceProperties physicalDeviceProperties = m_PhysicalDevice.getProperties();

        vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                      physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
        if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
        if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
        if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

        return vk::SampleCountFlagBits::e1;
    }

    void Renderer::createColorResources()
    {
        createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1,
                    m_SwapChainImageFormat,
                    m_MsaaSamples,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    m_ColorImage,
                    m_ColorImageMemory
        );

        m_ColorImageView = createImageView(m_ColorImage, m_SwapChainImageFormat, vk::ImageAspectFlagBits::eColor, 1);
    }

    uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
    {
        auto memProperties = m_PhysicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        throw std::runtime_error("Failed to find suitable memory type");
    }

    void Renderer::notifyFramebufferResized()
    {
        m_FramebufferResized = true;
    }

    void Renderer::waitIdle() const
    {
        m_Device.waitIdle();
    }

    void Renderer::waitForFences() const
    {
        auto result = m_Device.waitForFences(*m_InFlightFences[m_CurrentFrame], vk::True, UINT64_MAX);

        if (result != vk::Result::eSuccess)
            throw std::runtime_error("Waiting for fence failed/timed out!");
    }

    void Renderer::updateSwapChain(window::Window &window)
    {
        waitIdle();

        ImGui_ImplVulkan_Shutdown();

        ImGui_ImplGlfw_Shutdown();

        cleanupSwapChain();

        rebuildSwapChain(window);

        initImGuiBackend(window);

        ui::UIBackend::begin_frame();

        ui::UIBackend::end_frame();
    }

    void Renderer::setClearColor(vk::ClearValue color)
    {
        m_ClearColor = color;
    }

    void Renderer::createCommandBuffers()
    {
        m_CommandBuffers.clear();

        vk::CommandBufferAllocateInfo allocInfo(
            m_PrimaryCommandPool,
            vk::CommandBufferLevel::ePrimary,
            MAX_FRAMES_IN_FLIGHT
        );

        m_CommandBuffers = vk::raii::CommandBuffers(m_Device, allocInfo);

        m_ImGuiCommandBuffers.clear();

        vk::CommandBufferAllocateInfo imguiAllocInfo(
            m_ImGuiCommandPool,
            vk::CommandBufferLevel::eSecondary,
            MAX_FRAMES_IN_FLIGHT
        );

        m_ImGuiCommandBuffers = vk::raii::CommandBuffers(m_Device, imguiAllocInfo);
    }

    void Renderer::createSyncObjects()
    {
        m_PresentCompleteSemaphores.clear();

        m_RenderFinishedSemaphores.clear();

        m_InFlightFences.clear();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_PresentCompleteSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());

            m_InFlightFences.emplace_back(m_Device, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        }

        for (int i = 0; i < m_SwapChainImages.size(); ++i)
            m_RenderFinishedSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());
    }

    void Renderer::createDepthResources()
    {
        m_DepthFormat = findDepthFormat();

        createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1,
                    m_DepthFormat,
                    m_MsaaSamples,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    m_DepthImage,
                    m_DepthImageMemory
        );

        m_DepthImageView = createImageView(m_DepthImage, m_DepthFormat, vk::ImageAspectFlagBits::eDepth, 1);
    }

    void Renderer::createImGuiDescriptorPool()
    {
        constexpr uint32_t descriptorCount = 1000;

        constexpr std::array poolSizes = {
            vk::DescriptorPoolSize(vk::DescriptorType::eSampler, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, descriptorCount),
            vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, descriptorCount)
        };

        vk::DescriptorPoolCreateInfo poolInfo(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            descriptorCount * poolSizes.size(),
            poolSizes.size(),
            poolSizes.data()
        );

        m_ImGuiDescriptorPool = vk::raii::DescriptorPool(m_Device, poolInfo);
    }

    vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                                             vk::FormatFeatureFlags features) const
    {
        for (const auto &format: candidates)
        {
            vk::FormatProperties props = m_PhysicalDevice.getFormatProperties(format);

            if (tiling == vk::ImageTiling::eLinear &&
                (props.linearTilingFeatures & features) == features)
                return format;

            if (tiling == vk::ImageTiling::eOptimal &&
                (props.optimalTilingFeatures & features) == features)
                return format;
        }

        throw std::runtime_error("Failed to find suitable format");
    }

    vk::Format Renderer::findDepthFormat()
    {
        return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    bool Renderer::hasStencilComponent(vk::Format format)
    {
        return format == vk::Format::eD32SfloatS8Uint ||
               format == vk::Format::eD24UnormS8Uint;
    }

    void Renderer::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::Format format,
                               vk::SampleCountFlagBits numSamples,
                               vk::ImageTiling tiling,
                               vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                               vk::raii::Image &image,
                               vk::raii::DeviceMemory &imageMemory)
    {
        vk::ImageCreateInfo imageInfo(
            {},
            vk::ImageType::e2D,
            format,
            {width, height, 1},
            mipLevels
            , 1,
            numSamples,
            tiling,
            usage,
            vk::SharingMode::eExclusive
        );

        image = vk::raii::Image(m_Device, imageInfo);

        auto memReq = image.getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo(
            memReq.size,
            findMemoryType(memReq.memoryTypeBits, properties)
        );

        imageMemory = vk::raii::DeviceMemory(m_Device, allocInfo);

        image.bindMemory(imageMemory, 0);
    }

    vk::raii::ImageView Renderer::createImageView(vk::raii::Image &image, vk::Format format,
                                                  vk::ImageAspectFlags aspectFlags,
                                                  uint32_t mipLevels
    )
    {
        vk::ImageViewCreateInfo viewInfo(
            {},
            image,
            vk::ImageViewType::e2D,
            format,
            {},
            {vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 1}
        );

        viewInfo.subresourceRange.aspectMask = aspectFlags;

        return vk::raii::ImageView(m_Device, viewInfo);
    }

    void Renderer::recordUIData(ui::UIBackend::UIRenderData *data)
    {
        auto &cmd = m_ImGuiCommandBuffers[m_CurrentFrame];

        cmd.reset();

        vk::CommandBufferInheritanceRenderingInfoKHR inheritanceRenderingInfo{};

        inheritanceRenderingInfo.colorAttachmentCount = 1;
        inheritanceRenderingInfo.pColorAttachmentFormats = &m_SwapChainImageFormat;
        inheritanceRenderingInfo.depthAttachmentFormat = m_DepthFormat;
        inheritanceRenderingInfo.rasterizationSamples = m_MsaaSamples;

        vk::CommandBufferInheritanceInfo inheritanceInfo;

        inheritanceInfo.pNext = &inheritanceRenderingInfo;
        inheritanceInfo.renderPass = VK_NULL_HANDLE;

        vk::CommandBufferBeginInfo beginInfo(
            vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
            &inheritanceInfo
        );

        cmd.begin(beginInfo);

        ImGui_ImplVulkan_RenderDrawData(data, *cmd);

        cmd.end();
    }

    void Renderer::renderFrame()
    {
        m_FramebufferResized = false;

        uint32_t imageIndex;
        try
        {
            auto [result, index] = m_SwapChain.acquireNextImage(
                UINT64_MAX,
                *m_PresentCompleteSemaphores[m_CurrentFrame]
            );

            imageIndex = index;
        } catch (const vk::OutOfDateKHRError &)
        {
            m_FramebufferResized = true;
            return;
        }

        m_Device.resetFences(*m_InFlightFences[m_CurrentFrame]);

        recordCommandBuffer(imageIndex);

        vk::PipelineStageFlags waitStages[] = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eFragmentShader
        };

        vk::Semaphore waitSemaphores[] = {
            *m_PresentCompleteSemaphores[m_CurrentFrame]
        };

        const vk::SubmitInfo submitInfo(1,
                                        waitSemaphores,
                                        waitStages,
                                        1, &*m_CommandBuffers[m_CurrentFrame],
                                        1, &*m_RenderFinishedSemaphores[imageIndex]
        );

        m_GraphicsQueue.submit(submitInfo, *m_InFlightFences[m_CurrentFrame]);

        const vk::PresentInfoKHR presentInfo(1, &*m_RenderFinishedSemaphores[imageIndex],
                                             1, &*m_SwapChain,
                                             &imageIndex
        );

        vk::Result result;
        try
        {
            result = m_GraphicsQueue.presentKHR(presentInfo);
        } catch (const vk::OutOfDateKHRError &e)
        {
            result = vk::Result::eErrorOutOfDateKHR;
        }

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_FramebufferResized)
            m_FramebufferResized = true;
        else if (result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to present swap chain image");

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Renderer::~Renderer()
    {
        m_Device.waitIdle();

        ImGui_ImplVulkan_Shutdown();

        ImGui_ImplGlfw_Shutdown();

        cleanupSwapChain();
    }

    vk::Bool32 Renderer::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                       vk::DebugUtilsMessageTypeFlagsEXT type,
                                       const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUser)
    {
        std::println("Validation layer: type {} msg {}", to_string(type), pCallbackData->pMessage);

        return VK_FALSE;
    }
}