#!/usr/bin/python
import sys
import os
		
class LibraryBuilder:

	rules = "'canvas : stybuilder SRC_FOLDER OUTPUT_FILENAME'"
	
	def __init__(self,dir):
		self.dir = dir + "/"
		self.content = ""
		
	def assemble(self, out):
		self.content = self.concat()
		self.write_result(out)
			
	def concat(self):
		pipe = os.popen('ls -1 "' + self.dir + '"')
		files = pipe.read()
		pipe.close()
		files = files.strip()
		files = files.split("\n")
		buffer = ""
		for file in files:
			if os.path.isdir(self.dir+'/'+file):
				sub = LibraryBuilder(self.dir+'/'+file)
				buffer += sub.concat()

			elif file[-4:] == '.sty':
				stream = open(self.dir+'/'+file,'r')
				for line in stream:
					buffer += line
				buffer += "\n"
				stream.close()
		
		return buffer
		
	def write_result(self, out):
		output = open(out,'w')
		output.write(self.content)
		output.close()
			
	
	@staticmethod
	def displayRules():
		print(LibraryBuilder.rules)
	
if __name__ == '__main__':
	if (len(sys.argv) >= 3):
		dir = sys.argv[1]
		out = sys.argv[2]
		LibraryBuilder(dir).assemble(out)
		
	else:
		LibraryBuilder.displayRules()
