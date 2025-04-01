# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
import sys
import os
import configparser
from glob import glob
from pathlib import Path

from SCons.Script import SConscript, ARGUMENTS


env = SConscript("godot-cpp/SConstruct")

print("")
print("--- GDCubism ---")
print("")


GD_EXTENSION_PATH = "demo/addons/gd_cubism/gd_cubism.gdextension"


def live2d_arch() -> str:
    return {
        "arm64": "arm64-v8a",
        "arm32": "armeabi-v7a",
        "universal": "universal",
        "x86_32": "x86",
        "x86_64": "x86_64"
    }[env["arch"]]


# ------------------------------------------------------ get CubismSdkForNative
def get_cubism_sdk(dirname: str = "CubismSdkForNative*"):
    list_path: list[Path] = [
        o_path for o_path in Path("thirdparty").glob(dirname) if o_path.is_dir()
    ]

    o_path = max(list_path)

    tpl_path = (o_path.joinpath("Core"), o_path.joinpath("Framework"))
    if all(map(lambda o: o.is_dir(), tpl_path)) is True:
        print("     CUBISM_NATIVE_CORE_DIR = {:s}".format(str(tpl_path[0])))
        print("CUBISM_NATIVE_FRAMEWORK_DIR = {:s}".format(str(tpl_path[1])))

        # ------------------------------------------------ Compile parameter(s)
        dict_CPPDEFINES = {}
        for param_n, param_v in (
            ("CUBISM_MOTION_CUSTOMDATA", "1"),
            ("COUNTERMEASURES_90017_90030", "1"),
        ):
            v = ARGUMENTS.get(param_n, param_v)
            print("{:>27s} = {:s}".format(param_n, v))
            if v == "1":
                dict_CPPDEFINES[param_n] = v

        env.Append(CPPDEFINES=dict_CPPDEFINES)
        return str(tpl_path[0]), str(tpl_path[1])

    print("*** Directory not found, 'CubismSdkForNative' ***")
    sys.exit(1)


#
def get_dylib_pathname(env) -> tuple[bool, Path | None]:
    o = glob(os.path.join(CUBISM_NATIVE_CORE_DIR, "dll", "**"), recursive=True)
    for v in o:
        if os.path.isdir(v) is True:
            continue
        if v.find(env["platform"]) == -1:
            continue
        if env["platform"] != "macos":
            if v.find(live2d_arch()) == -1:
                continue

        if os.path.splitext(v)[1] in (".lib", ".dylib", ".so"):
            if ARGUMENTS.get("GDCUBISM_DYLIB", "0") == "1":
                return True, Path(v)

    return False, None


