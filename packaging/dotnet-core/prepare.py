#!/usr/bin/python3

import datetime
import json
import os.path
import sys
import xml.etree.ElementTree as ET

#
# OUTPUT CONFIGURATION
#

# Package metadata
NUPKG_ID = 'libsodium'
NUPKG_VERSION = '1.0.11'

# The names of the libsodium binaries in the package
LIBSODIUM_DLL   = 'libsodium.dll'
LIBSODIUM_DYLIB = 'libsodium.dylib'
LIBSODIUM_SO    = 'libsodium.so'

#
# INPUT CONFIGURATION
#

# The archives to download
WIN_FILE = 'libsodium-1.0.11-msvc.zip'
DEB_FILE = 'libsodium18_1.0.11-1_amd64.deb'
RPM_FILE = 'libsodium18-1.0.11-14.1.x86_64.rpm'
OSX_FILE = 'libsodium-1.0.11.{0}.bottle.tar.gz'

# The URLs of the archives
OFFICIAL_URL = 'https://download.libsodium.org/libsodium/releases/{0}'
OPENSUSE_URL = 'http://download.opensuse.org/repositories/home:/nsec/{0}/{1}/{2}'
HOMEBREW_URL = 'https://bintray.com/homebrew/bottles/download_file?file_path={0}'

# The files within the archives to extract
WIN_LIB = '{0}/Release/v140/dynamic/libsodium.dll'
DEB_LIB = './usr/lib/x86_64-linux-gnu/libsodium.so.18.1.1'
RPM_LIB = './usr/lib64/libsodium.so.18.1.1'
OSX_LIB = 'libsodium/1.0.11/lib/libsodium.18.dylib'

# Commands to extract a file from an archive
DEB_EXTRACT = 'ar -p {0} data.tar.xz | tar xJ "{1}"'
RPM_EXTRACT = 'rpm2cpio {0} | cpio -i "{1}"'
TAR_EXTRACT = 'tar xzf {0} "{1}"'
ZIP_EXTRACT = 'unzip {0} "{1}"'

