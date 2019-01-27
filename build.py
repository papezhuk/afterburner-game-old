# Waf seems pretty cool, now that Xash3D has introduced me to it.
# At some point this Python/CMake hybrid should be moved over to it.

import argparse
import os
import sys
import platform
import glob
import shutil
import runpy
from subprocess import call

SUPPORTED_PLATFORMS = [
	"Darwin",
	"Linux"
]

LIBRARY_EXTENSIONS = {
	"Darwin": "dylib",
	"Linux": "so"
}

def callProcess(args):
	print("Calling: `" + " ".join(args) + "`")

	returnCode = call(args)

	if returnCode != 0:
		print("Process call", args[0], "returned error code", returnCode, file=sys.stderr)
		sys.exit(1)

	print("Process call", args[0], "completed successfully.")

def parseCommandLineArguments():
	parser = argparse.ArgumentParser(description="Afterburner build script.")

	parser.add_argument("-c", "--config",
						help="Config for build.",
						default="release",
						choices=["debug", "release"])

	parser.add_argument("--rebuild-game",
						help="Cleans and rebuilds game libraries.",
						action="store_true")

	parser.add_argument("--rebuild-engine",
						help="Cleans and rebuilds engine libraries.",
						action="store_true")

	return parser.parse_args()

def buildEngine(enginePath, outputDirectory, buildConfig, forceRebuild):
	print("Building engine located in:", enginePath)
	print("Build output:", outputDirectory)

	wafBuildDir = os.path.join(enginePath, "build")

	oldPath = os.getcwd()
	os.chdir(enginePath)

	if forceRebuild:
		if os.path.exists(outputDirectory):
			shutil.rmtree(outputDirectory)

		if os.path.exists(wafBuildDir):
			shutil.rmtree(wafBuildDir)

		callProcess(["python",
					"waf",
					"configure",
					"--disable-vgui",
					f"--build-type={buildConfig}",
					"--win-style-install",
					f"--prefix={outputDirectory}"])

	callProcess(["python", "waf", "build"])
	callProcess(["python", "waf", "install"])

	os.chdir(oldPath)
	print("*** Engine build complete.")
	print()

def buildGame(gamePath, buildDirectory, config, forceRebuild):
	print("Building game located in:", gamePath)
	print("Build output:", buildDirectory)

	if forceRebuild and os.path.exists(buildDirectory):
		shutil.rmtree(buildDirectory)

	os.makedirs(buildDirectory, exist_ok=True)

	oldPath = os.getcwd()
	os.chdir(buildDirectory)

	cmakeArgs = ["cmake"]

	if config == "debug":
		cmakeArgs.append("-DCMAKE_BUILD_TYPE=Debug")

	cmakeArgs.append(gamePath)
	callProcess(cmakeArgs)

	callProcess(["make", "-j8"])

	os.chdir(oldPath)
	print("*** Game build complete.")
	print()

def ensureIsDirectory(path):
	if not os.path.isdir(path):
		if os.path.isfile(path):
			os.remove(path)

		os.makedirs(path, exist_ok=True)

def copyLibraries(sourcePath, destinationPath, extension):
	allLibs = glob.glob(os.path.join(sourcePath, "*." + extension))

	ensureIsDirectory(destinationPath)

	for lib in allLibs:
		if os.path.isfile(lib):
			print("Copying library", lib, "to", destinationPath)
			shutil.copy2(lib, destinationPath)

def copyGameContent(scriptPath, gameBuildPath, gameContentPath, engineGameLaunchPath, config):
	gameContentPathInEngine = os.path.join(engineGameLaunchPath, "afterburner")

	print("Copying game content from", gameContentPath, "to", gameContentPathInEngine)

	if os.path.exists(gameContentPathInEngine):
		print("Removing old content tree...")
		if os.path.isdir(gameContentPathInEngine):
			shutil.rmtree(gameContentPathInEngine)
		else:
			os.remove(gameContentPathInEngine)

	print("Copying new content tree...")
	shutil.copytree(gameContentPath, gameContentPathInEngine,
					ignore=shutil.ignore_patterns("git-track-me"))	# Ignore files that are only there for Git to see

	print("Copying game libraries from", gameBuildPath, "to", gameContentPathInEngine)

	libExtension = LIBRARY_EXTENSIONS[platform.system()]

	copyLibraries(os.path.join(gameBuildPath, "cl_dll"), os.path.join(gameContentPathInEngine, "cl_dlls"), libExtension)
	copyLibraries(os.path.join(gameBuildPath, "dlls"), os.path.join(gameContentPathInEngine, "dlls"), libExtension)

	if platform.system() == "Linux":
		srcPath = os.path.join(scriptPath, "xash3d-gdb.py")
		print("Copying", srcPath, "to", engineGameLaunchPath)
		shutil.copy2(srcPath, engineGameLaunchPath)

	print("*** Game content copy complete.")
	print()

print("Platform detected:", platform.system())

if platform.system() not in SUPPORTED_PLATFORMS:
	print("Platform '" + platform.system() + "' is not currently supported by this build script.", file=sys.stderr)
	sys.exit(1)

scriptPath = os.path.dirname(os.path.realpath(sys.argv[0]))
buildBasePath = os.path.realpath(os.path.join(scriptPath, "build"))
engineOutputPath = os.path.join(buildBasePath, "engine")
gameBuildPath = os.path.join(buildBasePath, "game")
gameContentPath = os.path.join(scriptPath, "content", "afterburner")
enginePath = os.path.join(scriptPath, "dependencies", "afterburner-engine")

os.makedirs(buildBasePath, exist_ok=True)

args = parseCommandLineArguments()

buildEngine(enginePath, engineOutputPath, args.config, args.rebuild_engine)
buildGame(scriptPath, gameBuildPath, args.config, args.rebuild_engine or args.rebuild_game)
copyGameContent(scriptPath, gameBuildPath, gameContentPath, engineOutputPath, args.config)

sys.exit(0)
