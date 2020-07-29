#! /usr/bin/env python3

# This script regenerates TIS CI configuration.

# Run from the root of the libsodium project:
# $ python3 trustinsoft/regen-trustinsoft.py

import re # Regular expressions.
from itertools import product # Cartesian product of lists.
import json # JSON generation.
import os # Write to files.
import shutil # Copy file.

# Outputting JSON.
def string_of_json(obj):
    # Output standard pretty-printed JSON (RFC 7159) with 4-space indentation.
    s = json.dumps(obj,indent=4)
    # Sometimes we need to have multiple "include" fields in the outputted JSON,
    # which is unfortunately impossible in the internal python representation
    # (OK, it is technically possible, but too cumbersome to bother implementing
    # it here), so we can name these fields 'include_', 'include__', etc, and
    # they are all converted to 'include' before outputting as JSON.
    s = re.sub(r'"include_+"', '"include"', s)
    return s

# Generated files which need to be a part of the repository.
files_to_copy = [
    {
        'src': 'src/libsodium/include/sodium/version.h',
        'dst': 'trustinsoft/sodium/version.h'
    }
]

# ---------------------------------------------------------------------------- #
# ------------------------------ INITIAL CHECKS ------------------------------ #
# ---------------------------------------------------------------------------- #

print("1. Check if needed directories and files exist...")

def check_dir(dir):
    if os.path.isdir(dir):
        print("   > OK! Directory '%s' exists." % dir)
    else:
        exit("Directory '%s' not found." % dir)

def check_file(file):
    if os.path.isfile(file):
        print("   > OK! File '%s' exists." % file)
    else:
        exit("File '%s' not found." % file)

check_dir('trustinsoft')
check_dir('src/libsodium')
for file in files_to_copy:
    check_file(file['src'])

# ---------------------------------------------------------------------------- #
# ------------------ GENERATE trustinsoft/<machdep>.config ------------------- #
# ---------------------------------------------------------------------------- #

machdeps = [
    { "machdep": "gcc_x86_64", "address-alignment": 64 }
]

def string_of_options(options):
    s = ''
    beginning = True
    for option_prefix in options:
        for option_val in options[option_prefix]:
            if beginning:
                beginning = False # No need for a separator at the beginning.
            else:
                s += ' '
            s += option_prefix + option_val
    return s

def make_compilation_cmd(machdep):
    return string_of_options({
        "-I": [
            ".",
            "sodium",
            "../src/libsodium/include",
            "../src/libsodium/include/sodium",
            "../test/quirks"
        ],
        "-D": [
            "'getrandom(B, S, F)=tis_getrandom(B, S, F)'",
            "__TIS_MKFS_PREALLOCATE",
            "volatile=",
            "HAVE_GETRANDOM",
            "CONFIGURED",
            "DEV_MODE=1",
            "NO_BLOCKING_RANDOM_POLL"
        ],
        "-U": [
            "HAVE_EXPLICIT_BZERO",
            "HAVE_INLINE_ASM"
        ]
    })

def make_machdep_config(machdep):
    return {
        "machdep": machdep["machdep"],
        "address-alignment": machdep["address-alignment"],
        "compilation_cmd": make_compilation_cmd(machdep)
    }

machdep_configs = map(make_machdep_config, machdeps)

print("2. Generate 'trustinsoft/<machdep>.config' files...")
for machdep_config in machdep_configs:
    filename = "trustinsoft/%s.config" % machdep_config["machdep"]
    with open(filename, 'w') as f:
        print("   > Generate the '%s' file." % filename)
        f.write(string_of_json(machdep_config))

# ---------------------------------------------------------------------------- #
# -------------------- GENERATE trustinsoft/common.config -------------------- #
# ---------------------------------------------------------------------------- #

import glob

def make_common_config_files():
    src_files = list(
        map(lambda file: file.replace("src/libsodium", "../src/libsodium"),
            sorted(glob.iglob("src/libsodium/**/*.c", recursive=True))))
    return ([ "stub.c" ] + src_files)

def make_common_config_filesystem():
    exp_files = list(
        map(lambda file: {
            "name": file.replace("test/default/", "./"),
            "from": file.replace("test/default/", "../test/default/")
        },
        sorted(glob.iglob("test/default/*.exp", recursive=False))))
    return {
        "files": exp_files
    }

common_config = {
    "no-results": "true",
    "val-timeout": 3600,
    "files": make_common_config_files(),
    "filesystem": make_common_config_filesystem()
}

with open('trustinsoft/common.config', 'w') as f:
    print("3. Generate the 'trustinsoft/common.config' file.")
    f.write(string_of_json(common_config))

# ---------------------------------------------------------------------------- #
# -------------------------------- tis.config -------------------------------- #
# ---------------------------------------------------------------------------- #

tests = [
    "aead_aes256gcm",
    "aead_aes256gcm2",
    "aead_chacha20poly1305",
    "aead_chacha20poly13052",
    "aead_xchacha20poly1305",
    "auth",
    "auth2",
    "auth3",
    "auth5",
    "auth6",
    "auth7",
    "box",
    "box2",
    "box7",
    "box8",
    "box_easy",
    "box_easy2",
    "box_seal",
    "box_seed",
    "chacha20",
    "codecs",
    "core1",
    "core2",
    "core4",
    "core5",
    "core6",
    "ed25519_convert",
    "generichash",
    "generichash2",
    "generichash3",
    "hash",
    "hash3",
    "kdf",
    "keygen",
    "kx",
    "metamorphic",
    "misuse",
    "onetimeauth",
    "onetimeauth2",
    "onetimeauth7",
    "scalarmult",
    "scalarmult2",
    "scalarmult5",
    "scalarmult6",
    "scalarmult7",
    "secretbox",
    "secretbox2",
    "secretbox_easy",
    "secretbox_easy2",
    "secretstream_xchacha20poly1305",
    "shorthash",
    "sodium_core",
    "sodium_version",
    "stream3",
    "stream4",
    "verify1"
]

shortened_tests = frozenset([
    "auth5",
    "auth7",
    "box7",
    "box8",
    "ed25519_convert",
    "metamorphic",
    "verify1"
])

def maybe_short(test_name):
    if test_name in shortened_tests:
        return ' (short)'
    else:
        return ''

def make_test(test_name, test_machdep):
    return {
        "name":  "%s%s - gcc_x86_64" % (test_name, maybe_short(test_name)),
        "include": "trustinsoft/gcc_x86_64.config",
        "include_": "trustinsoft/common.config",
        "files": [ "test/default/" + test_name + ".c" ]
    }

tis_config = list(
    map(lambda x: make_test(x[0], x[1]), product(tests, machdeps)))

with open('tis.config', 'w') as f:
    print("4. Generate the tis.config file.")
    f.write(string_of_json(tis_config))

# ---------------------------------------------------------------------------- #
# ------------------------------ COPY version.h ------------------------------ #
# ---------------------------------------------------------------------------- #

print("5. Copy generated files.")
for file in files_to_copy:
    with open(file['src'], 'r') as f_src:
        os.makedirs(os.path.dirname(file['dst']), exist_ok=True)
        with open(file['dst'], 'w') as f_dst:
            print("   > Copy '%s' to '%s'." % (file['src'], file['dst']))
            shutil.copyfileobj(f_src, f_dst)
