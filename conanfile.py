from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy
import os

class SLinkRecipe(ConanFile):
    name = "sLink"
    version = "1.0"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"

    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("asio/1.36.0") 
        self.requires("catch2/3.13.0") 
        self.requires("glfw/3.4") 
        self.requires("imgui/1.92.5")
        self.requires("nlohmann_json/3.12.0") 
        self.requires("miniaudio/0.11.22") 
        self.requires("sqlite3/3.51.0") 
        self.requires("stb/cci.20240531") 

    def configure(self):
        self.options["imgui"].with_glfw = True
        self.options["imgui"].with_vulkan = True

    def layout(self):
        cmake_layout(self)

    def generate(self):
        imgui = self.dependencies["imgui"]
        bindings_dir = os.path.join(imgui.package_folder, "res", "bindings")
        dest_dir = os.path.join(self.source_folder, "common", "imgui_bindings")

        for pattern in ["*imgui_impl_glfw*", "*imgui_impl_vulkan*"]:
            copy(self, pattern, bindings_dir, dest_dir)