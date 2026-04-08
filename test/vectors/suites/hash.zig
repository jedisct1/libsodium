const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

const HashFn = *const fn ([*c]u8, [*c]const u8, c_ulonglong) callconv(.c) c_int;

const GenericHashFn = *const fn ([*c]u8, usize, [*c]const u8, c_ulonglong, [*c]const u8, usize) callconv(.c) c_int;

fn runFixedHash(vectors: []const parse.HashVector, hash_fn: HashFn, out_len: usize) Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity == .invalid) {
            result.skipped += 1;
            continue;
        }
        var out_buf: [64]u8 = undefined;
        const out = out_buf[0..out_len];
        const ret = hash_fn(out.ptr, v.msg.ptr, @intCast(v.msg.len));
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: hash returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (!std.mem.eql(u8, out, v.digest)) {
            std.debug.print("FAIL tc={d}: digest mismatch\n", .{v.tc_id});
            result.failed += 1;
        } else {
            result.passed += 1;
        }
    }
    return result;
}

fn runBlake2b(vectors: []const parse.HashVector, hash_fn: GenericHashFn) Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity == .invalid) {
            result.skipped += 1;
            continue;
        }
        const out_len = if (v.out_len > 0) v.out_len else v.digest.len;
        var out_buf: [64]u8 = undefined;
        const out = out_buf[0..out_len];
        const key_ptr: [*c]const u8 = if (v.key.len > 0) v.key.ptr else null;
        const ret = hash_fn(out.ptr, out_len, v.msg.ptr, @intCast(v.msg.len), key_ptr, v.key.len);
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: blake2b returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (!std.mem.eql(u8, out, v.digest)) {
            std.debug.print("FAIL tc={d}: digest mismatch\n", .{v.tc_id});
            result.failed += 1;
        } else {
            result.passed += 1;
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadHashVectors(allocator, data);
    return switch (tag) {
        .hash_sha256 => runFixedHash(vectors, &c.crypto_hash_sha256, c.crypto_hash_sha256_BYTES),
        .hash_sha512 => runFixedHash(vectors, &c.crypto_hash_sha512, c.crypto_hash_sha512_BYTES),
        .hash_sha3_256 => runFixedHash(vectors, &c.crypto_hash_sha3256, c.crypto_hash_sha3256_BYTES),
        .hash_sha3_512 => runFixedHash(vectors, &c.crypto_hash_sha3512, c.crypto_hash_sha3512_BYTES),
        .hash_blake2b => runBlake2b(vectors, &c.crypto_generichash_blake2b),
        else => unreachable,
    };
}
