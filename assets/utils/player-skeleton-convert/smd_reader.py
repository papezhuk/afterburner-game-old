class SMDReader():
	def __init__(self, fileName):
		self.__fileName = fileName
		self.__file = None
		self.__currentLine = 0

	def __enter__(self):
		self.__file = open(self.__fileName, "r")
		self.__currentLine = 1
		return self

	def __exit__(self, *args):
		if self.__file is not None:
			self.__file.close()

		self.__currentLine = 0

	def expect(self, expectedLine):
		line = self.__readLine()
		lineNo = line[0]
		text = line[1].rstrip("\r\n")

		if text != expectedLine:
			raise AssertionError(f"File {self.__fileName}, line {lineNo}: expected '{expectedLine}' but got '{text}'.")

	def __readLine(self):
		ret = (self.__currentLine, self.__file.readline())

		if ret[1] == "":
			raise OSError(f"Reached end of file {self.__fileName}")

		self.__currentLine += 1
		return ret
