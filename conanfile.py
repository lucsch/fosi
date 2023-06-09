from conans import ConanFile, CMake
import os


class Fosi(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "wxwidgets/3.2.2.1@terranum-conan+wxwidgets/stable",
        "gdal/3.5.2@terranum-conan+gdal/stable",
        "libtiff/4.4.0",
        "libdeflate/1.12",
        "proj/9.0.1",
        "libjpeg/9e",
        "libpng/1.6.38"
    ]

    # options = {
    #     "unit_test": [True, False],
    #     "code_coverage": [True, False]}
    #
    # default_options = {
    #     "unit_test": False,
    #     "code_coverage": False}

    generators = "cmake", "gcc", "txt"

    # def requirements(self):
    #     if self.options.unit_test or self.options.code_coverage:
    #         self.requires("gtest/1.12.1")

    def configure(self):
        # if self.options.code_coverage:
        #     self.options.unit_test = True
        
        self.options["gdal"].with_curl = True # for xml support
        self.options["gdal"].shared = True


        # this isn't needed anymore with wxWidgets 3.2.1
        # if self.settings.os == "Linux":
        #    self.options["wxwidgets"].webview = False  # webview control isn't available on linux.

    def imports(self):
        # copy libraries
        self.copy("*.dll", dst="bin", src="bin")  # From bin to bin
        self.copy("*.dylib", dst="bin", src="lib")
        if self.settings.os == "Linux":
            self.copy("*.so*", dst="bin", src="lib")

        # copy proj library datum
        if self.settings.os == "Windows" or self.settings.os == "Linux":
            self.copy("*", dst="share/proj", src="res", root_package="proj")
        # copy proj.db into the binary directory on Linux
        if self.settings.os == "Linux":
            self.copy("proj.db", dst="bin", src="res", root_package="proj")
        if self.settings.os == "Macos":
            self.copy("*", dst="bin/Fosi.app/Contents/share/proj", src="res", root_package="proj")

    def build(self):
        cmake = CMake(self)
        # if self.options.unit_test:
        #     cmake.definitions["USE_UNITTEST"] = "ON"
        # if self.options.code_coverage:
        #     cmake.definitions["USE_UNITTEST"] = "ON"
        #     cmake.definitions["USE_CODECOV"] = "ON"

        cmake.configure()
        cmake.build()

        if self.settings.os == "Macos":
            cmake.install()
