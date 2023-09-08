#! /bin/sh

# Create an AAR with libsodium in all combinations of static | shared | minimal | full.
#
# The x86 static library will not work due to text relocation rules, so static x86 versions are limited to shared libraries.
# To simplify linking, library variants have distinct names: sodium, sodium-static, sodium-minimal and sodium-minimal-static.

SODIUM_VERSION="1.0.19.0"
NDK_VERSION=$(grep "Pkg.Revision = " <"${ANDROID_NDK_HOME}/source.properties" | cut -f 2 -d '=' | cut -f 2 -d' ' | cut -f 1 -d'.')
DEST_PATH=$(mktemp -d)

cd "$(dirname "$0")/../" || exit

make_abi_json() {
  echo "{\"abi\":\"${NDK_ARCH}\",\"api\":${SDK_VERSION},\"ndk\":${NDK_VERSION},\"stl\":\"none\"}" >"$1/abi.json"
}

make_prefab_json() {
  echo "{\"name\":\"sodium\",\"schema_version\":1,\"dependencies\":[],\"version\":\"$SODIUM_VERSION\"}" >"$1/prefab.json"
}

make_manifest() {
  echo "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\" package=\"com.android.ndk.thirdparty.sodium\" android:versionCode=\"1\" android:versionName=\"1.0\">
        <uses-sdk android:minSdkVersion=\"19\" android:targetSdkVersion=\"21\"/>
</manifest>" >"${1}/AndroidManifest.xml"
}

make_prefab_structure() {
  mkdir "$DEST_PATH"

  for variant_dirs in "prefab" "prefab/modules" "META-INF"; do
    mkdir "${DEST_PATH}/${variant_dirs}"
  done

  make_prefab_json "${DEST_PATH}/prefab"
  make_manifest "${DEST_PATH}"
  cp "LICENSE" "${DEST_PATH}/META-INF"

  for variant in \
    "prefab/modules/sodium" "prefab/modules/sodium-static" \
    "prefab/modules/sodium-minimal" "prefab/modules/sodium-minimal-static"; do
    mkdir "${DEST_PATH}/${variant}"

    if [ "$variant" = "prefab/modules/sodium-minimal" ]; then
      echo "{\"library_name\":\"libsodium\"}" >"${DEST_PATH}/${variant}/module.json"
    else
      echo "{}" >"${DEST_PATH}/${variant}/module.json"
    fi

    mkdir "${DEST_PATH}/${variant}/libs"

    for arch in "arm64-v8a" "armeabi-v7a" "x86" "x86_64"; do
      mkdir "$DEST_PATH/${variant}/libs/android.${arch}"
      mkdir "$DEST_PATH/${variant}/libs/android.${arch}/include"
      NDK_ARCH="$arch"
      if [ $arch = "arm64-v8a" ] || [ $arch = "x86_64" ]; then
        SDK_VERSION="21"
      else
        SDK_VERSION="19"
      fi

      make_abi_json "$DEST_PATH/${variant}/libs/android.${arch}"
    done
  done
}

copy_libs() {
  SRC_DIR="libsodium-android-${1}"

  SHARED_DEST_DIR="${DEST_PATH}/prefab/modules/sodium${3}/libs/android.${2}"
  STATIC_DEST_DIR="${DEST_PATH}/prefab/modules/sodium${3}-static/libs/android.${2}"

  cp -r "${SRC_DIR}/include" "$SHARED_DEST_DIR"
  cp -r "${SRC_DIR}/include" "$STATIC_DEST_DIR"
  cp "${SRC_DIR}/lib/libsodium.so" "${SHARED_DEST_DIR}/libsodium.so"
  cp "${SRC_DIR}/lib/libsodium.a" "${STATIC_DEST_DIR}/libsodium${3}-static.a"

  rm -r "$SRC_DIR"
}

build_all() {
  dist-build/android-armv7-a.sh
  dist-build/android-armv8-a.sh
  dist-build/android-x86_64.sh
  dist-build/android-x86.sh
}

make_prefab_structure

build_all

copy_libs "armv7-a" "armeabi-v7a" "-minimal"
copy_libs "armv8-a+crypto" "arm64-v8a" "-minimal"
copy_libs "i686" "x86" "-minimal"
copy_libs "westmere" "x86_64" "-minimal"

LIBSODIUM_FULL_BUILD="Y"
export LIBSODIUM_FULL_BUILD

build_all

copy_libs "armv7-a" "armeabi-v7a"
copy_libs "armv8-a+crypto" "arm64-v8a"
copy_libs "i686" "x86"
copy_libs "westmere" "x86_64"

AAR_PATH="$(pwd)/libsodium-${SODIUM_VERSION}.aar"
cd "$DEST_PATH" || exit
rm "$AAR_PATH"
zip -9 -r "$AAR_PATH" META-INF prefab AndroidManifest.xml
cd .. || exit
rm -r "$DEST_PATH"

echo
echo "Congrats you have built an AAR containing libsodium! To use it with
gradle or cmake (as set by default for Android Studio projects):

- Edit the app/build.gradle file to add:

    android {
        buildFeatures {
            prefab true
        }
    }

  and

    dependencies {
        implementation fileTree(dir:'path/to/aar/',include:['libsodium-$SODIUM_VERSION.aar'])
    }

  Optionally, store multiple AAR files in the same folder and include '*.aar'

- Edit your module's CMakeLists.txt file to add:

    find_package(sodium REQUIRED CONFIG)

- Then, specify 'sodium::x' as an item in the relevant 'target_link_libraries' statement.
    The first part is the AAR name and should be 'sodium'.
    The second part ('x', to be replaced) should be set to:
      - 'sodium' for the full shared library,
      - 'sodium-static' for the full static library
      - 'sodium-minimal' for the minimal shared library, or
      - 'sodium-minimal-static' for the minimal static library."
