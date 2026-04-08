const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

const XofFn = *const fn ([*c]u8, usize, [*c]const u8, c_ulonglong) callconv(.c) c_int;

fn runXof(allocator: Allocator, vectors: []const parse.XofVector, xof_fn: XofFn) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity == .invalid) {
            result.skipped += 1;
            continue;
        }
        const out = try allocator.alloc(u8, v.out_len);
        const ret = xof_fn(out.ptr, v.out_len, v.msg.ptr, @intCast(v.msg.len));
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: xof returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (std.mem.eql(u8, out, v.output)) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: output mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadXofVectors(allocator, data);
    return switch (tag) {
        .xof_shake128 => runXof(allocator, vectors, &c.crypto_xof_shake128),
        .xof_shake256 => runXof(allocator, vectors, &c.crypto_xof_shake256),
        else => unreachable,
    };
}
