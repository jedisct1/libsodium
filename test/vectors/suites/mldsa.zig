const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const c = @import("c");

const Result = parse.Result;

fn adapterReject(result: *Result, v_validity: parse.Validity, tc_id: u32) void {
    if (v_validity == .valid) {
        std.debug.print("FAIL tc={d}: valid vector with a length the API cannot take\n", .{tc_id});
        result.failed += 1;
    } else {
        result.skipped += 1;
    }
}

pub fn runVerify(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadMldsaVerifyVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.pk.len != c.crypto_sign_mldsa44_PUBLICKEYBYTES or
            v.sig.len != c.crypto_sign_mldsa44_BYTES)
        {
            adapterReject(&result, v.validity, v.tc_id);
            continue;
        }
        const ret = c.crypto_sign_mldsa44_verify_detached_ctx(
            v.sig.ptr,
            v.msg.ptr,
            @intCast(v.msg.len),
            v.ctx.ptr,
            v.ctx.len,
            v.pk.ptr,
        );
        if ((ret == 0) != (v.validity == .valid)) {
            std.debug.print("FAIL tc={d}: verify returned {d}\n", .{ v.tc_id, ret });
            result.failed += 1;
        } else {
            result.passed += 1;
        }
    }
    return result;
}

pub fn runSign(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadMldsaSignVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        var sk: [c.crypto_sign_mldsa44_SECRETKEYBYTES]u8 = undefined;
        if (v.pk.len != c.crypto_sign_mldsa44_PUBLICKEYBYTES or
            (v.sk.len == 0 and v.seed.len != c.crypto_sign_mldsa44_SEEDBYTES) or
            (v.sk.len != 0 and v.sk.len != sk.len))
        {
            adapterReject(&result, v.validity, v.tc_id);
            continue;
        }
        if (v.sk.len == 0) {
            var pk: [c.crypto_sign_mldsa44_PUBLICKEYBYTES]u8 = undefined;
            _ = c.crypto_sign_mldsa44_seed_keypair(&pk, &sk, v.seed.ptr);
            if (!std.mem.eql(u8, &pk, v.pk)) {
                std.debug.print("FAIL tc={d}: derived pk mismatch\n", .{v.tc_id});
                result.failed += 1;
                continue;
            }
        } else {
            @memcpy(&sk, v.sk);
        }
        if (v.internal) {
            result.skipped += 1;
            continue;
        }
        var rnd: [c.crypto_sign_mldsa44_RANDOMBYTES]u8 = @splat(0);
        if (v.rnd.len != 0) {
            if (v.rnd.len != rnd.len) {
                std.debug.print("FAIL tc={d}: unexpected rnd length\n", .{v.tc_id});
                result.failed += 1;
                continue;
            }
            @memcpy(&rnd, v.rnd);
        }
        var sig: [c.crypto_sign_mldsa44_BYTES]u8 = @splat(0xa5);
        var siglen: c_ulonglong = 1;
        const ret = c.crypto_sign_mldsa44_detached_deterministic(
            &sig,
            &siglen,
            v.msg.ptr,
            @intCast(v.msg.len),
            v.ctx.ptr,
            v.ctx.len,
            &sk,
            &rnd,
        );
        if (v.validity == .valid) {
            if (ret != 0 or siglen != sig.len) {
                std.debug.print("FAIL tc={d}: signing failed\n", .{v.tc_id});
                result.failed += 1;
            } else if (!std.mem.eql(u8, &sig, v.sig)) {
                std.debug.print("FAIL tc={d}: signature mismatch\n", .{v.tc_id});
                result.failed += 1;
            } else if (c.crypto_sign_mldsa44_verify_detached_ctx(&sig, v.msg.ptr, @intCast(v.msg.len), v.ctx.ptr, v.ctx.len, v.pk.ptr) != 0) {
                std.debug.print("FAIL tc={d}: signature does not verify\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        } else {
            const cleared = std.mem.allEqual(u8, &sig, 0) and siglen == 0;
            if (ret == 0 or !cleared) {
                std.debug.print("FAIL tc={d}: invalid vector was not rejected cleanly\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}
