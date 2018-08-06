import argparse
import os
import sys
import platform
import glob
import shutil
from subprocess import call

SUPPORTED_PLATFORMS = {
	"Darwin": True,
	"Linux": True
}

LIBRARY_EXTENSIONS = {
	"Darwin": "dylib",
	"Linux": "so"
}

DEBUGINFO_EXTENSIONS = {
	"Darwin": "dSYM"
}

BUILD_TARGETS = [
	"hl_cdll",
	"hl"
]

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

	parser.add_argument("--clean",
						help="Force a clean of the build directory",
						action="store_true")

	# TODO: A better way of doing this would be to import the engine build scripts
	# and call functions directly, so that we don't actually have to worry about
	# the interpreter name.
	parser.add_argument("--python-executable",
						help="Name of Python executable, if different from 'python', eg. 'python3'.",
						default="python")

	parser.add_argument("--reconfigure",
						help="Forces Cmake to be re-run for the engine.",
						action="store_true")

	return parser.parse_args()

def buildEngine(enginePath, buildDirectory, pythonExecutable, buildConfig, forceConfigure):
	print("Building engine located in:", enginePath)
	print("Build output:", buildDirectory)

	configureScriptPath = os.path.join(enginePath, "build-util", "create-build.py")
	makeScriptPath = os.path.join(enginePath, "build-util", "make-and-install.py")

	# Run cmake if no directory exists yet for the engine, or if we're forcibly reconfiguring.
	if forceConfigure or not os.path.isdir(buildDirectory):
		configureCallArgs = [pythonExecutable, configureScriptPath, "--build-dir", buildDirectory]

		if buildConfig == "debug":
			configureCallArgs.append("--debug")

		callProcess(configureCallArgs)

	oldPath = os.getcwd()
	os.chdir(buildDirectory)

	callProcess([pythonExecutable, makeScriptPath, "--no-install", "--build-dir", buildDirectory])

	os.chdir(oldPath)
	print("*** Engine build complete.")
	print()

def generateGameDebugInfo(gameMakefilePath, buildDirectory):
	print("Generating debug info for libraries...")

	libExtension = LIBRARY_EXTENSIONS[platform.system()]
	allLibs = glob.glob(os.path.join(buildDirectory, "*." + libExtension))

	for lib in allLibs:
		callProcess([gameMakefilePath + "/gendbg.sh", lib])

def buildGame(gameMakefilePath, buildDirectory, config, shouldClean):
	print("Building game via Makefile located in:", gameMakefilePath)
	print("Build output:", buildDirectory)

	os.makedirs(buildDirectory, exist_ok=True)

	oldPath = os.getcwd()
	os.chdir(gameMakefilePath)

	callProcess(["make", "CFG=" + config, "BUILD_DIR=\"" + buildDirectory + "\""] +
				(["clean"] if shouldClean else []) +
				BUILD_TARGETS)

	if config == "debug":
		generateGameDebugInfo(gameMakefilePath, buildDirectory)

	os.chdir(oldPath)
	print("*** Game build complete.")
	print()

def ensureIsDirectory(path):
	if not os.path.isdir(path):
		if os.path.isfile(path):
			os.remove(path)

		os.makedirs(path, exist_ok=True)

def copyGameContent(gameBuildPath, gameContentPath, engineGameLaunchPath, config):
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

	allLibs = glob.glob(os.path.join(gameBuildPath, "*." + libExtension))
	clientLibs = [lib for lib in allLibs if os.path.basename(lib).startswith("client")]
	serverLibs = [lib for lib in allLibs if not os.path.basename(lib).startswith("client")]

	clientLibDest = os.path.join(gameContentPathInEngine, "cl_dlls")
	ensureIsDirectory(clientLibDest)

	serverLibDest = os.path.join(gameContentPathInEngine, "dlls")
	ensureIsDirectory(serverLibDest)

	for lib in clientLibs:
		if os.path.isfile(lib):
			print("Copying client library", lib, "to", clientLibDest)
			shutil.copy2(lib, clientLibDest)

	for lib in serverLibs:
		if os.path.isfile(lib):
			print("Copying server library", lib, "to", serverLibDest)
			shutil.copy2(lib, serverLibDest)

	if config == "debug" and platform.system() in DEBUGINFO_EXTENSIONS:

		allDebugInfo = glob.glob(os.path.join(gameBuildPath, "*." + libExtension + "." + DEBUGINFO_EXTENSIONS[platform.system()]))
		clientDebugInfo = [item for item in allDebugInfo if os.path.basename(item).startswith("client")]
		serverDebugInfo = [item for item in allDebugInfo if not os.path.basename(item).startswith("client")]

		for item in clientDebugInfo:
			if os.path.isfile(item):
				print("Copying client debug info", item, "to", clientLibDest)
				shutil.copy2(item, clientLibDest)

		for item in serverDebugInfo:
			if os.path.isfile(item):
				print("Copying server debug info", item, "to", serverLibDest)
				shutil.copy2(item, serverLibDest)

	print("*** Game content copy complete.")
	print()

print("Platform detected:", platform.system())

if platform.system() not in SUPPORTED_PLATFORMS:
	print("Platform '" + platform.system() + "' is not currently supported by this build script.", file=sys.stderr)
	sys.exit(1)

scriptPath = os.path.dirname(os.path.realpath(sys.argv[0]))
buildBasePath = os.path.realpath(os.path.join(scriptPath, "build"))
engineBuildPath = os.path.join(buildBasePath, "engine")
engineGameLaunchPath = os.path.join(engineBuildPath, "game_launch")
gameBuildPath = os.path.join(buildBasePath, "game")
gameContentPath = os.path.join(scriptPath, "content", "afterburner")
enginePath = os.path.join(scriptPath, "dependencies", "afterburner-engine")
gameMakefilePath = os.path.join(scriptPath, "linux")

os.makedirs(buildBasePath, exist_ok=True)

args = parseCommandLineArguments()

buildEngine(enginePath, engineBuildPath, args.python_executable, args.config, args.reconfigure)
buildGame(gameMakefilePath, gameBuildPath, args.config, args.clean or args.reconfigure)
copyGameContent(gameBuildPath, gameContentPath, engineGameLaunchPath, args.config)

sys.exit(0)
