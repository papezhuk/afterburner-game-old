import argparse
import smd_writer
import os
from smd_reader import SMDReader

def __parseArgs():
	parser = argparse.ArgumentParser(description="Script for removing unneeded bones from HLDM models.")
	parser.add_argument("--template",
						help="SMD file that lists valid bone structure.")
	parser.add_argument("smd",
						action="append",
						help="SMD files to modify bones for")

	return parser.parse_args()

def __parseBoneTemplate(filePath):
	print("Reading allowed bone template from:", filePath)

	with SMDReader(filePath) as reader:
		reader.expect("version 1")
		return reader.readBones()

def __parseSmdBonesAndSkeleton(filePath):
	with SMDReader(filePath) as reader:
		reader.expect("version 1")
		return (reader.readBones(), reader.readAllSkeletonFrames())

def __computeBonesToRemove(bones, allowedBoneNames):
	disallowedBoneNames = {bone.name(): True for bone in bones.list()}

	for name in allowedBoneNames:
		if name in disallowedBoneNames:
			del disallowedBoneNames[name]

	return disallowedBoneNames.keys()

def __removeBones(bones, boneNamesToRemove):
	removalMap = {name: True for name in boneNamesToRemove}
	boneList = bones.list()

	# Iterate backwards to avoid issues with shifting indices.
	for index in range(len(boneList) - 1, -1, -1):
		bone = boneList[index]
		if bone.name() in removalMap:
			del boneList[index]

def __modifySkeletonFrame(frame, boneRenumberMap):
	bones = frame["bones"]

	# Reverse iteration to avoid index shifting issues.
	for index in range(len(bones) - 1, -1, -1):
		entry = bones[index]
		boneIndex = entry[0]

		if boneIndex in boneRenumberMap:
			tempList = list(entry)
			tempList[0] = boneRenumberMap[boneIndex]
			bones[index] = tuple(tempList)
		else:
			del bones[index]
			continue

def __modifySkeleton(skeleton, boneRenumberMap):
	for frame in skeleton:
		__modifySkeletonFrame(frame, boneRenumberMap)

def __outputFilePath(filePath):
	directory = os.path.normpath(os.path.dirname(filePath))
	fileName = os.path.basename(filePath)

	outputDirectory = os.path.join(directory, "output")
	if not os.path.isdir(outputDirectory):
		os.mkdir(outputDirectory)

	return os.path.join(outputDirectory, fileName)

def __modifySmdFile(allowedBoneNames, filePath):
	print("Modifying SMD file:", filePath)

	(bones, skeleton) = __parseSmdBonesAndSkeleton(filePath)

	bonesToRemove = __computeBonesToRemove(bones, allowedBoneNames)
	__removeBones(bones, bonesToRemove)
	boneRenumberMap = bones.renumber()
	__modifySkeleton(skeleton, boneRenumberMap)

	outputPath = __outputFilePath(filePath)
	smd_writer.write(outputPath, bones, skeleton)

def __modifySmdDirectory(allowedBoneNames, dirPath):
	print("Finding SMD files in directory:", dirPath)

	for item in os.listdir(dirPath):
		fullPath = os.path.join(dirPath, item)
		(_, fileExt) = os.path.splitext(os.path.basename(item))
		if fileExt == ".smd" and os.path.isfile(fullPath):
			__modifySmdFile(allowedBoneNames, fullPath)

def __modifySmdByPath(allowedBoneNames, path):
	if os.path.isdir(path):
		__modifySmdDirectory(allowedBoneNames, os.path.abspath(path))
	else:
		__modifySmdFile(allowedBoneNames, os.path.abspath(path))

def main():
	args = __parseArgs()
	boneList = __parseBoneTemplate(args.template)

	allowedBoneNames = [bone.name() for bone in boneList.list()]
	print("Allowed bones:", "\n".join(["  " + item for item in allowedBoneNames]))

	for smdPath in args.smd:
		__modifySmdByPath(allowedBoneNames, smdPath)

if __name__ == "__main__":
	main()