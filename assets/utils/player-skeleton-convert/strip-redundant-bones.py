import argparse

# I think the dummy bones are something that 3DS Max put into the
# original model for helper purposes? Easiest just to remove them
# as they don't correspond to anything in Nightfire models.
# Other generic "bones" don't appear to do anything, and the toe
# bones are present in Half Life DM models but not Nightfire.
REMOVE_BONES = \
{
	"Dummy16": True,
	"Dummy11": True,
	"Dummy21": True,
	"Dummy08": True,
	"Bone02": True,
	"Bone03": True,
	"Bone04": True,
	"Dummy05": True,
	"Bone09": True,
	"Bone10": True,
	"Dummy04": True,
	"Bone05": True,
	"Bone06": True,
	"Dummy03": True,
	"Bone07": True,
	"Bone08": True,
	"Dummy09": True,
	"Bone11": True,
	"Bone12": True,
	"Dummy10": True,
	"Bone13": True,
	"Bone14": True,
	"Bone15": True,
	"Dummy06": True,
	"Dummy07": True,
	"Dummy01": True,
	"Dummy02": True,
	"Bip01 L Toe01": True,
	"Bip01 L Toe02": True,
	"Bip01 R Toe01": True,
	"Bip01 R Toe02": True
}

def __parseArgs():
	parser = argparse.ArgumentParser(description="Script for removing unneeded bones from HLDM models.")
	parser.add_argument("--aniqc",
						help="QC file that lists animation SMDs.")
	parser.add_argument("--boneqc",
						help="QC file that lists bones.")

	return parser.parse_args()

def main():
	args = __parseArgs()
	# TODO

if __name__ == "__main__":
	main()