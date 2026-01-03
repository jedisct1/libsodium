#! /bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

errors=0
warnings=0

error() {
    echo -e "${RED}ERROR:${NC} $1"
    errors=$((errors + 1))
}

warn() {
    echo -e "${YELLOW}WARNING:${NC} $1"
    warnings=$((warnings + 1))
}

ok() {
    echo -e "${GREEN}OK:${NC} $1"
}

# Extract version from configure.ac (source of truth)
if [ ! -f configure.ac ]; then
    echo "ERROR: configure.ac not found. Run from repository root."
    exit 1
fi

VERSION=$(sed -n 's/^AC_INIT(\[libsodium\],\[\([0-9]*\.[0-9]*\.[0-9]*\)\].*/\1/p' configure.ac)
MAJOR=$(sed -n 's/^SODIUM_LIBRARY_VERSION_MAJOR=\([0-9]*\)/\1/p' configure.ac)
MINOR=$(sed -n 's/^SODIUM_LIBRARY_VERSION_MINOR=\([0-9]*\)/\1/p' configure.ac)
DLL_VERSION=$(sed -n 's/^DLL_VERSION=\([0-9]*\)/\1/p' configure.ac)
LIBTOOL_VERSION=$(sed -n 's/^SODIUM_LIBRARY_VERSION=\([0-9]*:[0-9]*:[0-9]*\)/\1/p' configure.ac)

if [ -z "$VERSION" ] || [ -z "$MAJOR" ] || [ -z "$MINOR" ]; then
    echo "ERROR: Could not extract version from configure.ac"
    exit 1
fi

echo "Source of truth (configure.ac):"
echo "  VERSION: $VERSION"
echo "  SODIUM_LIBRARY_VERSION_MAJOR: $MAJOR"
echo "  SODIUM_LIBRARY_VERSION_MINOR: $MINOR"
echo "  DLL_VERSION: $DLL_VERSION"
echo "  SODIUM_LIBRARY_VERSION (libtool): $LIBTOOL_VERSION"
echo ""

# Check DLL_VERSION matches MAJOR
if [ "$DLL_VERSION" != "$MAJOR" ]; then
    error "DLL_VERSION ($DLL_VERSION) should match SODIUM_LIBRARY_VERSION_MAJOR ($MAJOR)"
else
    ok "DLL_VERSION matches MAJOR"
fi

echo ""
echo "Checking version files..."
echo ""

