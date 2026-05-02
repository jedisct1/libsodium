const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @import("c");

const Result = parse.Result;

fn runHkdf(
    allocator: Allocator,
    vectors: []const parse.HkdfVector,
    comptime prk_len: usize,
    extract_fn: *const fn ([*c]u8, [*c]const u8, usize, [*c]const u8, usize) callconv(.c) c_int,
    expand_fn: *const fn ([*c]u8, usize, [*c]const u8, usize, [*c]const u8) callconv(.c) c_int,
) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity != .valid) {
            result.skipped += 1;
            continue;
        }
        var prk: [prk_len]u8 = undefined;
        const salt_ptr: [*c]const u8 = if (v.salt.len > 0) v.salt.ptr else null;
        _ = extract_fn(&prk, salt_ptr, v.salt.len, v.ikm.ptr, v.ikm.len);

        const out = try allocator.alloc(u8, v.out_len);
        const info_ptr: [*c]const u8 = if (v.info.len > 0) v.info.ptr else null;
        const ret = expand_fn(out.ptr, v.out_len, info_ptr, v.info.len, &prk);
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: expand returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (std.mem.eql(u8, out, v.okm)) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: okm mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadHkdfVectors(allocator, data);
    return switch (tag) {
        .kdf_hkdf_sha256 => runHkdf(
            allocator,
            vectors,
            c.crypto_kdf_hkdf_sha256_KEYBYTES,
            &c.crypto_kdf_hkdf_sha256_extract,
            @ptrCast(&c.crypto_kdf_hkdf_sha256_expand),
        ),
        .kdf_hkdf_sha512 => runHkdf(
            allocator,
            vectors,
            c.crypto_kdf_hkdf_sha512_KEYBYTES,
            &c.crypto_kdf_hkdf_sha512_extract,
            @ptrCast(&c.crypto_kdf_hkdf_sha512_expand),
        ),
        else => unreachable,
    };
}
