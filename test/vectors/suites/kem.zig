const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const c = @import("c");

const Result = parse.Result;

pub fn run(allocator: Allocator, data: []const u8) !Result {
    const vectors = try parse.loadMlkemVectors(allocator, data);
    var result = Result{};
    for (vectors) |v| {
        if (v.seed.len != c.crypto_kem_mlkem768_SEEDBYTES or
            v.ct.len != c.crypto_kem_mlkem768_CIPHERTEXTBYTES)
        {
            if (v.validity == .invalid) {
                result.passed += 1;
            } else {
                result.skipped += 1;
            }
            continue;
        }

        var pk: [c.crypto_kem_mlkem768_PUBLICKEYBYTES]u8 = undefined;
        var sk: [c.crypto_kem_mlkem768_SECRETKEYBYTES]u8 = undefined;
        _ = c.crypto_kem_mlkem768_seed_keypair(&pk, &sk, v.seed.ptr);

        if (v.ek.len == c.crypto_kem_mlkem768_PUBLICKEYBYTES and
            !std.mem.eql(u8, &pk, v.ek))
        {
            std.debug.print("FAIL tc={d}: derived pk mismatch\n", .{v.tc_id});
            result.failed += 1;
            continue;
        }

        var ss: [c.crypto_kem_mlkem768_SHAREDSECRETBYTES]u8 = undefined;
        const ret = c.crypto_kem_mlkem768_dec(&ss, v.ct.ptr, &sk);

        if (v.validity == .valid) {
            if (ret != 0) {
                std.debug.print("FAIL tc={d}: dec failed\n", .{v.tc_id});
                result.failed += 1;
            } else if (!std.mem.eql(u8, &ss, v.ss)) {
                std.debug.print("FAIL tc={d}: shared secret mismatch\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        } else if (v.validity == .acceptable) {
            result.passed += 1;
        } else {
            // ML-KEM implicit rejection: dec always returns 0 but produces
            // a pseudorandom shared secret that won't match the expected one.
            if (ret == 0 and std.mem.eql(u8, &ss, v.ss)) {
                std.debug.print("FAIL tc={d}: invalid vector produced matching ss\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}
