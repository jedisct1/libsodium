#!/bin/sh

#creates an AAR with libsodium in 4 configurations all combinations of static | shared | minimal | full
#the x86 static library will not work due to text relocation rules and so all static x86 versions are just the shared library
#to simplify linking each version of the library is given a different name sodium, sodium-static, sodium-minimal and sodium-minimal-static

SODIUM_VERSION="1.0.18.0"
NDK_VERSION=$(grep "Pkg.Revision = " <"$ANDROID_NDK_HOME/source.properties" | cut -f 2 -d '=' | cut -f 2 -d' ' | cut -f 1 -d'.')
DEST_PATH=$(mktemp -d)

cd "$(dirname "$0")/../" || exit

make_abi_json() {
  echo "{\"abi\":\"$NDK_ARCH\",\"api\":$SDK_VERSION,\"ndk\":$NDK_VERSION,\"stl\":\"none\"}" >"$1/abi.json"
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
    if [ "$i" = "prefab/modules/sodium-minimal" ]; then
      echo "{\"library_name\":\"libsodium\"}" >"$DEST_PATH/$i/module.json"
    else
      echo "{}" >"$DEST_PATH/$i/module.json"
    fi
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

      make_abi_json "$DEST_PATH/$i/libs/android.$j"

    done
  done
}

copy_libs() {
  SRC_DIR="libsodium-android-$1"

  SHARED_DEST_DIR="$DEST_PATH/prefab/modules/sodium$3/libs/android.$2"
  STATIC_DEST_DIR="$DEST_PATH/prefab/modules/sodium$3-static/libs/android.$2"

  cp -r "$SRC_DIR/include" "$SHARED_DEST_DIR"
  cp -r "$SRC_DIR/include" "$STATIC_DEST_DIR"
  cp "$SRC_DIR/lib/libsodium.so" "$SHARED_DEST_DIR/libsodium.so"
  cp "$SRC_DIR/lib/libsodium.a" "$STATIC_DEST_DIR/libsodium$3-static.a"

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

AAR_PATH="$(pwd)/libsodium-$SODIUM_VERSION.aar"
cd "$DEST_PATH" || exit
rm "$AAR_PATH"
zip -r "$AAR_PATH" META-INF prefab AndroidManifest.xml
cd .. || exit
rm -r "$DEST_PATH"
clear
echo "congrats you have built an AAR containing libsodium. To use it with 
gradle / cmake (defaults for Android Studio projects) simply:

edit the app/build.gradle file to add:

    android {
        buildFeatures {
            prefab true
        }
    }
    
    #and
    dependencies {
        implementation fileTree(dir:'path/to/aar/',include:['libsodium-$SODIUM_VERSION.aar']) 
    }
    #you can optionally store multiple AAR files in the same folder and include '*.aar'

edit your module's CMakeLists.txt file to add:
    
    find_package(sodium REQUIRED CONFIG)
    
    # the specify sodium::sodium as an item in the relevant target_link_libraries statement
    # the first value specifies the AAR and is always sodium the second is the library 
    # name which supports sodium for the full shared library sodium-static
    # for the full static library sodium-minimal for the minimal shared library
    # or sodium-minimal-static for the minimal static library"

