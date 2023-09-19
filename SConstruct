# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
import os
from glob import glob
from pathlib import Path


env = SConscript("godot-cpp/SConstruct")


CUBISM_NATIVE_CORE_DIR = "thirdparty/CubismSdkForNative/Core"
CUBISM_NATIVE_FRAMEWORK_DIR = "thirdparty/CubismSdkForNative/Framework"


# check for experimenal
CHECK_PATH = "thirdparty/CubismNativeFramework"
if os.path.isdir(CHECK_PATH) is True:
    CHECK_FILE = os.path.join(CHECK_PATH, "src", "Motion", "ACubismMotion.hpp")
    if os.path.isfile(CHECK_FILE) is True:
        env.Append(CPPDEFINES={"CUBISM_MOTION_CUSTOMDATA": 1})
        CUBISM_NATIVE_FRAMEWORK_DIR = CHECK_PATH


# Add source files.
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=[os.path.join(CUBISM_NATIVE_CORE_DIR, "include")])

if env["platform"] == "windows":
    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR,
                "lib",
                "windows/{:s}/{:s}".format(
                    env["arch"], env["MSVC_VERSION"].replace(".", "")
                ),
            )
        ]
    )
    env.Append(LIBS=["Live2DCubismCore_MT"])

elif env["platform"] == "macos":
    env.Append(LIBPATH=[os.path.join(CUBISM_NATIVE_CORE_DIR, "lib", "macos")])
    env.Append(LIBS=["Live2DCubismCore.universal"])

elif env["platform"] == "linux":
    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR, "lib", "linux/{:s}".format(env["arch"])
            )
        ]
    )
    env.Append(LIBS=["Live2DCubismCore"])
else:
    pass

sources = glob("src/*.cpp")
sources += glob("src/private/*.cpp")

env.Append(CPPPATH=[os.path.join(CUBISM_NATIVE_FRAMEWORK_DIR, "src")])

sources_cubism = glob(os.path.join(CUBISM_NATIVE_FRAMEWORK_DIR, "src", "*.cpp"))

for dirname in (
    "Effect",
    "Id",
    "Math",
    "Model",
    "Motion",
    "Physics",
    "Rendering",
    "Type",
    "Utils",
):
    sources_cubism += glob(
        os.path.join(CUBISM_NATIVE_FRAMEWORK_DIR, "src", dirname, "*.cpp")
    )

sources += sources_cubism


(extension_path,) = glob("demo/addons/*/*.gdextension")

# Find the addon path (e.g. project/addons/example).
addon_path = Path(extension_path).parent

# Find the project name from the gdextension file (e.g. example).
project_name = Path(extension_path).stem

# TODO: Cache is disabled currently.
# scons_cache_path = os.environ.get("SCONS_CACHE")
# if scons_cache_path != None:
#     CacheDir(scons_cache_path)
#     print("Scons cache enabled... (path: '" + scons_cache_path + "')")

# Create the library target (e.g. libexample.linux.debug.x86_64.so).
debug_or_release = "release" if env["target"] == "template_release" else "debug"
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

Default(library)
