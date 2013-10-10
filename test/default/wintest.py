import sys
import os
import subprocess

test_files = [
	"auth",
	"auth2",
	"auth3",
	"auth5",
	"box",
	"box2",
	"box7",
	"box8",
	"core1",
	"core2",
	"core3",
	"core4",
	"core5",
	"core6",
	"generichash",
	"generichash2",
	"hash",
	"hash3",
	"onetimeauth",
	"onetimeauth2",
	"onetimeauth7",
	"randombytes",
	"scalarmult",
	"scalarmult2",
	"scalarmult5",
	"scalarmult6",
	"secretbox",
	"secretbox2",
	"secretbox7",
	"secretbox8",
	"shorthash",
	"sodium_core",
	"sodium_utils",
	"sodium_version",
	"stream",
	"stream2",
	"stream3",
	"stream4",
	"stream5",
	"stream6"]

build_types = ["Release", "ReleaseDLL", "Debug", "DebugDLL"]
architectures = ["Win32", "x64"]
path = os.environ['PATH']

if len(sys.argv) not in [3]:
	print "Usage: %s <Release | ReleaseDLL | Debug | DebugDLL> <Win32 | x64>" % (sys.argv[0])
	sys.exit(0)

for filename in test_files:
	if not os.path.exists("%s.c" % (filename)):
		print "%s not found in current directory" % (filename)
		sys.exit(-1)

if sys.argv[1] not in build_types:
	print "Invalid build type %s" % (sys.argv[1])
	exit(-2)

if sys.argv[2] not in architectures:
	print "Invalid architecture %s" % (sys.argv[2])
	sys.exit(-3)

dev_null = open(os.devnull, 'wb')
build_type = sys.argv[1]
arch = sys.argv[2]
compiler = "cl.exe"
cflags = r' /nologo /D_CRT_SECURE_NO_WARNINGS /DTEST_SRCDIR=\".\" /I..\..\src\libsodium\include\sodium /I..\..\src\libsodium\include /I..\quirks '
ldflags = r' /link advapi32.lib ..\..\Build' + '\\' + build_type + '\\' + arch + r'\libsodium.lib '

if   build_type == "Release":
	cflags += ' /MT /Ox /W3 /Oi /Ot /GL /DSODIUM_STATIC '
elif build_type == "Debug":
	cflags += ' /GS /MTd /Od /W3 /DSODIUM_STATIC '
elif build_type == "ReleaseDLL":
	cflags += ' /MD /Ox /W3 /Oi /Ot /GL '
	os.environ['PATH'] = r'..\..\Build' + '\\' + build_type + '\\' + arch + ';' + path
elif build_type == "DebugDLL":
	cflags += ' /MDd /Od /W3 '
	os.environ['PATH'] = r'..\..\Build' + '\\' + build_type + '\\' + arch + ';' + path

try:
	for test in test_files:
		code = subprocess.call(" ".join([compiler,cflags,"%s.c" % (test),ldflags,'/OUT:%s.exe' % (test)]), stdout=dev_null, stderr=None)
		if code != 0:
			print "error compiling %s" % (test)
			raise
		code = subprocess.call([test], stdout=dev_null, stderr=dev_null)
		if code == 0:
			print "%s: ok" % (test)
		else:
			print "%s: fail" % (test)
except KeyboardInterrupt:
	print "Aborted by user"
except:
	print "Fatal error. cl.exe not in path or wrong architecture?"

for test in test_files:
	for ext in ['exe', 'obj', 'res', 'pdb']:
		try:
			os.remove("%s.%s" % (test,ext))
		except:
			pass

os.environ['PATH'] = path