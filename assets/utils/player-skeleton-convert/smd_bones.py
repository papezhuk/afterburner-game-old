class SMDBone():
	def __init__(self, index=0, name="", parent=None):
		self.setIndex(index)
		self.setName(name)
		self.setParent(parent)

	def __repr__(self):
		return f"SMDBone({self.__index}, \"{self.__name}\", {self.__parent.index() if self.__parent is not None else -1})"

	def name(self):
		return self.__name

	def setName(self, newName):
		if not isinstance(newName, str):
			raise TypeError("Name must be a string.")

		self.__name = newName

	def index(self):
		return self.__index

	def setIndex(self, newIndex):
		if not isinstance(newIndex, int):
			raise TypeError("Index must be an integer.")

		self.__index = newIndex

	def parent(self):
		return self.__parent

	def setParent(self, newParent):
		if newParent is not None and not isinstance(newParent, SMDBone):
			raise TypeError("Parent must be an SMDBone, or None.")

		self.__parent = newParent

class SMDBoneList():
	def __init__(self, boneList):
		if not isinstance(boneList, list):
			raise TypeError("Bones must be provided as a list.")

		self.__list = boneList

	def list(self):
		return self.__list

	def setList(self, newList):
		if not isinstance(newList, list):
			raise TypeError("Bones must be provided as a list.")

		self.__list = newList

	def getByName(self, name):
		for bone in self.__list:
			if bone.name() == name:
				return bone

		return None