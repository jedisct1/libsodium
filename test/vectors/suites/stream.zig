const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @import("c");

const Result = parse.Result;

const XorFn = *const fn ([*c]u8, [*c]const u8, c_ulonglong, [*c]const u8, [*c]const u8) callconv(.c) c_int;

fn runStream(allocator: Allocator, vectors: []const parse.StreamVector, xor_fn: XorFn) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity != .valid) {
            result.skipped += 1;
            continue;
        }
        const out = try allocator.alloc(u8, v.msg.len);
        const ret = xor_fn(out.ptr, v.msg.ptr, @intCast(v.msg.len), v.nonce.ptr, v.key.ptr);
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: stream_xor returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (std.mem.eql(u8, out, v.ct)) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: ciphertext mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadStreamVectors(allocator, data);
    return switch (tag) {
        .stream_salsa20 => runStream(allocator, vectors, &c.crypto_stream_salsa20_xor),
        .stream_xsalsa20 => runStream(allocator, vectors, &c.crypto_stream_xsalsa20_xor),
        else => unreachable,
    };
}
