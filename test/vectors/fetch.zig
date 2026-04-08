const std = @import("std");
const Allocator = std.mem.Allocator;
const Io = std.Io;
const Dir = Io.Dir;

pub const Source = struct {
    base_url: []const u8,
    category: []const u8,
    filename: []const u8,
    suite_tag: SuiteTag,
};

pub const SuiteTag = enum {
    aead_chacha20poly1305_ietf,
    aead_xchacha20poly1305,
    aead_aes256gcm,
    aead_aegis128l,
    aead_aegis256,
    hash_sha256,
    hash_sha512,
    hash_blake2b,
    hash_sha3_256,
    hash_sha3_512,
    mac_hmacsha256,
    mac_hmacsha512,
    mac_siphash24,
    mac_siphashx24,
    onetimeauth_poly1305,
    xdh_x25519,
    eddsa_ed25519_verify,
    eddsa_ed25519_cofactored_verify,
    eddsa_ed25519_sign,
    secretbox_xsalsa20,
    secretbox_xchacha20,
    box_xsalsa20,
    box_xchacha20,
    stream_salsa20,
    stream_xsalsa20,
    kdf_hkdf_sha256,
    kdf_hkdf_sha512,
    pwhash_scrypt,
    xof_shake128,
    xof_shake256,
    kem_mlkem768,
};

const rooterberg_base = "https://raw.githubusercontent.com/bleichenbacher-daniel/Rooterberg/master/test_vectors";
const wycheproof_base = "https://raw.githubusercontent.com/C2SP/wycheproof/master/testvectors_v1";

pub const rooterberg_sources = [_]Source{
    .{ .base_url = rooterberg_base, .category = "aead", .filename = "chacha20_poly1305.json", .suite_tag = .aead_chacha20poly1305_ietf },
    .{ .base_url = rooterberg_base, .category = "aead", .filename = "xchacha20_poly1305.json", .suite_tag = .aead_xchacha20poly1305 },
    .{ .base_url = rooterberg_base, .category = "aead", .filename = "aes_gcm_256_96_128.json", .suite_tag = .aead_aes256gcm },
    .{ .base_url = rooterberg_base, .category = "aead", .filename = "aegis128_l_256.json", .suite_tag = .aead_aegis128l },
    .{ .base_url = rooterberg_base, .category = "aead", .filename = "aegis256_256.json", .suite_tag = .aead_aegis256 },
    .{ .base_url = rooterberg_base, .category = "message_digest", .filename = "sha_256.json", .suite_tag = .hash_sha256 },
    .{ .base_url = rooterberg_base, .category = "message_digest", .filename = "sha_512.json", .suite_tag = .hash_sha512 },
    .{ .base_url = rooterberg_base, .category = "message_digest", .filename = "blake2b.json", .suite_tag = .hash_blake2b },
    .{ .base_url = rooterberg_base, .category = "message_digest", .filename = "sha3_256.json", .suite_tag = .hash_sha3_256 },
    .{ .base_url = rooterberg_base, .category = "message_digest", .filename = "sha3_512.json", .suite_tag = .hash_sha3_512 },
    .{ .base_url = rooterberg_base, .category = "mac", .filename = "hmac_sha256_256.json", .suite_tag = .mac_hmacsha256 },
    .{ .base_url = rooterberg_base, .category = "mac", .filename = "hmac_sha512_512.json", .suite_tag = .mac_hmacsha512 },
    .{ .base_url = rooterberg_base, .category = "mac", .filename = "sip_hash_2_4.json", .suite_tag = .mac_siphash24 },
    .{ .base_url = rooterberg_base, .category = "mac", .filename = "sip_hash128_2_4.json", .suite_tag = .mac_siphashx24 },
    .{ .base_url = rooterberg_base, .category = "one_time_mac", .filename = "poly1305.json", .suite_tag = .onetimeauth_poly1305 },
    .{ .base_url = rooterberg_base, .category = "xdh", .filename = "x25519.json", .suite_tag = .xdh_x25519 },
    .{ .base_url = rooterberg_base, .category = "eddsa", .filename = "ed25519.json", .suite_tag = .eddsa_ed25519_verify },
    .{ .base_url = rooterberg_base, .category = "eddsa", .filename = "ed25519_cofactored.json", .suite_tag = .eddsa_ed25519_cofactored_verify },
    .{ .base_url = rooterberg_base, .category = "eddsa", .filename = "ed25519_sign.json", .suite_tag = .eddsa_ed25519_sign },
    .{ .base_url = rooterberg_base, .category = "auth_enc", .filename = "nacl_xsalsa20_poly1305.json", .suite_tag = .secretbox_xsalsa20 },
    .{ .base_url = rooterberg_base, .category = "auth_enc", .filename = "nacl_xchacha20_poly1305.json", .suite_tag = .secretbox_xchacha20 },
    .{ .base_url = rooterberg_base, .category = "nacl_crypto_box", .filename = "nacl_crypto_box_curve25519_xsalsa20_poly1305.json", .suite_tag = .box_xsalsa20 },
    .{ .base_url = rooterberg_base, .category = "nacl_crypto_box", .filename = "nacl_crypto_box_curve25519_xchacha20_poly1305.json", .suite_tag = .box_xchacha20 },
    .{ .base_url = rooterberg_base, .category = "ind_cpa", .filename = "salsa20_256_64.json", .suite_tag = .stream_salsa20 },
    .{ .base_url = rooterberg_base, .category = "ind_cpa", .filename = "xsalsa20_256_192.json", .suite_tag = .stream_xsalsa20 },
    .{ .base_url = rooterberg_base, .category = "kdf", .filename = "hkdf_sha256.json", .suite_tag = .kdf_hkdf_sha256 },
    .{ .base_url = rooterberg_base, .category = "kdf", .filename = "hkdf_sha512.json", .suite_tag = .kdf_hkdf_sha512 },
    .{ .base_url = rooterberg_base, .category = "kdf", .filename = "scrypt.json", .suite_tag = .pwhash_scrypt },
    .{ .base_url = rooterberg_base, .category = "xof", .filename = "shake128.json", .suite_tag = .xof_shake128 },
    .{ .base_url = rooterberg_base, .category = "xof", .filename = "shake256.json", .suite_tag = .xof_shake256 },
    .{ .base_url = wycheproof_base, .category = "", .filename = "mlkem_768_test.json", .suite_tag = .kem_mlkem768 },
};

