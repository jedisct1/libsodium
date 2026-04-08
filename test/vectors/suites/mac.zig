const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

const MacFn = *const fn ([*c]u8, [*c]const u8, c_ulonglong, [*c]const u8) callconv(.c) c_int;

fn checkMac(result: *Result, v: parse.MacVector, computed: []const u8) void {
    const matches = std.mem.eql(u8, computed, v.mac);
    if (v.validity == .valid) {
        if (matches) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: mac mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    } else {
        if (!matches) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: invalid mac should not match\n", .{v.tc_id});
            result.failed += 1;
        }
    }
}

fn runHmac256(vectors: []const parse.MacVector) Result {
    var result = Result{};
    for (vectors) |v| {
        var state: c.crypto_auth_hmacsha256_state = undefined;
        _ = c.crypto_auth_hmacsha256_init(&state, v.key.ptr, v.key.len);
        _ = c.crypto_auth_hmacsha256_update(&state, v.msg.ptr, @intCast(v.msg.len));
        var out: [c.crypto_auth_hmacsha256_BYTES]u8 = undefined;
        _ = c.crypto_auth_hmacsha256_final(&state, &out);
        checkMac(&result, v, &out);
    }
    return result;
}

fn runHmac512(vectors: []const parse.MacVector) Result {
    var result = Result{};
    for (vectors) |v| {
        var state: c.crypto_auth_hmacsha512_state = undefined;
        _ = c.crypto_auth_hmacsha512_init(&state, v.key.ptr, v.key.len);
        _ = c.crypto_auth_hmacsha512_update(&state, v.msg.ptr, @intCast(v.msg.len));
        var out: [c.crypto_auth_hmacsha512_BYTES]u8 = undefined;
        _ = c.crypto_auth_hmacsha512_final(&state, &out);
        checkMac(&result, v, &out);
    }
    return result;
}

fn runOneShot(vectors: []const parse.MacVector, mac_fn: MacFn, comptime mac_len: usize) Result {
    var result = Result{};
    for (vectors) |v| {
        var out: [mac_len]u8 = undefined;
        const ret = mac_fn(&out, v.msg.ptr, @intCast(v.msg.len), v.key.ptr);
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: mac returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        checkMac(&result, v, &out);
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadMacVectors(allocator, data);
    return switch (tag) {
        .mac_hmacsha256 => runHmac256(vectors),
        .mac_hmacsha512 => runHmac512(vectors),
        .mac_siphash24 => runOneShot(vectors, &c.crypto_shorthash_siphash24, c.crypto_shorthash_siphash24_BYTES),
        .mac_siphashx24 => runOneShot(vectors, &c.crypto_shorthash_siphashx24, c.crypto_shorthash_siphashx24_BYTES),
        .onetimeauth_poly1305 => runOneShot(vectors, &c.crypto_onetimeauth_poly1305, c.crypto_onetimeauth_poly1305_BYTES),
        else => unreachable,
    };
}