# ---------------------------------------------- generate gd_cubism.gdextension
def generate_gdextension() -> configparser.ConfigParser:
    libraries = {
        "macos": {
            "arch": ["universal"],
            "target": ["debug", "release"],
            "tpl_k": "{0}.{2}",
            "tpl_v": '"res://addons/gd_cubism/bin/{0}/libgd_cubism.{0}.{2}.framework"',
        },
        "windows": {
            "arch": ["x86_64"],
            "target": ["debug", "release"],
            "tpl_k": "{0}.{2}.{1}",
            "tpl_v": '"res://addons/gd_cubism/bin/{0}/{1}/libgd_cubism.{0}.{2}.{1}.dll"',
        },
        "linux": {
            "arch": ["x86_64"],
            "target": ["debug", "release"],
            "tpl_k": "{0}.{2}.{1}",
            "tpl_v": '"res://addons/gd_cubism/bin/{0}/{1}/libgd_cubism.{0}.{2}.{1}.so"',
        },
        "ios": {
            "arch": ["universal"],
            "target": ["debug", "release"],
            "tpl_k": "{0}.{2}",
            "tpl_v": '"res://addons/gd_cubism/bin/{0}/libgd_cubism.{0}.{2}.dylib"',
        },
        "android": {
            "arch": ["arm32", "arm64"],
            "target": ["debug", "release"],
            "tpl_k": "{0}.{2}.{1}",
            "tpl_v": '"res://addons/gd_cubism/bin/{0}/{1}/libgd_cubism.{0}.{2}.{1}.so"',
        },
    }
    dependencies = {
        "macos": {
            "arch": ["universal"],
            "tpl_k": "{0}.{1}",
            "tpl_v": '{{"res://addons/gd_cubism/bin/{0}/libLive2DCubismCore.dylib": ""}}',
        },
        "windows": {
            "arch": ["x86_64"],
            "tpl_k": "{0}.{1}",
            "tpl_v": '{{"res://addons/gd_cubism/bin/{0}/{1}/Live2DCubismCore.dll": ""}}',
        },
        "linux": {
            "arch": ["x86_64"],
            "tpl_k": "{0}.{1}",
            "tpl_v": '{{"res://addons/gd_cubism/bin/{0}/{1}/libLive2DCubismCore.so": ""}}',
        },
    }

    gdextension = configparser.ConfigParser()

    gdextension.add_section("configuration")
    gdextension.set("configuration", "entry_symbol", '"gd_cubism_library_init"')
    gdextension.set("configuration", "compatibility_minimum", '"4.3"')

    gdextension.add_section("libraries")

    for plat, o in libraries.items():
        for arch in o["arch"]:
            for target in o["target"]:
                gdextension.set(
                    "libraries",
                    o["tpl_k"].format(plat, arch, target),
                    o["tpl_v"].format(
                        plat,
                        arch,
                        target,
                    ),
                )

    if GDCUBISM_DYLIB is True:
        gdextension.add_section("dependencies")

        for plat, o in dependencies.items():
            for arch in o["arch"]:
                gdextension.set(
                    "dependencies",
                    o["tpl_k"].format(plat, arch),
                    o["tpl_v"].format(plat, arch),
                )

    with open(GD_EXTENSION_PATH, "w", encoding="utf-8") as wf:
        gdextension.write(wf)

    return gdextension


CUBISM_NATIVE_CORE_DIR, CUBISM_NATIVE_FRAMEWORK_DIR = get_cubism_sdk()
GDCUBISM_DYLIB, GDCUBISM_DYLIB_PATHNAME = get_dylib_pathname(env)
conf_gdextension = generate_gdextension()

TARGET = "release" if env["target"] == "template_release" else "debug"


print("             GDCUBISM_DYLIB = {:d}".format(GDCUBISM_DYLIB))
print("    GDCUBISM_DYLIB_PATHNAME = {:s}".format(str(GDCUBISM_DYLIB_PATHNAME)))


# Add source files.
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=[os.path.join(CUBISM_NATIVE_CORE_DIR, "include")])


print("                   platform = {:s}".format(env["platform"]))
print("                       arch = {:s}".format(env["arch"]))
print("                live2d_arch = {:s}".format(live2d_arch()))
print("                     TARGET = {:s}".format(TARGET))


# --------------------------------------------------------------------- Windows
if env["platform"] == "windows":
    print(
        "               MSVC_VERSION = {:s}".format(
            env.get("MSVC_VERSION", "(undefined)")
        )
    )

    arch = live2d_arch()

    if GDCUBISM_DYLIB is True:
        o_cubism_lib = GDCUBISM_DYLIB_PATHNAME
    else:
        o_cubism_lib = (
            Path(CUBISM_NATIVE_CORE_DIR)
            .joinpath("lib")
            .joinpath(env["platform"])
            .joinpath(arch)
            .joinpath(env["MSVC_VERSION"].replace(".", ""))
            .joinpath("Live2DCubismCore_MT.lib")
        )

    print("                       libs = {:s}".format(str(o_cubism_lib)))
    if GDCUBISM_DYLIB is True:
        env.Append(LIBPATH=GDCUBISM_DYLIB_PATHNAME.parent)
        env.Append(LIBS=["Live2DCubismCore"])
    else:
        env.Append(
            LIBPATH=[
                Path(CUBISM_NATIVE_CORE_DIR)
                .joinpath("lib")
                .joinpath(env["platform"])
                .joinpath(arch)
                .joinpath(env["MSVC_VERSION"].replace(".", ""))
            ]
        )
        env.Append(LIBS=["Live2DCubismCore_MT"])


