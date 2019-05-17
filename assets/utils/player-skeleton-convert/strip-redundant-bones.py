import argparse
from smd_reader import SMDReader

def __parseArgs():
	parser = argparse.ArgumentParser(description="Script for removing unneeded bones from HLDM models.")
	parser.add_argument("--template",
						help="SMD file that lists valid bone structure.")
	parser.add_argument("smd",
						action="append",
						nargs='*',
						help="SMD files to modify bones for")

	return parser.parse_args()

def __parseBoneTemplate(filePath):
	with SMDReader(filePath) as reader:
		reader.expect("version 1")
		return reader.readBones()

def __modifySmd(boneList, filePath):
	pass

def main():
	args = __parseArgs()
	boneList = __parseBoneTemplate(args.template)

	for smdPath in args.smd:
		__modifySmd(boneList, smdPath)

if __name__ == "__main__":
	main()