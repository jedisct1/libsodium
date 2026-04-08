const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

pub fn run(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadScryptVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.validity != .valid) {
            result.skipped += 1;
            continue;
        }
        if (v.n > 1 << 20 or v.r > 8 or v.p > 1) {
            result.skipped += 1;
            continue;
        }
        const out = try allocator.alloc(u8, v.dk_len);
        const ret = c.crypto_pwhash_scryptsalsa208sha256_ll(
            v.password.ptr,
            v.password.len,
            v.salt.ptr,
            v.salt.len,
            v.n,
            v.r,
            v.p,
            out.ptr,
            v.dk_len,
        );
        if (ret != 0) {
            std.debug.print("FAIL tc={d}: scrypt returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
            continue;
        }
        if (std.mem.eql(u8, out, v.dk)) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: derived key mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    }
    return result;
}
