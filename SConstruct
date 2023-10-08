# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
import sys
import os
from glob import glob
from pathlib import Path


env = SConscript("godot-cpp/SConstruct")

print("")
print("--- GDCubism ---")
print("")


# get CubismSdkForNative
def get_cubism_sdk(dirname: str) -> tuple[Path]:
    for o_path in Path("thirdparty").glob(dirname):
        tpl_path = (o_path.joinpath("Core"), o_path.joinpath("Framework"))
        if all(map(lambda o: o.is_dir(), tpl_path)) is True:
            print("     CUBISM_NATIVE_CORE_DIR = {:s}".format(str(tpl_path[0])))
            print("CUBISM_NATIVE_FRAMEWORK_DIR = {:s}".format(str(tpl_path[1])))
            if o_path.name == "CubismSdkForNative-5-r.1-beta.2":
                print("   CUBISM_MOTION_CUSTOMDATA = 1")
                env.Append(CPPDEFINES={"CUBISM_MOTION_CUSTOMDATA": 1})
            return str(tpl_path[0]), str(tpl_path[1])

    print("*** Directory not found, 'CubismSdkForNative' ***")
    sys.exit(1)


CUBISM_NATIVE_CORE_DIR, CUBISM_NATIVE_FRAMEWORK_DIR = get_cubism_sdk(
    "CubismSdkForNative*"
)

# check for experimenal

CHECK_PATH = "thirdparty/CubismNativeFramework"
if os.path.isdir(CHECK_PATH) is True:
    print("*** You are using a custom native framework that you prepared yourself. ***")
    CHECK_FILE = os.path.join(CHECK_PATH, "src", "Motion", "ACubismMotion.hpp")
    if os.path.isfile(CHECK_FILE) is True:
        print("CUBISM_NATIVE_FRAMEWORK_DIR = {:s}".format(CHECK_PATH))
        CUBISM_NATIVE_FRAMEWORK_DIR = CHECK_PATH


# Add source files.
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=[os.path.join(CUBISM_NATIVE_CORE_DIR, "include")])


print("                   platform = {:s}".format(env["platform"]))
print("                       arch = {:s}".format(env["arch"]))

if env["platform"] == "windows":
    print(
        "               MSVC_VERSION = {:s}".format(
            env.get("MSVC_VERSION", "(undefined)")
        )
    )
    arch = env["arch"]
    if arch == "x86_32":
        arch = "x86"

    o_cubism_lib = (
        Path(CUBISM_NATIVE_CORE_DIR)
        .joinpath("lib")
        .joinpath(env["platform"])
        .joinpath(arch)
        .joinpath(env["MSVC_VERSION"].replace(".", ""))
        .joinpath("Live2DCubismCore_MT.lib")
    )
    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR,
                "lib",
                "windows",
                arch,
                env["MSVC_VERSION"].replace(".", ""),
            ),
        ]
    )

    print("                       libs = {:s}".format(str(o_cubism_lib)))
    env.Append(LIBS=["Live2DCubismCore_MT"])

elif env["platform"] == "macos":
    o_cubism_lib = (
        Path(CUBISM_NATIVE_CORE_DIR)
        .joinpath("lib")
        .joinpath(env["platform"])
        .joinpath(env["arch"])
        .joinpath("libLive2DCubismCore.a")
    )
    if env["arch"] == "universal":
        if o_cubism_lib.is_file() is False:
            print("*** File not found, {:s} ***".format(str(o_cubism_lib)))
            print("*** Please refer to doc/BUILD.adoc or doc/BUILD.en.adoc")
            print("*** ex)")
            print("*** pushd {:s}/lib/macos".format(CUBISM_NATIVE_CORE_DIR))
            print("*** mkdir universal")
            print(
                "*** lipo -create arm64/{0:s} x86_64/{0:s} -output universal/{0:s}".format(
                    "libLive2DCubismCore.a"
                )
            )
            print("*** popd")
            sys.exit()
    print("                       libs = {:s}".format(str(o_cubism_lib)))
    env.Append(
        LIBPATH=[os.path.join(CUBISM_NATIVE_CORE_DIR, "lib", "macos", env["arch"])]
    )
    env.Append(LIBS=["Live2DCubismCore"])

elif env["platform"] == "ios":
    o_cubism_lib = (
        Path(CUBISM_NATIVE_CORE_DIR)
        .joinpath("lib")
        .joinpath(env["platform"])
        .joinpath("Release-iphoneos")
        .joinpath("libLive2DCubismCore.a")
    )
    print("                       libs = {:s}".format(str(o_cubism_lib)))
    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR, "lib", env["platform"], "Release-iphoneos"
            )
        ]
    )
    env.Append(LIBS=["Live2DCubismCore"])

elif env["platform"] == "linux":
    o_cubism_lib = (
        Path(CUBISM_NATIVE_CORE_DIR)
        .joinpath("lib")
        .joinpath(env["platform"])
        .joinpath(env["arch"])
        .joinpath("libLive2DCubismCore.a")
    )
    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR, "lib", "linux/{:s}".format(env["arch"])
            )
        ]
    )
    print("                       libs = {:s}".format(str(o_cubism_lib)))
    env.Append(LIBS=["Live2DCubismCore"])
else:
    pass

print("")

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
