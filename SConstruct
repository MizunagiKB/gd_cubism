from glob import glob
from pathlib import Path

env = SConscript("godot-cpp/SConstruct")

# Add source files.
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=["thirdparty/CubismSdkForNative/Core/include"])

if env["platform"] == "windows":
    env.Append(
        LIBPATH=[
            "thirdparty/CubismSdkForNative/Core/lib/windows/{:s}/{:s}".format(
                env["arch"], env["MSVC_VERSION"].replace(".", "")
            )
        ]
    )
    env.Append(LIBS=["Live2DCubismCore_MT"])
elif env["platform"] == "macos":
    env.Append(LIBPATH=["thirdparty/CubismSdkForNative/Core/lib/macos"])
    env.Append(LIBS=["Live2DCubismCore.universal"])
else:
    pass

sources = glob("src/*.cpp")
sources += glob("src/private/*.cpp")

env.Append(CPPPATH=["thirdparty/CubismNativeFramework/src"])

sources_cubism = glob("thirdparty/CubismNativeFramework/src/*.cpp")

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
        "thirdparty/CubismNativeFramework/src/{:s}/*.cpp".format(dirname)
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
