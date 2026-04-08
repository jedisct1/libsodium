const std = @import("std");
const Io = std.Io;
const Dir = Io.Dir;
const Allocator = std.mem.Allocator;

const options = @import("build_options");
const fetch = @import("fetch.zig");
const parse = @import("parse.zig");
const aead = @import("suites/aead.zig");
const hash = @import("suites/hash.zig");
const mac = @import("suites/mac.zig");
const xdh = @import("suites/xdh.zig");
const eddsa = @import("suites/eddsa.zig");
const secretbox = @import("suites/secretbox.zig");
const box = @import("suites/box.zig");
const stream = @import("suites/stream.zig");
const kdf = @import("suites/kdf.zig");
const pwhash = @import("suites/pwhash.zig");
const xof = @import("suites/xof.zig");
const kem = @import("suites/kem.zig");
const c = @cImport(@cInclude("sodium.h"));

const Result = parse.Result;

fn runSuite(allocator: Allocator, data: []const u8, tag: fetch.SuiteTag) !Result {
    return switch (tag) {
        .aead_chacha20poly1305_ietf,
        .aead_xchacha20poly1305,
        .aead_aes256gcm,
        .aead_aegis128l,
        .aead_aegis256,
        => aead.run(allocator, data, tag),

        .hash_sha256,
        .hash_sha512,
        .hash_blake2b,
        .hash_sha3_256,
        .hash_sha3_512,
        => hash.run(allocator, data, tag),

        .mac_hmacsha256,
        .mac_hmacsha512,
        .mac_siphash24,
        .mac_siphashx24,
        .onetimeauth_poly1305,
        => mac.run(allocator, data, tag),

        .xdh_x25519 => xdh.run(allocator, data),

        .eddsa_ed25519_verify => eddsa.runVerify(allocator, data, false),
        .eddsa_ed25519_cofactored_verify => eddsa.runVerify(allocator, data, true),
        .eddsa_ed25519_sign => eddsa.runSign(allocator, data),

        .secretbox_xsalsa20,
        .secretbox_xchacha20,
        => secretbox.run(allocator, data, tag),

        .box_xsalsa20,
        .box_xchacha20,
        => box.run(allocator, data, tag),

        .stream_salsa20,
        .stream_xsalsa20,
        => stream.run(allocator, data, tag),

        .kdf_hkdf_sha256,
        .kdf_hkdf_sha512,
        => kdf.run(allocator, data, tag),

        .pwhash_scrypt => pwhash.run(allocator, data),

        .xof_shake128,
        .xof_shake256,
        => xof.run(allocator, data, tag),

        .kem_mlkem768 => kem.run(allocator, data),
    };
}

pub fn main(init: std.process.Init) !void {
    var debug_allocator: std.heap.DebugAllocator(.{}) = .init;
    defer if (debug_allocator.deinit() == .leak) @panic("memory leak detected");
    const allocator = debug_allocator.allocator();
    const io = init.io;

    if (c.sodium_init() < 0) return error.SodiumInitFailed;

    const cache_dir = Dir.cwd().createDirPathOpen(io, options.cache_dir, .{}) catch |err| {
        std.debug.print("Cannot open cache directory '{s}': {}\n", .{ options.cache_dir, err });
        return err;
    };

    fetch.ensureCached(allocator, io, cache_dir, &fetch.rooterberg_sources, options.offline) catch |err| {
        std.debug.print("Fetch failed: {}\n", .{err});
        if (!options.offline) return err;
    };

    var total = Result{};
    var suite_count: u32 = 0;
    var suite_fail: u32 = 0;

    for (&fetch.rooterberg_sources) |*src| {
        const data = fetch.loadCached(allocator, io, cache_dir, src.*) catch |err| {
            std.debug.print("Cannot read {s}/{s}: {}\n", .{ src.category, src.filename, err });
            total.skipped += 1;
            continue;
        } orelse {
            if (!options.offline) {
                std.debug.print("SKIP {s}/{s}: not cached\n", .{ src.category, src.filename });
            }
            total.skipped += 1;
            continue;
        };
        defer allocator.free(data);

        var arena = std.heap.ArenaAllocator.init(allocator);
        defer arena.deinit();

        const result = runSuite(arena.allocator(), data, src.suite_tag) catch |err| {
            std.debug.print("ERROR {s}/{s}: {}\n", .{ src.category, src.filename, err });
            suite_fail += 1;
            continue;
        };

        suite_count += 1;
        total.passed += result.passed;
        total.failed += result.failed;
        total.skipped += result.skipped;

        const status: []const u8 = if (result.failed > 0) "FAIL" else "ok";
        std.debug.print("{s}: {s}/{s} -- {d} passed, {d} failed, {d} skipped\n", .{
            status,
            src.category,
            src.filename,
            result.passed,
            result.failed,
            result.skipped,
        });
    }

    std.debug.print("\n{d} suites, {d} passed, {d} failed, {d} skipped\n", .{
        suite_count,
        total.passed,
        total.failed,
        total.skipped,
    });

    if (total.failed > 0 or suite_fail > 0) {
        return error.TestsFailed;
    }
}
