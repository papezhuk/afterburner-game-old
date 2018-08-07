import argparse
import os
import sys
import platform
import glob
import shutil
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

	# TODO: A better way of doing this would be to import the engine build scripts
	# and call functions directly, so that we don't actually have to worry about
	# the interpreter name.
	parser.add_argument("--python-executable",
						help="Name of Python executable, if different from 'python', eg. 'python3'.",
						default="python")

	return parser.parse_args()

def buildEngine(enginePath, buildDirectory, pythonExecutable, buildConfig, forceRebuild):
	print("Building engine located in:", enginePath)
	print("Build output:", buildDirectory)

	configureScriptPath = os.path.join(enginePath, "build-util", "create-build.py")
	makeScriptPath = os.path.join(enginePath, "build-util", "make-and-install.py")

	# Run cmake if no directory exists yet for the engine, or if we're forcibly rebuilding.
	if forceRebuild or not os.path.isdir(buildDirectory):
		configureCallArgs = [pythonExecutable, configureScriptPath, "--no-vgui", "--build-dir", buildDirectory]

		if buildConfig == "debug":
			configureCallArgs.append("--debug")

		callProcess(configureCallArgs)

	oldPath = os.getcwd()
	os.chdir(buildDirectory)

	callProcess([pythonExecutable, makeScriptPath, "--no-install", "--build-dir", buildDirectory])

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

	copyLibraries(os.path.join(gameBuildPath, "cl_dll"), os.path.join(gameContentPathInEngine, "cl_dlls"), libExtension)
	copyLibraries(os.path.join(gameBuildPath, "dlls"), os.path.join(gameContentPathInEngine, "dlls"), libExtension)

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

os.makedirs(buildBasePath, exist_ok=True)

args = parseCommandLineArguments()

buildEngine(enginePath, engineBuildPath, args.python_executable, args.config, args.rebuild_engine)
buildGame(scriptPath, gameBuildPath, args.config, args.rebuild_engine or args.rebuild_game)
copyGameContent(gameBuildPath, gameContentPath, engineGameLaunchPath, args.config)

sys.exit(0)
