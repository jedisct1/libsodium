#! /bin/sh

# Check that a self-contained, single-file .NET app embeds libsodium for a given runtime identifier
# Usage: test-single-file.sh <directory containing the nupkg> <runtime identifier>

set -e

if [ $# -ne 2 ]; then
  echo "Usage: $0 <directory containing the nupkg> <runtime identifier>" >&2
  exit 1
fi

PACKAGE_DIR=$(cd "$1" && pwd)
RID="$2"
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
WORK_DIR=$(mktemp -d)
trap 'rm -rf "$WORK_DIR"' EXIT

# nuget.org may have a package with the same version number.
# A private cache guarantees that the package being tested is the local one.
export NUGET_PACKAGES="${WORK_DIR}/packages"

dotnet new console -n Tests -o "${WORK_DIR}/Tests"
dotnet add "${WORK_DIR}/Tests/Tests.csproj" package libsodium -s "$PACKAGE_DIR"
cp -f "${SCRIPT_DIR}/test.cs" "${WORK_DIR}/Tests/Program.cs"

publish() {
  dotnet publish "${WORK_DIR}/Tests/Tests.csproj" -c Release -r "$RID" \
    --self-contained true -p:PublishSingleFile=true "$@"
}

# Without self-extraction, the native library must be published next to the
# executable.
# If it isn't, the package has nothing for that runtime identifier, and the
# check below would pass for the wrong reason.
publish -p:IncludeNativeLibrariesForSelfExtract=false -o "${WORK_DIR}/publish-unbundled"
if [ -z "$(find "${WORK_DIR}/publish-unbundled" -name 'libsodium*')" ]; then
  echo "The package doesn't provide libsodium for ${RID}" >&2
  exit 1
fi

publish -p:IncludeNativeLibrariesForSelfExtract=true -o "${WORK_DIR}/publish"
ls -l "${WORK_DIR}/publish"
if [ -n "$(find "${WORK_DIR}/publish" -name 'libsodium*')" ]; then
  echo "libsodium was not embedded into the single-file executable for ${RID}" >&2
  exit 1
fi
echo "libsodium is embedded into the single-file executable for ${RID}"
