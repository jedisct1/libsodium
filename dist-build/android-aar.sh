#!/bin/sh

#creates an AAR with libsodium in 4 configurations all combinations of static | shared | minimal | full
#the x86 static library will not work due to text relocation rules and so all static x86 versions are just the shared library
#to simplify linking each version of the library is given a different name sodium, sodium-static, sodium-minimal and sodium-minimal-static

SODIUM_VERSION="1.0.18.0"
NDK_VERSION=$(grep "Pkg.Revision = " <"$ANDROID_NDK_HOME/source.properties" | cut -f 2 -d '=' | cut -f 2 -d' ' | cut -f 1 -d'.')
DEST_PATH=$(mktemp -d)

cd "$(dirname "$0")/../" || exit

make_abi_json() {
  if [ -z "$2" ]; then
    STL_VALUE="c++_shared"
  else
    STL_VALUE="c++_static"
  fi
  echo "{\"abi\":\"$NDK_ARCH\",\"api\":$SDK_VERSION,\"ndk\":$NDK_VERSION,\"stl\":\"$STL_VALUE\"}" >"$1/abi.json"
}

make_prefab_json() {
  echo "{\"name\":\"sodium\",\"schema_version\":1,\"dependencies\":[],\"version\":\"$SODIUM_VERSION\"}" >"$1/prefab.json"
}

make_manifest() {
  echo "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\" package=\"com.android.ndk.thirdparty.sodium\" android:versionCode=\"1\" android:versionName=\"1.0\">
	<uses-sdk android:minSdkVersion=\"19\" android:targetSdkVersion=\"21\"/>
</manifest>" >"$1/AndroidManifest.xml"

}

make_prefab_structure() {
  mkdir "$DEST_PATH"
  for i in "prefab" "prefab/modules" "META-INF"; do
    mkdir "$DEST_PATH/$i"
  done
  make_prefab_json "$DEST_PATH/prefab"
  make_manifest "$DEST_PATH"
  cp "LICENSE" "$DEST_PATH/META-INF"
  for i in "prefab/modules/sodium" "prefab/modules/sodium-static" "prefab/modules/sodium-minimal" "prefab/modules/sodium-minimal-static"; do
    mkdir "$DEST_PATH/$i"
    mkdir "$DEST_PATH/$i/libs"
    for j in "arm64-v8a" "armeabi-v7a" "x86" "x86_64"; do
      mkdir "$DEST_PATH/$i/libs/android.$j"
      mkdir "$DEST_PATH/$i/libs/android.$j/include"
      NDK_ARCH="$j"
      if [ $j = "arm64-v8a" ] || [ $j = "x86_64" ]; then
        SDK_VERSION="21"
      else
        SDK_VERSION="19"

      fi

      if [ $j != "x86" ]; then
        if [ $i = "prefab/modules/sodium-minimal-static" ] || [ $i = "prefab/modules/sodium-static" ]; then
          make_abi_json "$DEST_PATH/$i/libs/android.$j" static
        else
          make_abi_json "$DEST_PATH/$i/libs/android.$j"
        fi
      else
        make_abi_json "$DEST_PATH/$i/libs/android.$j"
      fi
    done
  done
}

copy_libs() {
  SRC_DIR="libsodium-android-$1"

  SHARED_DEST_DIR="$DEST_PATH/prefab/modules/sodium$3/libs/android.$2"
  STATIC_DEST_DIR="$DEST_PATH/prefab/modules/sodium$3-static/libs/android.$2"

  cp -r "$SRC_DIR/include" "$SHARED_DEST_DIR"
  cp -r "$SRC_DIR/include" "$STATIC_DEST_DIR"
  cp "$SRC_DIR/lib/libsodium.so" "$SHARED_DEST_DIR/libsodium$3.so"
  if [ "$2" = "x86" ]; then
    cp "$SRC_DIR/lib/libsodium.so" "$STATIC_DEST_DIR/libsodium$3-static.so"
  else
    cp "$SRC_DIR/lib/libsodium.a" "$STATIC_DEST_DIR/libsodium$3-static.a"
  fi
  rm -r "$SRC_DIR"
}

make_prefab_structure

"dist-build/android-all.sh"

copy_libs "armv7-a" "armeabi-v7a" "-minimal"
copy_libs "armv8-a+crypto" "arm64-v8a" "-minimal"
copy_libs "i686" "x86" "-minimal"
copy_libs "westmere" "x86_64" "-minimal"

LIBSODIUM_FULL_BUILD="Y"
export LIBSODIUM_FULL_BUILD

"dist-build/android-all.sh"

copy_libs "armv7-a" "armeabi-v7a"
copy_libs "armv8-a+crypto" "arm64-v8a"
copy_libs "i686" "x86"
copy_libs "westmere" "x86_64"

AAR_PATH="$(pwd)/libsodium-$SODIUM_VERSION.aar"
cd "$DEST_PATH" || exit
rm "$AAR_PATH"
zip -r "$AAR_PATH" META-INF prefab AndroidManifest.xml
cd .. || exit
rm -r "$DEST_PATH"
