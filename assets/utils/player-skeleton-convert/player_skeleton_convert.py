# This script is for converting the skeleton of a Nightfire player model
# to use the same bone configuration as a HLDM player model.
# The HLDM animations can then be used instead.

import argparse
import os
import sys
import smd_writer
from smd_reader import SMDReader
from smd_bones import SMDBone

def __parseArgs():
	parser = argparse.ArgumentParser(description="Script for converting Nightfire player model skeleton to HLDM.")
	parser.add_argument("ref", help="Reference SMD file to edit.")
	parser.add_argument("-o", "--output", default=None, help="Output file to write to. Default is input name with '_new' suffix.")

	return parser.parse_args()

# Re-parents leg bones to from spine to pelvis,
# which is the correct parent in Half Life meshes.
def __reparentLegs(bones):
	print("Re-parenting leg bones")

	pelvisBone = bones.getByName("Bip01 Pelvis")

	if pelvisBone is None:
		raise ValueError(f"Could not find bone: 'Bip01 Pelvis'.")

	for index in range(0, 2):
		side = "L" if index == 0 else "R"
		legBoneName = f"Bip01 {side} Thigh"

		bone = bones.getByName(legBoneName)

		if bone is None:
			raise ValueError(f"Could not find bone '{legBoneName}'.")

		bone.setParent(pelvisBone)

# Remaps fingers 2, 3, and 4 to just be 1, as is the case in Half Life.
def __calculateFingerRemap(bones, boneMap):
	print("Remapping finger bones")

	for index in range(0, 2):
		side = "L" if index == 0 else "R"

		handBones = []

		for boneIndex in range(0, 3):
			boneName = f"Bip01 {side} Finger1{boneIndex if boneIndex > 0 else ''}"
			bone = bones.getByName(boneName)

			if bone is None:
				raise ValueError(f"Could not find bone '{boneName}'.")

			handBones.append(bone)

		# Thumb and index finger are fine to remain as-is.
		# The rest must be remapped. The index finger bones
		# will be repurposed as general hand bones.

		for fingerIndex in range(2, 5):
			for subIndex in range(0, 3):
				boneName = f"Bip01 {side} Finger{fingerIndex}{subIndex if subIndex > 0 else ''}"
				bone = bones.getByName(boneName)

				if bone is None:
					raise ValueError(f"Could not find bone '{boneName}'.")

				# Map the index of this bone to the corresponding index of the general hand bones.
				boneMap[bone.index()] = handBones[subIndex].index()

# Creates a "PlayerMesh" bone, which may or may not be used by HLDM
# (but I'm playing it safe).
def __createPlayerMeshBone(bones):
	print("Creating Player_Mesh bone")

	pelvis = bones.getByName("Bip01 Pelvis")
	if pelvis is None:
		raise ValueError(f"Could not find bone 'Bip01 Pelvis'.")

	bones.list().append(SMDBone(len(bones.list()), "Player_Mesh", pelvis))

# Creates the corresponding skeleton entry for the new Player_Mesh bone.
def __createPlayerMeshSkeletonEntry(bones, skeleton):
	print("Creating Player_Mesh skeleton entry")

	bone = bones.getByName("Player_Mesh")
	if bone is None:
		raise ValueError(f"Could not find bone 'Player_Mesh'.")

	# Gonna assume that the HLDM position is fine for Nightfire models too.
	# The model proportions seem to be the same anyway.
	skeleton.append((bone.index(), 3.279230, 2.542519, -1.277027, -1.571592, -1.570451, 0.000000))

# Given remapped finger bones, reorders bones and writes their new mappings.
# Assumes that original finger bones should no longer exist in the model.
def __generateBoneMap(bones, fingerRemap, boneMap, inverseBoneMap):
	print("Mapping and re-ordering bones")

	boneList = bones.list()
	renumber = 0

	for index in range(0, len(boneList)):
		if index not in fingerRemap:
			# Bone still exists in new set, so add.
			boneMap[index] = renumber			# Map to renumbered index.
			inverseBoneMap[renumber] = index	# Record new to old as well.
			renumber += 1

	for index in fingerRemap:
		# Add mappings from old to new bones.
		# Note that boneMap holds renumbered indices now,
		# So we need to translate through that.
		boneMap[index] = boneMap[fingerRemap[index]]
		boneList[index] = None

	# Update all indices on the physical bone items.
	# We do this backwards because we remove invalid bones.
	for index in range(len(boneList) - 1, -1, -1):
		bone = boneList[index]
		if bone is None:
			del boneList[index]
			continue

		# Should never happen:
		if bone.index() in fingerRemap:
			raise AssertionError()

		bone.setIndex(boneMap[bone.index()])

# Renumbers skeleton items due to bone re-ordering.
def __renumberSkeletonEntries(skeleton, boneList, inverseBoneMap):
	print("Renumbering skeleton entries")

	newSkeleton = []

	for bone in boneList.list():
		# Get the original skeleton entry index that this bone referenced
		# before it was re-ordered.
		skeletonIndex = inverseBoneMap[bone.index()]

		# Update the index of this entry to point to the new bone.
		item = list(skeleton[skeletonIndex])
		item[0] = bone.index()

		# Add to new skeleton list.
		newSkeleton.append(tuple(item))

	return newSkeleton

# Changes bones that triangle vertices map to, due to bone re-ordering.
def __remapTriangleBones(boneMap, triangles):
	print("Remapping triangle bones")

	for index in range(0, len(triangles)):
		triangle = list(triangles[index])

		for subIndex in range(1, len(triangle)):
			vertexDef = list(triangle[subIndex])
			vertexDef[0] = boneMap[vertexDef[0]]
			triangle[subIndex] = tuple(vertexDef)

		triangles[index] = tuple(triangle)

def __readSmd(refSmd):
	with SMDReader(refSmd) as reader:
		reader.expect("version 1")

		bones = reader.readBones()
		__createPlayerMeshBone(bones)
		__reparentLegs(bones)

		fingerRemap = {}
		__calculateFingerRemap(bones, fingerRemap)

		boneMap = {}
		inverseBoneMap = {}
		__generateBoneMap(bones, fingerRemap, boneMap, inverseBoneMap)

		skeleton = reader.readSkeleton()
		__createPlayerMeshSkeletonEntry(bones, skeleton)
		skeleton = __renumberSkeletonEntries(skeleton, bones, inverseBoneMap)

		triangles = reader.readTriangles()
		__remapTriangleBones(boneMap, triangles)

		return (bones, skeleton, triangles)

def main():
	args = __parseArgs()

	refSmd = args.ref

	if not os.path.isfile(refSmd):
		print("Reference SMD", refSmd, "was not found on disk.", file=sys.stderr)
		sys.exit(1)

	if args.output is None:
		fileWithExt = os.path.splitext(os.path.basename(refSmd))
		args.output = fileWithExt[0] + "_new" + fileWithExt[1]

	print("Converting", refSmd, "to", args.output)
	(bones, skeleton, triangles) = __readSmd(refSmd)

	print("Writing", args.output)
	smd_writer.write(args.output, bones, skeleton, triangles)

if __name__ == "__main__":
	main()
