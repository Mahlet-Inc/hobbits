from conans import ConanFile, CMake, tools
from conans.tools import load
import glob
import os

class HobbitsConan(ConanFile):
    name = "hobbits"
    def set_version(self):
        version_file = os.path.join(self.recipe_folder, ".version")
        if os.path.exists(version_file):
            self.version = load(version_file)
        else:
            self.version = f"development-build"

    license = "MIT"
    author = "Adam Nash adam@smr.llc"
    url = "https://github.com/Mahlet-Inc/hobbits"
    description = "A multi-platform GUI for bit-based analysis, processing, and visualization"
    topics = ("qt")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {
        "shared": True,
        "fPIC": True,
        "qt:shared": True,
        "fftw:shared": True,
        "libpcap:shared": True,
        "fftw:threads": True,
        "fftw:combinedthreads": True,
        "icu:shared":False
        }
    generators = "cmake"
    exports_sources = "CMakeLists.txt", "cmake*", "src*"

    requires = [
        ("qt/5.15.2"),
        ("hobbits-cpython/3.9.1"),
        ("pffft/cci.20210511"),
        ("libusb/1.0.24")
    ]

    def requirements(self):
        if self.settings.os != "Windows":
            self.requires("libpcap/1.10.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        elif self.settings.os == "Macos":
            self.options['libpcap'].shared = False
            self.options['fftw'].shared = False

    def _configure_cmake(self):
        cmake = CMake(self)
        defs = {
            "SELF_CONTAINED_APP" : 1,
            "BUILDING_WITH_CONAN" : 1
        }
        if self.settings.build_type == "Release":
            defs["QT_NO_DEBUG"] = 1
        cmake.configure(source_folder=".", defs=defs)
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

        self.copy("*.h", dst="include", src="src")

        if self.settings.os == "Linux":
            self.copy("*", dst="bin/platforms", src="bin/platforms")
            self.copy("*", dst="lib", src="lib", symlinks=True)
            self.copy("*", dst="hobbits-cpython", src="hobbits-cpython", symlinks=True)
        elif self.settings.os == "Macos":
            self.copy("*", dst="hobbits.app/Contents/PlugIns/platforms", src="bin/platforms")
            self.copy("*", dst="hobbits.app/Contents/Frameworks", src="lib", symlinks=True)
            self.copy("*", dst="hobbits.app/Contents/Frameworks/hobbits-cpython", src="hobbits-cpython", symlinks=True)
        elif self.settings.os == "Windows":
            self.copy("*", dst=".", src="bin")
            self.copy("*", dst=".", src="hobbits-cpython/bin", symlinks=True)


    def package_info(self):
        self.cpp_info.libs = ["hobbits-core", "hobbits-widgets", "hobbits-python"]

    def imports(self):
        for pkg in ["qt", "pffft", "libpcap", "icu"]:
            self.copy("*.dll", "bin", "bin", root_package=pkg)
            self.copy("*.dylib*", "lib", "lib", root_package=pkg)
            self.copy("*.so*", "lib", "lib", root_package=pkg)
        
        self.copy("*", dst="bin/platforms", src="bin/archdatadir/plugins/platforms", root_package="qt")
        
        self.copy("*", dst="hobbits-cpython/bin", src="bin", root_package="hobbits-cpython")
        self.copy("*", dst="hobbits-cpython/include", src="include", root_package="hobbits-cpython")
        self.copy("*", dst="hobbits-cpython/lib", src="lib", root_package="hobbits-cpython")

        if self.settings.os == "Linux":
            for platform_plugin in glob.glob("bin/platforms/*.so*"):
                self.run(f"patchelf --remove-rpath {platform_plugin}")
                self.run(f"patchelf --force-rpath --set-rpath \\$ORIGIN/../../lib64:\\$ORIGIN/../../lib {platform_plugin}")
