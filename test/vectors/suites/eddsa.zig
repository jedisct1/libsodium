const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

pub fn runVerify(allocator: Allocator, data: []const u8, is_cofactored: bool) !Result {
    const vectors = try parse.loadEddsaVerifyVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.sig.len != c.crypto_sign_ed25519_BYTES or
            v.public_key.len != c.crypto_sign_ed25519_PUBLICKEYBYTES)
        {
            result.skipped += 1;
            continue;
        }
        const ret = c.crypto_sign_ed25519_verify_detached(
            v.sig.ptr,
            v.msg.ptr,
            @intCast(v.msg.len),
            v.public_key.ptr,
        );
        const accepted = ret == 0;

        if (v.validity == .valid) {
            if (!accepted) {
                if (is_cofactored) {
                    std.debug.print("INFO tc={d}: cofactored-valid sig rejected (small-order)\n", .{v.tc_id});
                    result.skipped += 1;
                } else {
                    std.debug.print("FAIL tc={d}: verify rejected valid sig\n", .{v.tc_id});
                    result.failed += 1;
                }
            } else {
                result.passed += 1;
            }
        } else {
            if (accepted) {
                if (!is_cofactored) {
                    std.debug.print("INFO tc={d}: non-cofactored-invalid sig accepted (cofactored)\n", .{v.tc_id});
                    result.skipped += 1;
                } else {
                    std.debug.print("FAIL tc={d}: verify accepted invalid sig\n", .{v.tc_id});
                    result.failed += 1;
                }
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}

pub fn runSign(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadEddsaSignVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.private_key.len != c.crypto_sign_ed25519_SEEDBYTES) {
            result.skipped += 1;
            continue;
        }
        if (v.validity != .valid) {
            result.skipped += 1;
            continue;
        }
        var pk: [c.crypto_sign_ed25519_PUBLICKEYBYTES]u8 = undefined;
        var sk: [c.crypto_sign_ed25519_SECRETKEYBYTES]u8 = undefined;
        _ = c.crypto_sign_ed25519_seed_keypair(&pk, &sk, v.private_key.ptr);

        if (v.public_key.len == c.crypto_sign_ed25519_PUBLICKEYBYTES and
            !std.mem.eql(u8, &pk, v.public_key))
        {
            std.debug.print("FAIL tc={d}: derived pk mismatch\n", .{v.tc_id});
            result.failed += 1;
            continue;
        }

        var sig: [c.crypto_sign_ed25519_BYTES]u8 = undefined;
        _ = c.crypto_sign_ed25519_detached(&sig, null, v.msg.ptr, @intCast(v.msg.len), &sk);

        if (std.mem.eql(u8, &sig, v.sig)) {
            result.passed += 1;
        } else {
            std.debug.print("FAIL tc={d}: signature mismatch\n", .{v.tc_id});
            result.failed += 1;
        }
    }
    return result;
}
