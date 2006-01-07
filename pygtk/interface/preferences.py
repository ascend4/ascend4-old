# Preferences module for ASCPY.

import ConfigParser, os
import re

class Preferences:
	__sharedstate = {}

	def __init__(self):
		self.__dict__ = self.__sharedstate

		if not hasattr(self,'n'):
			self.n = 0;
			self.fname = os.path.expanduser("~/.ascend.ini");
		self.n = self.n + 1;

		if not hasattr(self,'ini'):
			print "CREATING CONFIG"
			self.ini = ConfigParser.SafeConfigParser();
			self.ini.read( [ self.fname ] );

	def __del__(self):
		self.n = self.n - 1;
		if self.n==0:
			print "WRITING CONFIG to",self.fname
			f = open(self.fname, "w");
			self.ini.write( f );

	def getGeometrySizePosition(self,displayname,key):
		try:
			_g = self.ini.get("Geometry:"+displayname,key)
		except ConfigParser.NoSectionError:
			return None
		except ConfigParser.NoOptionError:
			return None
		_p = re.compile('^\s*(\d+)[Xx](\d+)\+(\d+)\+(\d+)\s*$');

		_m = _p.match(_g)
		print "MATCH: ",_m.groups()
		return 	tuple(int(i) for i in _m.groups())

	def setGeometrySizePosition(self,displayname,key,width,height,top,left):
		if not self.ini.has_section("Geometry:"+displayname):
			self.ini.add_section("Geometry:"+displayname)
		self.ini.set("Geometry:"+displayname,key, "%dx%d+%d+%d" % (width, height, top, left) );

	def getGeometryValue(self,displayname,key):
		try:
			_g = self.ini.get("Geometry:"+displayname,key)
		except ConfigParser.NoSectionError:
			return None
		except ConfigParser.NoOptionError:
			return None
		return int(_g)

	def setGeometryValue(self,displayname,key,value):
		if not self.ini.has_section("Geometry:"+displayname):
			self.ini.add_section("Geometry:"+displayname)
		self.ini.set("Geometry:"+displayname,key,str(value))		

	def getPreferredUnits(self,key):
		try:
			_u = self.ini.get("PreferredUnits",key);
		except ConfigParser.NoSectionError:
			return None
		except ConfigParser.NoOptionError:
			return None
		return _u

	def setPreferredUnits(self,key,val):
		if not self.ini.has_section("PreferredUnits"):
			self.ini.add_section("PreferredUnits")
		self.ini.set("PreferredUnits",key,val)
		#print "SET PREFERRED UNITS FOR %s TO %s" % ( key, val )

# Test script:
def main():
	x = Preferences();
	y = Preferences();

	print "Units for length: ",x.getPreferredUnits("length");
	print "Units for time: ",x.getPreferredUnits("time");

	x.setPreferredUnits("length","m");
	y.setPreferredUnits("time","hr");
	print "Units for length: ",y.getPreferredUnits("length");
	print "Units for time: ",y.getPreferredUnits("time");

	print "About to delete x"
	del x;
	print "Deleted x"

	y.setPreferredUnits("length","cm");

	print "About to delete y"
	del y;

	print "Deleted y"

if __name__ == "__main__":
    main()
