#!/usr/bin/env python3
"""Download Wycheproof test vectors and generate C headers for libsodium.

Usage:
    python3 generate.py [--vectors-dir /path/to/testvectors_v1]

If --vectors-dir is not specified, downloads from GitHub.
"""

import argparse
import io
import json
import os
import sys
import tarfile
import tempfile
import urllib.request
from pathlib import Path

WYCHEPROOF_URL = (
    "https://github.com/C2SP/wycheproof/archive/"
    "refs/heads/master.tar.gz"
)

SCRIPT_DIR = Path(__file__).parent
VECTORS_OUT_DIR = SCRIPT_DIR / "vectors"

AEAD_CONFIGS = [
    {
        "json": "chacha20_poly1305_test.json",
        "header": "chacha20_poly1305_vectors.h",
        "iv_bits": 96,
    },
    {
        "json": "xchacha20_poly1305_test.json",
        "header": "xchacha20_poly1305_vectors.h",
        "iv_bits": 192,
    },
    # AEGIS-128L and AEGIS-256 omitted: libsodium uses 256-bit tags
    # but Wycheproof only provides 128-bit tag vectors.
    {
        "json": "aes_gcm_test.json",
        "header": "aes256gcm_vectors.h",
        "iv_bits": 96,
        "key_bits": 256,
        "tag_bits": 128,
    },
]


def download_vectors(dest_dir):
    """Download and extract Wycheproof vectors from GitHub."""
    print(f"Downloading vectors from {WYCHEPROOF_URL} ...")
    resp = urllib.request.urlopen(WYCHEPROOF_URL, timeout=60)
    data = resp.read()

    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tf:
        try:
            tf.extractall(dest_dir, filter="data")
        except TypeError:
            tf.extractall(dest_dir)

    for entry in Path(dest_dir).iterdir():
        candidate = entry / "testvectors_v1"
        if candidate.is_dir():
            return candidate

    raise RuntimeError("testvectors_v1 not found in archive")


def load_json(vectors_dir, filename):
    path = Path(vectors_dir) / filename
    with open(path) as f:
        return json.load(f)


def header_guard(name):
    return name.upper().replace(".", "_").replace("/", "_")


