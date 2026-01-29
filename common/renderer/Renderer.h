#ifndef SLINK_RENDERER_H
#define SLINK_RENDERER_H

#define NOMINMAX

#include <vulkan/vulkan_raii.hpp>
#include <window/Window.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <ranges>
#include <vector>
#include <GLFW/glfw3native.h>

namespace sLink::renderer
{
    class Renderer
    {
    public:
        Renderer();

        void init(window::Window &window);

        //void initImGuiBackend(window::Window &window);

        void notifyFramebufferResized();

        void waitIdle() const;

        void waitForFences() const;

        void updateSwapChain(window::Window &window);

        void setClearColor(vk::ClearValue color);

        // void recordImGuiData(Im_Gui &imgui);
        //
        // void beginImGuiFrame(Im_Gui &imgui);
        //
        // void endImGuiFrame(Im_Gui &imgui);

        void renderFrame();

        ~Renderer();

    private:
        void createInstance();

        std::vector<const char *> getRequiredExtensions() const;

        void setupDebugMessenger();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSurface(GLFWwindow *window);

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

        void createSwapChain(GLFWwindow *window);

        void createImageViews();

        void createCommandPools();

        void createCommandBuffers();

        void createSyncObjects();

        void createDepthResources();

        void createImGuiDescriptorPool();

        vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates,
                                       vk::ImageTiling tiling,
                                       vk::FormatFeatureFlags features) const;

        vk::Format findDepthFormat();

        bool hasStencilComponent(vk::Format format);

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::Format format,
                         vk::SampleCountFlagBits numSamples, vk::ImageTiling tiling,
                         vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image &image,
                         vk::raii::DeviceMemory &imageMemory);

        vk::raii::ImageView createImageView(vk::raii::Image &image,
                                            vk::Format format,
                                            vk::ImageAspectFlags aspectFlags,
                                            uint32_t mipLevels
        );

        void recordCommandBuffer(uint32_t imageIndex);

        void transitionImageLayout(
            uint32_t imageIndex,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            vk::AccessFlags2 srcAccessMask,
            vk::AccessFlags2 dstAccessMask,
            vk::PipelineStageFlags2 srcStageMask,
            vk::PipelineStageFlags2 dstStageMask
        );

        void transitionImageLayout(const vk::raii::Image &image,
                                   vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout,
                                   uint32_t mipLevels
        );

        void transitionImageLayout(vk::raii::CommandBuffer &commandBuffer,
                                   const vk::raii::Image &image,
                                   vk::Format format,
                                   vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout,
                                   uint32_t mipLevels,
                                   uint32_t baseMipLevel,
                                   vk::ImageAspectFlags aspectFlags
        );

        void copyBufferToImage(const vk::raii::Buffer &buffer,
                               vk::raii::Image &image,
                               uint32_t width, uint32_t height);

        std::unique_ptr<vk::raii::CommandBuffer> beginSingleTimeCommands();

        void endSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer);

        void cleanupSwapChain();

        void rebuildSwapChain(window::Window &window);

        vk::SampleCountFlagBits getMaxUsableSampleCount();

        void createColorResources();

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                              vk::DebugUtilsMessageTypeFlagsEXT type,
                                                              const vk::DebugUtilsMessengerCallbackDataEXT *
                                                              pCallbackData,
                                                              void *pUser);

        vk::raii::Context m_Context;

        vk::raii::Instance m_Instance;

        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger;

        vk::raii::PhysicalDevice m_PhysicalDevice;

        vk::raii::Device m_Device;

        vk::raii::Queue m_GraphicsQueue;

        vk::raii::SurfaceKHR m_Surface;

        vk::SurfaceFormatKHR m_SwapChainSurfaceFormat;

        vk::Extent2D m_SwapChainExtent;

        vk::raii::SwapchainKHR m_SwapChain;

        std::vector<vk::Image> m_SwapChainImages;

        vk::Format m_SwapChainImageFormat;

        std::vector<vk::raii::ImageView> m_SwapChainImageViews;

        vk::Viewport m_Viewport;

        vk::Rect2D m_Scissor;

        vk::raii::CommandPool m_PrimaryCommandPool;

        std::vector<vk::raii::CommandBuffer> m_CommandBuffers;

        std::vector<vk::raii::Semaphore> m_PresentCompleteSemaphores;

        std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores;

        std::vector<vk::raii::Fence> m_InFlightFences;

        uint32_t m_CurrentFrame;

        uint32_t m_GraphicsQueueFamilyIndex;

        uint32_t m_DirtyFrame;

        vk::raii::DescriptorPool m_ImGuiDescriptorPool;

        vk::raii::CommandPool m_ImGuiCommandPool;

        std::vector<vk::raii::CommandBuffer> m_ImGuiCommandBuffers;

        vk::raii::Image m_DepthImage;

        vk::raii::DeviceMemory m_DepthImageMemory;

        vk::raii::ImageView m_DepthImageView;

        vk::Format m_DepthFormat;

        vk::raii::Image m_ColorImage;

        vk::raii::DeviceMemory m_ColorImageMemory;

        vk::raii::ImageView m_ColorImageView;

        vk::SampleCountFlagBits m_MsaaSamples;

        vk::ClearValue m_ClearColor;

        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char *> deviceExtensions = {
            vk::KHRSwapchainExtensionName,
            vk::KHRSpirv14ExtensionName,
            vk::KHRSynchronization2ExtensionName,
            vk::KHRCreateRenderpass2ExtensionName,
            vk::KHRMaintenance7ExtensionName
        };

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        bool m_FramebufferResized;
    };
}

#endif
