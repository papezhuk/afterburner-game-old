# This script is for converting the skeleton of a Nightfire player model
# to use the same bone configuration as a HLDM player model.
# The HLDM animations can then be used instead.

import argparse
import os
import sys
from smd_reader import SMDReader

def __parseArgs():
	parser = argparse.ArgumentParser(description="Script for converting Nightfire player model skeleton to HLDM.")
	parser.add_argument("ref", help="Reference SMD file to edit.")
	parser.add_argument("-o", "--output", default=None, help="Output file to write to. Default is input name with '_new' suffix.")

	return parser.parse_args()

def __readSmd(reader):
	reader.expect("version 1")
	reader.expect("nodes")

def main():
	args = __parseArgs()

	refSmd = args.ref

	if not os.path.isfile(refSmd):
		print("Reference SMD", refSmd, "was not found on disk.", file=sys.stderr)
		sys.exit(1)

	if args.output is None:
		fileWithExt = os.path.splitext(os.path.basename(refSmd))
		args.output = fileWithExt[0] + "_new" + fileWithExt[1]

	outputSmd = args.output

	with SMDReader(refSmd) as reader:
		__readSmd(reader)

if __name__ == "__main__":
	main()
