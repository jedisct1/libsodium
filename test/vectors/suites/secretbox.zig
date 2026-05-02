const std = @import("std");
const Allocator = std.mem.Allocator;
const parse = @import("../parse.zig");
const fetch = @import("../fetch.zig");
const c = @import("c");

const Result = parse.Result;

const DetachedEncFn = *const fn ([*c]u8, [*c]u8, [*c]const u8, c_ulonglong, [*c]const u8, [*c]const u8) callconv(.c) c_int;
const DetachedDecFn = *const fn ([*c]u8, [*c]const u8, [*c]const u8, c_ulonglong, [*c]const u8, [*c]const u8) callconv(.c) c_int;

fn runSecretbox(
    allocator: Allocator,
    vectors: []const parse.AuthEncVector,
    enc_fn: DetachedEncFn,
    dec_fn: DetachedDecFn,
) !Result {
    var result = Result{};
    for (vectors) |v| {
        if (v.validity == .valid) {
            const ct_buf = try allocator.alloc(u8, v.msg.len);
            var mac: [c.crypto_secretbox_MACBYTES]u8 = undefined;
            _ = enc_fn(ct_buf.ptr, &mac, v.msg.ptr, @intCast(v.msg.len), v.nonce.ptr, v.key.ptr);

            if (!std.mem.eql(u8, ct_buf, v.ct) or !std.mem.eql(u8, &mac, v.tag)) {
                std.debug.print("FAIL tc={d}: encrypt mismatch\n", .{v.tc_id});
                result.failed += 1;
                continue;
            }
            const pt_buf = try allocator.alloc(u8, v.ct.len);
            const ret = dec_fn(
                pt_buf.ptr,
                v.ct.ptr,
                v.tag.ptr,
                @intCast(v.ct.len),
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
            const ret = dec_fn(
                pt_buf.ptr,
                v.ct.ptr,
                v.tag.ptr,
                @intCast(v.ct.len),
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
    const vectors = try parse.loadAuthEncVectors(allocator, data);
    return switch (tag) {
        .secretbox_xsalsa20 => runSecretbox(
            allocator,
            vectors,
            &c.crypto_secretbox_detached,
            &c.crypto_secretbox_open_detached,
        ),
        .secretbox_xchacha20 => runSecretbox(
            allocator,
            vectors,
            &c.crypto_secretbox_xchacha20poly1305_detached,
            &c.crypto_secretbox_xchacha20poly1305_open_detached,
        ),
        else => unreachable,
    };
}