pub fn cacheName(buf: []u8, category: []const u8, filename: []const u8) []const u8 {
    if (category.len == 0)
        return std.fmt.bufPrint(buf, "{s}", .{filename}) catch buf[0..0];
    return std.fmt.bufPrint(buf, "{s}_{s}", .{ category, filename }) catch buf[0..0];
}

pub fn ensureCached(
    allocator: Allocator,
    io: Io,
    cache_dir: Dir,
    sources: []const Source,
    offline: bool,
) !void {
    if (offline) return;

    var http_arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer http_arena.deinit();
    var client: std.http.Client = .{ .allocator = http_arena.allocator(), .io = io };
    defer client.deinit();

    for (sources) |src| {
        var name_buf: [256]u8 = undefined;
        const name = cacheName(&name_buf, src.category, src.filename);
        if (cache_dir.openFile(io, name, .{})) |file| {
            file.close(io);
        } else |err| {
            if (err != error.FileNotFound) return err;
            try download(allocator, &client, io, cache_dir, src, name);
        }
    }
}

fn download(
    allocator: Allocator,
    client: *std.http.Client,
    io: Io,
    cache_dir: Dir,
    src: Source,
    dest_name: []const u8,
) !void {
    var url_buf: [512]u8 = undefined;
    const url = (if (src.category.len == 0)
        std.fmt.bufPrint(&url_buf, "{s}/{s}", .{ src.base_url, src.filename })
    else
        std.fmt.bufPrint(&url_buf, "{s}/{s}/{s}", .{ src.base_url, src.category, src.filename })) catch return error.UrlTooLong;

    var body = std.Io.Writer.Allocating.init(allocator);
    defer body.deinit();

    const result = try client.fetch(.{
        .location = .{ .url = url },
        .response_writer = &body.writer,
    });

    if (result.status != .ok) {
        std.debug.print("HTTP {d} fetching {s}\n", .{ @intFromEnum(result.status), url });
        return error.HttpFetchFailed;
    }

    try cache_dir.writeFile(io, .{ .sub_path = dest_name, .data = body.written() });
}

pub fn loadCached(allocator: Allocator, io: Io, cache_dir: Dir, src: Source) !?[]const u8 {
    var name_buf: [256]u8 = undefined;
    const name = cacheName(&name_buf, src.category, src.filename);
    return cache_dir.readFileAlloc(io, name, allocator, .unlimited) catch |err| {
        if (err == error.FileNotFound) return null;
        return err;
    };
}
