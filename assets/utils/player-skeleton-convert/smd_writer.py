def __writeLine(outFile, line):
	outFile.write(line + "\n")

def __writeBones(outFile, bones):
	__writeLine(outFile, "nodes")

	for bone in bones.list():
		__writeLine(outFile, f"  {bone.index()} {bone.name()}  {bone.parent().index() if bone.parent() is not None else -1}")

	__writeLine(outFile, "end")

def __writeSkeleton(outFile, skeleton):
	__writeLine(outFile, "skeleton")
	__writeLine(outFile, "time 0")

	for bone in skeleton:
		__writeLine(outFile, "  " + (" ".join([str(item) for item in bone])))

	__writeLine(outFile, "end")

def __writeTriangles(outFile, triangles):
	__writeLine(outFile, "triangles")

	for triangle in triangles:
		__writeLine(outFile, triangle[0])

		for index in range(1, len(triangle)):
			# The spacing isn't exactly as the example SMD uses here, but hopefully that doesn't matter.
			__writeLine(outFile, " " + (" ".join([str(item) for item in triangle[index]])))

	__writeLine(outFile, "end")

def write(fileName, bones, skeleton, triangles):
	with open(fileName, "w") as outFile:
		__writeLine(outFile, "version 1")
		__writeBones(outFile, bones)
		__writeSkeleton(outFile, skeleton)
		__writeTriangles(outFile, triangles)