def generate_aead_header(vectors_dir, cfg):
    """Generate C header for AEAD test vectors."""
    data = load_json(vectors_dir, cfg["json"])
    guard = header_guard(cfg["header"])
    iv_bits = cfg["iv_bits"]
    key_bits = cfg.get("key_bits")
    tag_bits = cfg.get("tag_bits")

    lines = [
        f"/* Auto-generated from Wycheproof {cfg['json']} */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "struct wycheproof_aead_test {",
        "    int         tc_id;",
        "    const char *key;",
        "    const char *iv;",
        "    const char *aad;",
        "    const char *msg;",
        "    const char *ct;",
        "    const char *tag;",
        "    const char *result;",
        "};",
        "",
        "static const struct wycheproof_aead_test wycheproof_tests[] = {",
    ]

    count = 0
    for group in data["testGroups"]:
        if group.get("ivSize") != iv_bits:
            continue
        if key_bits is not None and group.get("keySize") != key_bits:
            continue
        if tag_bits is not None and group.get("tagSize") != tag_bits:
            continue
        for t in group["tests"]:
            lines.append(
                f"    {{ {t['tcId']}, "
                f'"{t["key"]}", "{t["iv"]}", '
                f'"{t["aad"]}", "{t["msg"]}", '
                f'"{t["ct"]}", "{t["tag"]}", '
                f'"{t["result"]}" }},'
            )
            count += 1

    lines.extend([
        "};",
        "",
        "#define WYCHEPROOF_AEAD_COUNT \\",
        "    (sizeof(wycheproof_tests) / sizeof(wycheproof_tests[0]))",
        "",
        f"#endif /* {guard} */",
    ])

    print(f"  {cfg['header']}: {count} vectors")
    return "\n".join(lines) + "\n"


def generate_eddsa_header(vectors_dir):
    """Generate C header for Ed25519 verify test vectors."""
    data = load_json(vectors_dir, "ed25519_test.json")
    guard = "WYCHEPROOF_ED25519_VECTORS_H"

    lines = [
        "/* Auto-generated from Wycheproof ed25519_test.json */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "struct wycheproof_eddsa_test {",
        "    int         tc_id;",
        "    const char *pk;",
        "    const char *sig;",
        "    const char *msg;",
        "    const char *result;",
        "};",
        "",
        "static const struct wycheproof_eddsa_test wycheproof_tests[] = {",
    ]

    count = 0
    for group in data["testGroups"]:
        pk = group["publicKey"]["pk"]
        for t in group["tests"]:
            lines.append(
                f"    {{ {t['tcId']}, "
                f'"{pk}", '
                f'"{t["sig"]}", "{t["msg"]}", '
                f'"{t["result"]}" }},'
            )
            count += 1

    lines.extend([
        "};",
        "",
        "#define WYCHEPROOF_EDDSA_COUNT \\",
        "    (sizeof(wycheproof_tests) / sizeof(wycheproof_tests[0]))",
        "",
        f"#endif /* {guard} */",
    ])

    print(f"  ed25519_vectors.h: {count} vectors")
    return "\n".join(lines) + "\n"


def generate_x25519_header(vectors_dir):
    """Generate C header for X25519 test vectors."""
    data = load_json(vectors_dir, "x25519_test.json")
    guard = "WYCHEPROOF_X25519_VECTORS_H"

    lines = [
        "/* Auto-generated from Wycheproof x25519_test.json */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "struct wycheproof_x25519_test {",
        "    int         tc_id;",
        "    const char *public_key;",
        "    const char *private_key;",
        "    const char *shared;",
        "    const char *result;",
        "};",
        "",
        "static const struct wycheproof_x25519_test wycheproof_tests[] = {",
    ]

    count = 0
    for group in data["testGroups"]:
        for t in group["tests"]:
            lines.append(
                f"    {{ {t['tcId']}, "
                f'"{t["public"]}", "{t["private"]}", '
                f'"{t["shared"]}", '
                f'"{t["result"]}" }},'
            )
            count += 1

    lines.extend([
        "};",
        "",
        "#define WYCHEPROOF_X25519_COUNT \\",
        "    (sizeof(wycheproof_tests) / sizeof(wycheproof_tests[0]))",
        "",
        f"#endif /* {guard} */",
    ])

    print(f"  x25519_vectors.h: {count} vectors")
    return "\n".join(lines) + "\n"


def generate_hkdf_header(vectors_dir, json_file, header_name):
    """Generate C header for HKDF test vectors."""
    data = load_json(vectors_dir, json_file)
    guard = header_guard(header_name)

    lines = [
        f"/* Auto-generated from Wycheproof {json_file} */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "struct wycheproof_hkdf_test {",
        "    int         tc_id;",
        "    const char *ikm;",
        "    const char *salt;",
        "    const char *info;",
        "    int         size;",
        "    const char *okm;",
        "    const char *result;",
        "};",
        "",
        "static const struct wycheproof_hkdf_test wycheproof_tests[] = {",
    ]

    count = 0
    for group in data["testGroups"]:
        for t in group["tests"]:
            lines.append(
                f"    {{ {t['tcId']}, "
                f'"{t["ikm"]}", "{t["salt"]}", '
                f'"{t["info"]}", {t["size"]}, '
                f'"{t["okm"]}", '
                f'"{t["result"]}" }},'
            )
            count += 1

    lines.extend([
        "};",
        "",
        "#define WYCHEPROOF_HKDF_COUNT \\",
        "    (sizeof(wycheproof_tests) / sizeof(wycheproof_tests[0]))",
        "",
        f"#endif /* {guard} */",
    ])

    print(f"  {header_name}: {count} vectors")
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(
        description="Generate C headers from Wycheproof test vectors"
    )
    parser.add_argument(
        "--vectors-dir", type=Path,
        help="Local testvectors_v1 directory (downloads if omitted)",
    )
    args = parser.parse_args()

    if args.vectors_dir:
        vectors_dir = args.vectors_dir
    else:
        tmpdir = Path(tempfile.mkdtemp())
        vectors_dir = download_vectors(tmpdir)

    VECTORS_OUT_DIR.mkdir(parents=True, exist_ok=True)

    print("Generating AEAD headers:")
    for cfg in AEAD_CONFIGS:
        try:
            content = generate_aead_header(vectors_dir, cfg)
            (VECTORS_OUT_DIR / cfg["header"]).write_text(content)
        except FileNotFoundError:
            print(f"  WARNING: {cfg['json']} not found, skipping")

    print("Generating Ed25519 header:")
    content = generate_eddsa_header(vectors_dir)
    (VECTORS_OUT_DIR / "ed25519_vectors.h").write_text(content)

    print("Generating X25519 header:")
    content = generate_x25519_header(vectors_dir)
    (VECTORS_OUT_DIR / "x25519_vectors.h").write_text(content)

    print("Generating HKDF-SHA256 header:")
    content = generate_hkdf_header(
        vectors_dir, "hkdf_sha256_test.json", "hkdf_sha256_vectors.h"
    )
    (VECTORS_OUT_DIR / "hkdf_sha256_vectors.h").write_text(content)

    print("Done.")


if __name__ == "__main__":
    main()