# 1. src/libsodium/include/sodium/version.h
FILE="src/libsodium/include/sodium/version.h"
if [ -f "$FILE" ]; then
    VH_VERSION=$(sed -n 's/.*#define SODIUM_VERSION_STRING "\([0-9]*\.[0-9]*\.[0-9]*\)".*/\1/p' "$FILE")
    VH_MAJOR=$(sed -n 's/.*#define SODIUM_LIBRARY_VERSION_MAJOR \([0-9]*\).*/\1/p' "$FILE")
    VH_MINOR=$(sed -n 's/.*#define SODIUM_LIBRARY_VERSION_MINOR \([0-9]*\).*/\1/p' "$FILE")

    if [ "$VH_VERSION" != "$VERSION" ]; then
        error "$FILE: SODIUM_VERSION_STRING is '$VH_VERSION', expected '$VERSION'"
    else
        ok "$FILE: SODIUM_VERSION_STRING"
    fi

    if [ "$VH_MAJOR" != "$MAJOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MAJOR is '$VH_MAJOR', expected '$MAJOR'"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MAJOR"
    fi

    if [ "$VH_MINOR" != "$MINOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MINOR is '$VH_MINOR', expected '$MINOR'"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MINOR"
    fi
else
    warn "$FILE not found (may need to run ./configure first)"
fi

# 2. builds/msvc/version.h (should be copy of version.h)
FILE="builds/msvc/version.h"
if [ -f "$FILE" ]; then
    MVH_VERSION=$(sed -n 's/.*#define SODIUM_VERSION_STRING "\([0-9]*\.[0-9]*\.[0-9]*\)".*/\1/p' "$FILE")
    MVH_MAJOR=$(sed -n 's/.*#define SODIUM_LIBRARY_VERSION_MAJOR \([0-9]*\).*/\1/p' "$FILE")
    MVH_MINOR=$(sed -n 's/.*#define SODIUM_LIBRARY_VERSION_MINOR \([0-9]*\).*/\1/p' "$FILE")

    if [ "$MVH_VERSION" != "$VERSION" ]; then
        error "$FILE: SODIUM_VERSION_STRING is '$MVH_VERSION', expected '$VERSION'"
    else
        ok "$FILE: SODIUM_VERSION_STRING"
    fi

    if [ "$MVH_MAJOR" != "$MAJOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MAJOR is '$MVH_MAJOR', expected '$MAJOR'"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MAJOR"
    fi

    if [ "$MVH_MINOR" != "$MINOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MINOR is '$MVH_MINOR', expected '$MINOR'"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MINOR"
    fi
else
    error "$FILE not found"
fi

# 3. builds/msvc/resource.rc
FILE="builds/msvc/resource.rc"
if [ -f "$FILE" ]; then
    RC_VERSION=$(sed -n 's/.*#define LIBSODIUM_VERSION_STRING "\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' "$FILE")
    RC_BIN=$(sed -n 's/.*#define LIBSODIUM_VERSION_BIN \([0-9]*,[0-9]*,[0-9]*\).*/\1/p' "$FILE")
    EXPECTED_BIN=$(echo "$VERSION" | tr '.' ',')

    if [ "$RC_VERSION" != "$VERSION" ]; then
        error "$FILE: LIBSODIUM_VERSION_STRING is '$RC_VERSION', expected '$VERSION'"
    else
        ok "$FILE: LIBSODIUM_VERSION_STRING"
    fi

    if [ "$RC_BIN" != "$EXPECTED_BIN" ]; then
        error "$FILE: LIBSODIUM_VERSION_BIN is '$RC_BIN', expected '$EXPECTED_BIN'"
    else
        ok "$FILE: LIBSODIUM_VERSION_BIN"
    fi
else
    error "$FILE not found"
fi

# 4. ci/appveyor/msvc-scripts/process.bat
FILE="ci/appveyor/msvc-scripts/process.bat"
if [ -f "$FILE" ]; then
    BAT_VERSION=$(sed -n 's/.*@VERSION@\/\([0-9]*\.[0-9]*\.[0-9]*\)\/.*/\1/p' "$FILE")
    BAT_MAJOR=$(sed -n 's/.*@SODIUM_LIBRARY_VERSION_MAJOR@\/\([0-9]*\)\/.*/\1/p' "$FILE")
    BAT_MINOR=$(sed -n 's/.*@SODIUM_LIBRARY_VERSION_MINOR@\/\([0-9]*\)\/.*/\1/p' "$FILE")

    if [ "$BAT_VERSION" != "$VERSION" ]; then
        error "$FILE: VERSION is '$BAT_VERSION', expected '$VERSION'"
    else
        ok "$FILE: VERSION"
    fi

    if [ "$BAT_MAJOR" != "$MAJOR" ]; then
        error "$FILE: MAJOR is '$BAT_MAJOR', expected '$MAJOR'"
    else
        ok "$FILE: MAJOR"
    fi

    if [ "$BAT_MINOR" != "$MINOR" ]; then
        error "$FILE: MINOR is '$BAT_MINOR', expected '$MINOR'"
    else
        ok "$FILE: MINOR"
    fi
else
    error "$FILE not found"
fi

# 5. appveyor.yml
FILE="appveyor.yml"
if [ -f "$FILE" ]; then
    AV_VERSION=$(sed -n 's/^version: \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' "$FILE")

    if [ "$AV_VERSION" != "$VERSION" ]; then
        error "$FILE: version is '$AV_VERSION', expected '$VERSION'"
    else
        ok "$FILE: version"
    fi
else
    error "$FILE not found"
fi

# 6. dist-build/android-aar.sh
FILE="dist-build/android-aar.sh"
if [ -f "$FILE" ]; then
    AAR_VERSION=$(sed -n 's/^SODIUM_VERSION="\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' "$FILE")

    if [ "$AAR_VERSION" != "$VERSION" ]; then
        error "$FILE: SODIUM_VERSION is '$AAR_VERSION', expected '$VERSION'"
    else
        ok "$FILE: SODIUM_VERSION"
    fi
else
    error "$FILE not found"
fi

# 7. packaging/dotnet-core/libsodium.pkgproj
FILE="packaging/dotnet-core/libsodium.pkgproj"
if [ -f "$FILE" ]; then
    PKG_VERSION=$(sed -n 's/.*<Version>\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' "$FILE")

    if [ "$PKG_VERSION" != "$VERSION" ]; then
        error "$FILE: Version is '$PKG_VERSION', expected '$VERSION'"
    else
        ok "$FILE: Version"
    fi
else
    error "$FILE not found"
fi

# 8. packaging/nuget/package.config (uses 4-part version: X.Y.Z.0)
FILE="packaging/nuget/package.config"
if [ -f "$FILE" ]; then
    NUGET_VERSION=$(sed -n 's/.*version *= *"\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' "$FILE")
    NUGET_PATHVER=$(sed -n 's/.*pathversion="\([0-9_]*\)".*/\1/p' "$FILE")
    EXPECTED_PATHVER="${VERSION}_0"
    EXPECTED_PATHVER=$(echo "$EXPECTED_PATHVER" | tr '.' '_')

    if [ "$NUGET_VERSION" != "$VERSION" ]; then
        error "$FILE: version is '$NUGET_VERSION', expected '$VERSION'"
    else
        ok "$FILE: version"
    fi

    if [ "$NUGET_PATHVER" != "$EXPECTED_PATHVER" ]; then
        error "$FILE: pathversion is '$NUGET_PATHVER', expected '$EXPECTED_PATHVER'"
    else
        ok "$FILE: pathversion"
    fi
else
    error "$FILE not found"
fi

# 9. configure (generated from configure.ac)
FILE="configure"
if [ -f "$FILE" ]; then
    CONF_VERSION=$(sed -n "s/^PACKAGE_VERSION='\([0-9]*\.[0-9]*\.[0-9]*\)'/\1/p" "$FILE")
    CONF_MAJOR=$(sed -n 's/^SODIUM_LIBRARY_VERSION_MAJOR=\([0-9]*\)/\1/p' "$FILE")
    CONF_MINOR=$(sed -n 's/^SODIUM_LIBRARY_VERSION_MINOR=\([0-9]*\)/\1/p' "$FILE")

    if [ "$CONF_VERSION" != "$VERSION" ]; then
        error "$FILE: PACKAGE_VERSION is '$CONF_VERSION', expected '$VERSION' (run ./autogen.sh)"
    else
        ok "$FILE: PACKAGE_VERSION"
    fi

    if [ "$CONF_MAJOR" != "$MAJOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MAJOR is '$CONF_MAJOR', expected '$MAJOR' (run ./autogen.sh)"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MAJOR"
    fi

    if [ "$CONF_MINOR" != "$MINOR" ]; then
        error "$FILE: SODIUM_LIBRARY_VERSION_MINOR is '$CONF_MINOR', expected '$MINOR' (run ./autogen.sh)"
    else
        ok "$FILE: SODIUM_LIBRARY_VERSION_MINOR"
    fi
else
    warn "$FILE not found (run ./autogen.sh to generate)"
fi

echo ""
echo "=========================================="
if [ $errors -gt 0 ]; then
    echo -e "${RED}FAILED:${NC} $errors error(s), $warnings warning(s)"
    exit 1
elif [ $warnings -gt 0 ]; then
    echo -e "${YELLOW}PASSED WITH WARNINGS:${NC} $warnings warning(s)"
    exit 0
else
    echo -e "${GREEN}PASSED:${NC} All version checks passed"
    exit 0
fi
