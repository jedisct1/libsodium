const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const c = @import("c");

const Result = parse.Result;

pub fn run(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadXdhVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.private_key.len != c.crypto_scalarmult_curve25519_SCALARBYTES or
            v.public_key.len != c.crypto_scalarmult_curve25519_BYTES)
        {
            result.skipped += 1;
            continue;
        }
        var shared: [c.crypto_scalarmult_curve25519_BYTES]u8 = undefined;
        const ret = c.crypto_scalarmult_curve25519(&shared, v.private_key.ptr, v.public_key.ptr);

        if (v.validity == .valid) {
            if (ret != 0) {
                std.debug.print("FAIL tc={d}: scalarmult returned {d}\n", .{ v.tc_id, ret });
                result.failed += 1;
            } else if (!std.mem.eql(u8, &shared, v.shared)) {
                std.debug.print("FAIL tc={d}: shared secret mismatch\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        } else {
            if (ret != 0) {
                result.passed += 1;
            } else if (std.mem.eql(u8, &shared, v.shared)) {
                std.debug.print("FAIL tc={d}: invalid vector produced matching output\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}
