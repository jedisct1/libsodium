const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

const OpenEasyFn = *const fn ([*c]u8, [*c]const u8, c_ulonglong, [*c]const u8, [*c]const u8, [*c]const u8) callconv(.c) c_int;

fn runBox(
    allocator: Allocator,
    vectors: []const parse.BoxVector,
    dec_fn: OpenEasyFn,
    comptime mac_bytes: usize,
    comptime nonce_bytes: usize,
) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.nonce.len != nonce_bytes) {
            result.skipped += 1;
            continue;
        }
        if (v.ct.len < nonce_bytes + mac_bytes) {
            if (v.validity != .valid) {
                result.passed += 1;
            } else {
                result.failed += 1;
            }
            continue;
        }
        const box_ct = v.ct[nonce_bytes..];
        const ct_len = box_ct.len;

        if (v.validity == .valid) {
            const pt_buf = try allocator.alloc(u8, if (ct_len > mac_bytes) ct_len - mac_bytes else 0);
            const ret = dec_fn(
                pt_buf.ptr,
                box_ct.ptr,
                @intCast(ct_len),
                v.nonce.ptr,
                v.public_key.ptr,
                v.private_key.ptr,
            );
            if (ret != 0) {
                if (v.shared_zero) {
                    std.debug.print("INFO tc={d}: low-order public key rejected\n", .{v.tc_id});
                    result.skipped += 1;
                    continue;
                }
                std.debug.print("FAIL tc={d}: box_open failed\n", .{v.tc_id});
                result.failed += 1;
            } else if (!std.mem.eql(u8, pt_buf, v.msg)) {
                std.debug.print("FAIL tc={d}: plaintext mismatch\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        } else {
            const pt_buf = try allocator.alloc(u8, if (ct_len > mac_bytes) ct_len - mac_bytes else 1);
            const ret = dec_fn(
                pt_buf.ptr,
                box_ct.ptr,
                @intCast(ct_len),
                v.nonce.ptr,
                v.public_key.ptr,
                v.private_key.ptr,
            );
            if (ret == 0) {
                std.debug.print("FAIL tc={d}: box_open should have failed\n", .{v.tc_id});
                result.failed += 1;
            } else {
                result.passed += 1;
            }
        }
    }
    return result;
}

pub fn run(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    const vectors = try parse.loadBoxVectors(allocator, data);
    return switch (tag) {
        .box_xsalsa20 => runBox(
            allocator,
            vectors,
            &c.crypto_box_open_easy,
            c.crypto_box_MACBYTES,
            c.crypto_box_NONCEBYTES,
        ),
        .box_xchacha20 => runBox(
            allocator,
            vectors,
            &c.crypto_box_curve25519xchacha20poly1305_open_easy,
            c.crypto_box_curve25519xchacha20poly1305_MACBYTES,
            c.crypto_box_curve25519xchacha20poly1305_NONCEBYTES,
        ),
        else => unreachable,
    };
}
