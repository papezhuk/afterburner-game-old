def __writeLine(outFile, line):
	outFile.write(line + "\n")

def __writeBones(outFile, bones):
	__writeLine(outFile, "nodes")

	for bone in bones.list():
		__writeLine(outFile, f" {bone.index()} \"{bone.name()}\"  {bone.parent().index() if bone.parent() is not None else -1}")

	__writeLine(outFile, "end")

def __writeSkeleton(outFile, skeleton):
	__writeLine(outFile, "skeleton")
	__writeLine(outFile, "time 0")

	for bone in skeleton:
		outString = " "

		for index in range(0, len(bone)):
			if index == 0:
				outString += str(bone[index])
			else:
				outString += " {0:.6f}".format(bone[index])

		__writeLine(outFile, outString)

	__writeLine(outFile, "end")

def __writeTriangles(outFile, triangles):
	__writeLine(outFile, "triangles")

	for triangle in triangles:
		__writeLine(outFile, triangle[0])

		for index in range(1, len(triangle)):
			__writeLine(outFile, " " + (" ".join([str(item) for item in triangle[index]])))

	__writeLine(outFile, "end")

def write(fileName, bones, skeleton, triangles):
	with open(fileName, "w") as outFile:
		__writeLine(outFile, "version 1")
		__writeBones(outFile, bones)
		__writeSkeleton(outFile, skeleton)
		__writeTriangles(outFile, triangles)