# ----------------------------------------------------------------------- macOS
elif env["platform"] == "macos":
    if GDCUBISM_DYLIB is True:
        o_cubism_lib = GDCUBISM_DYLIB_PATHNAME
    else:
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

    if GDCUBISM_DYLIB is True:
        env.Append(LIBPATH=GDCUBISM_DYLIB_PATHNAME.parent)
        env.Append(LINKFLAGS=["-Wl,-rpath,addons/gd_cubism/bin/macos"])
    else:
        env.Append(
            LIBPATH=[
                Path(CUBISM_NATIVE_CORE_DIR)
                .joinpath("lib")
                .joinpath(env["platform"])
                .joinpath(env["arch"])
            ]
        )

    env.Append(LIBS=["Live2DCubismCore"])


# ------------------------------------------------------------------------- iOS
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
            Path(CUBISM_NATIVE_CORE_DIR)
            .joinpath("lib")
            .joinpath(env["platform"])
            .joinpath("Release-iphoneos")
        ]
    )
    env.Append(LIBS=["Live2DCubismCore"])


# ----------------------------------------------------------------------- Linux
elif env["platform"] == "linux":
    arch = live2d_arch()

    if GDCUBISM_DYLIB is True:
        o_cubism_lib = GDCUBISM_DYLIB_PATHNAME
    else:
        o_cubism_lib = (
            Path(CUBISM_NATIVE_CORE_DIR)
            .joinpath("lib")
            .joinpath(env["platform"])
            .joinpath(arch)
            .joinpath("libLive2DCubismCore.a")
        )

    print("                       libs = {:s}".format(str(o_cubism_lib)))
    if GDCUBISM_DYLIB is True:
        env.Append(LIBPATH=GDCUBISM_DYLIB_PATHNAME.parent)
    else:
        env.Append(
            LIBPATH=[
                Path(CUBISM_NATIVE_CORE_DIR)
                .joinpath("lib")
                .joinpath(env["platform"])
                .joinpath(arch)
            ]
        )

    env.Append(LIBS=["Live2DCubismCore"])


# --------------------------------------------------------------------- android
elif env["platform"] == "android":
    arch = live2d_arch()

    o_cubism_lib = (
        Path(CUBISM_NATIVE_CORE_DIR)
        .joinpath("lib")
        .joinpath(env["platform"])
        .joinpath(arch)
        .joinpath("libLive2DCubismCore.a")
    )

    env.Append(
        LIBPATH=[
            os.path.join(
                CUBISM_NATIVE_CORE_DIR, "lib", "android", arch
            )
        ]
    )
    print("                       libs = {:s}".format(str(o_cubism_lib)))
    env.Append(LIBS=["Live2DCubismCore"])

else:
    print("!!! Unsupported platform !!!")
    sys.exit()

print("")

sources = glob("src/*.cpp")
sources += glob("src/private/*.cpp")
sources += glob("src/loaders/*.cpp")
sources += glob("src/importers/*.cpp")

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


#
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp", source=glob("doc_classes/*.xml")
        )
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")


# Find the addon path (e.g. project/addons/example).
addon_path = Path(GD_EXTENSION_PATH).parent


# TODO: Cache is disabled currently.
# scons_cache_path = os.environ.get("SCONS_CACHE")
# if scons_cache_path != None:
#     CacheDir(scons_cache_path)
#     print("Scons cache enabled... (path: '" + scons_cache_path + "')")

if env["platform"] in ("macos", "ios"):
    name = conf_gdextension.get(
        "libraries", "{0}.{2}".format(env["platform"], env["arch"], TARGET)
    )
else:
    name = conf_gdextension.get(
        "libraries",
        "{0}.{2}.{1}".format(env["platform"], env["arch"], TARGET),
    )

name = name.replace("res://addons/gd_cubism/", "")
library = env.SharedLibrary("{}/{}".format(addon_path, name[1:-1]), source=sources)


Default(library)
