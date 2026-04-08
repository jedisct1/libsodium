const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

const EncryptDetachedFn = *const fn (
    [*c]u8,
    [*c]u8,
    [*c]c_ulonglong,
    [*c]const u8,
    c_ulonglong,
    [*c]const u8,
    c_ulonglong,
    [*c]const u8,
    [*c]const u8,
    [*c]const u8,
) callconv(.c) c_int;

const DecryptDetachedFn = *const fn (
    [*c]u8,
    [*c]u8,
    [*c]const u8,
    c_ulonglong,
    [*c]const u8,
    [*c]const u8,
    c_ulonglong,
    [*c]const u8,
    [*c]const u8,
) callconv(.c) c_int;

fn runAead(
    allocator: Allocator,
    vectors: []const parse.AeadVector,
    encrypt_fn: EncryptDetachedFn,
    decrypt_fn: DecryptDetachedFn,
    comptime tag_len: usize,
) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.tag.len != tag_len) {
            result.skipped += 1;
            continue;
        }
        if (v.validity == .valid) {
            const ct_buf = try allocator.alloc(u8, v.msg.len);
            var mac: [tag_len]u8 = undefined;
            _ = encrypt_fn(
                ct_buf.ptr,
                &mac,
                null,
                v.msg.ptr,
                @intCast(v.msg.len),
                v.aad.ptr,
                @intCast(v.aad.len),
                null,
                v.nonce.ptr,
                v.key.ptr,
            );
            if (!std.mem.eql(u8, ct_buf, v.ct) or !std.mem.eql(u8, &mac, v.tag)) {
                std.debug.print("FAIL tc={d}: encrypt mismatch\n", .{v.tc_id});
                result.failed += 1;
                continue;
            }
            const pt_buf = try allocator.alloc(u8, v.ct.len);
            const ret = decrypt_fn(
                pt_buf.ptr,
                null,
                v.ct.ptr,
                @intCast(v.ct.len),
                v.tag.ptr,
                v.aad.ptr,
                @intCast(v.aad.len),
                v.nonce.ptr,
                v.key.ptr,
            );
            if (ret != 0 or !std.mem.eql(u8, pt_buf, v.msg)) {
                std.debug.print("FAIL tc={d}: decrypt mismatch\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        } else {
            const pt_buf = try allocator.alloc(u8, if (v.ct.len > 0) v.ct.len else 1);
            const ret = decrypt_fn(
                pt_buf.ptr,
                null,
                v.ct.ptr,
                @intCast(v.ct.len),
                v.tag.ptr,
                v.aad.ptr,
                @intCast(v.aad.len),
                v.nonce.ptr,
                v.key.ptr,
            );
            if (ret == 0) {
                std.debug.print("FAIL tc={d}: decrypt should have failed\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadAeadVectors(allocator, data);
    return switch (tag) {
        .aead_chacha20poly1305_ietf => try runAead(
            allocator,
            vectors,
            &c.crypto_aead_chacha20poly1305_ietf_encrypt_detached,
            &c.crypto_aead_chacha20poly1305_ietf_decrypt_detached,
            c.crypto_aead_chacha20poly1305_ietf_ABYTES,
        ),
        .aead_xchacha20poly1305 => try runAead(
            allocator,
            vectors,
            &c.crypto_aead_xchacha20poly1305_ietf_encrypt_detached,
            &c.crypto_aead_xchacha20poly1305_ietf_decrypt_detached,
            c.crypto_aead_xchacha20poly1305_ietf_ABYTES,
        ),
        .aead_aes256gcm => {
            if (c.crypto_aead_aes256gcm_is_available() == 0) {
                return Result{ .skipped = @intCast(vectors.len) };
            }
            return try runAead(
                allocator,
                vectors,
                &c.crypto_aead_aes256gcm_encrypt_detached,
                &c.crypto_aead_aes256gcm_decrypt_detached,
                c.crypto_aead_aes256gcm_ABYTES,
            );
        },
        .aead_aegis128l => try runAead(
            allocator,
            vectors,
            &c.crypto_aead_aegis128l_encrypt_detached,
            &c.crypto_aead_aegis128l_decrypt_detached,
            c.crypto_aead_aegis128l_ABYTES,
        ),
        .aead_aegis256 => try runAead(
            allocator,
            vectors,
            &c.crypto_aead_aegis256_encrypt_detached,
            &c.crypto_aead_aegis256_decrypt_detached,
            c.crypto_aead_aegis256_ABYTES,
        ),
        else => unreachable,
    };
}