# The inputs
INPUTS = [

  ( 'win10-x64',
    WIN_FILE,
    OFFICIAL_URL.format(WIN_FILE),
    WIN_LIB.format('x64'),
    ZIP_EXTRACT,
    LIBSODIUM_DLL),

  ( 'win10-x86',
    WIN_FILE,
    OFFICIAL_URL.format(WIN_FILE),
    WIN_LIB.format('Win32'),
    ZIP_EXTRACT,
    LIBSODIUM_DLL),

  ( 'debian.8-x64',
    DEB_FILE,
    OPENSUSE_URL.format('Debian_8.0', 'amd64', DEB_FILE),
    DEB_LIB,
    DEB_EXTRACT,
    LIBSODIUM_SO),

  ( 'ubuntu.14.04-x64',
    DEB_FILE,
    OPENSUSE_URL.format('xUbuntu_14.04', 'amd64', DEB_FILE),
    DEB_LIB,
    DEB_EXTRACT,
    LIBSODIUM_SO),

  ( 'ubuntu.16.04-x64',
    DEB_FILE,
    OPENSUSE_URL.format('xUbuntu_16.04', 'amd64', DEB_FILE),
    DEB_LIB,
    DEB_EXTRACT,
    LIBSODIUM_SO),

  ( 'ubuntu.16.10-x64',
    DEB_FILE,
    OPENSUSE_URL.format('xUbuntu_16.10', 'amd64', DEB_FILE),
    DEB_LIB,
    DEB_EXTRACT,
    LIBSODIUM_SO),

  ( 'centos.7-x64',
    RPM_FILE,
    OPENSUSE_URL.format('CentOS_7', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'fedora.23-x64',
    RPM_FILE,
    OPENSUSE_URL.format('Fedora_23', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'fedora.24-x64',
    RPM_FILE,
    OPENSUSE_URL.format('Fedora_24', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'fedora.25-x64',
    RPM_FILE,
    OPENSUSE_URL.format('Fedora_25', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'opensuse.42.1-x64',
    RPM_FILE,
    OPENSUSE_URL.format('openSUSE_Leap_42.1', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'opensuse.42.2-x64',
    RPM_FILE,
    OPENSUSE_URL.format('openSUSE_Leap_42.2', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'rhel.7-x64',
    RPM_FILE,
    OPENSUSE_URL.format('RHEL_7', 'x86_64', RPM_FILE),
    RPM_LIB,
    RPM_EXTRACT,
    LIBSODIUM_SO),

  ( 'osx.10.10-x64',
    OSX_FILE.format('yosemite'),
    HOMEBREW_URL.format(OSX_FILE.format('yosemite')),
    OSX_LIB,
    TAR_EXTRACT,
    LIBSODIUM_DYLIB),

  ( 'osx.10.11-x64',
    OSX_FILE.format('el_capitan'),
    HOMEBREW_URL.format(OSX_FILE.format('el_capitan')),
    OSX_LIB,
    TAR_EXTRACT,
    LIBSODIUM_DYLIB),

  ( 'osx.10.12-x64',
    OSX_FILE.format('sierra'),
    HOMEBREW_URL.format(OSX_FILE.format('sierra')),
    OSX_LIB,
    TAR_EXTRACT,
    LIBSODIUM_DYLIB),

]

# The version cookie
COOKIE_FILE = 'version.json'

#
# INTERMEDIATE FILES
#

CACHEDIR = 'cache'
TEMPDIR = 'build'

#
# DO NOT EDIT BELOW THIS LINE
#

class Item:
  def __init__(self, input, cachedir, tempdir):
    rid, archive, url, file, extract, lib = input

    self.rid = rid
    self.archive = archive
    self.url = url
    self.file = file
    self.extract = extract
    self.lib = lib

    self.cachefile = os.path.join(cachedir, rid, archive)
    self.sourcedir = os.path.join(tempdir, rid)
    self.sourcefile = os.path.join(tempdir, rid, os.path.normpath(file))
    self.targetfile = os.path.join('runtimes', rid, 'native', lib)

def create_nuspec(template, nuspec, version, items):
  tree = ET.parse(template)
  package = tree.getroot()
  metadata = package.find('metadata')
  metadata.find('version').text = version
  files = package.find('files')
  for item in items:
    ET.SubElement(files, 'file', src=item.sourcefile, target=item.targetfile).tail = '\n'
  tree.write(nuspec, 'ascii', '<?xml version="1.0"?>')

def create_makefile(makefile, nupkg, nuspec, items):
  with open(makefile, 'w') as f:
    for item in items:
      f.write('FILES += {0}\n'.format(item.sourcefile))
    f.write('\n')
    f.write('{0}: {1} $(FILES)\n\tdotnet nuget pack $<\n'.format(nupkg, nuspec))
    for item in items:
      f.write('\n')
      f.write('{0}:\n\t@mkdir -p $(dir $@)\n\tcurl -f#Lo $@ "{1}"\n'.format(item.cachefile, item.url))
    for item in items:
      f.write('\n')
      f.write('{0}: {1}\n\t@mkdir -p $(dir $@)\n\tcd {2} && {3}\n'.format(
        item.sourcefile,
        item.cachefile,
        item.sourcedir,
        item.extract.format(os.path.relpath(item.cachefile, item.sourcedir), item.file)))

def make_prerelease_version(version, suffix, cookie_file):
  cookies = dict()
  if os.path.isfile(cookie_file):
    with open(cookie_file, 'r') as f:
      cookies = json.load(f)
  cookie = cookies.get(suffix, '---').split('-')
  year, month, day, *rest = datetime.datetime.utcnow().timetuple()
  major = '{0:03}{1:02}'.format(year * 12 + month - 23956, day)
  minor = int(cookie[3]) + 1 if cookie[:3] == [version, suffix, major] else 1
  result = '{0}-{1}-{2}-{3:02}'.format(version, suffix, major, minor)
  cookies[suffix] = result
  with open(cookie_file, 'w') as f:
    json.dump(cookies, f, indent=4, sort_keys=True)
  return result

def main(args):
  if len(args) > 2 or len(args) > 1 and not args[1].isalpha:
    print('usage: {0} [label]'.format(os.path.basename(args[0])))
    sys.exit(1)

  version = NUPKG_VERSION

  if len(args) > 1:
    suffix = args[1].lower()
  else:
    suffix = 'preview'

  if suffix != 'release':
    version = make_prerelease_version(version, suffix, COOKIE_FILE)
    print('updated', COOKIE_FILE)

  template = NUPKG_ID + '.nuspec'
  nuspec = NUPKG_ID + '.' + version + '.nuspec'
  nupkg = NUPKG_ID + '.' + version + '.nupkg'

  tempdir = os.path.join(TEMPDIR, version)
  items = [Item(input, CACHEDIR, tempdir) for input in INPUTS]

  create_nuspec(template, nuspec, version, items)
  print('created', nuspec)

  create_makefile('Makefile', nupkg, nuspec, items)
  print('created', 'Makefile', 'to make', nupkg)

if __name__ == '__main__':
  main(sys.argv)
