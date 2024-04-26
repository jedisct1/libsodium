const std = @import("std");
const fmt = std.fmt;
const fs = std.fs;
const heap = std.heap;
const mem = std.mem;
const Compile = std.Build.Step.Compile;
const Target = std.Target;

fn initLibConfig(target: std.Build.ResolvedTarget, lib: *Compile) void {
    lib.linkLibC();
    lib.addIncludePath(.{ .path = "src/libsodium/include/sodium" });
    lib.defineCMacro("_GNU_SOURCE", "1");
    lib.defineCMacro("CONFIGURED", "1");
    lib.defineCMacro("DEV_MODE", "1");
    lib.defineCMacro("HAVE_ATOMIC_OPS", "1");
    lib.defineCMacro("HAVE_C11_MEMORY_FENCES", "1");
    lib.defineCMacro("HAVE_CET_H", "1");
    lib.defineCMacro("HAVE_GCC_MEMORY_FENCES", "1");
    lib.defineCMacro("HAVE_INLINE_ASM", "1");
    lib.defineCMacro("HAVE_INTTYPES_H", "1");
    lib.defineCMacro("HAVE_STDINT_H", "1");
    lib.defineCMacro("HAVE_TI_MODE", "1");
    lib.want_lto = false;

    const endian = target.result.cpu.arch.endian();
    switch (endian) {
        .big => lib.defineCMacro("NATIVE_BIG_ENDIAN", "1"),
        .little => lib.defineCMacro("NATIVE_LITTLE_ENDIAN", "1"),
    }

    switch (target.result.os.tag) {
        .linux => {
            lib.defineCMacro("ASM_HIDE_SYMBOL", ".hidden");
            lib.defineCMacro("TLS", "_Thread_local");

            lib.defineCMacro("HAVE_CATCHABLE_ABRT", "1");
            lib.defineCMacro("HAVE_CATCHABLE_SEGV", "1");
            lib.defineCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.defineCMacro("HAVE_GETPID", "1");
            lib.defineCMacro("HAVE_INLINE_ASM", "1");
            lib.defineCMacro("HAVE_MADVISE", "1");
            lib.defineCMacro("HAVE_MLOCK", "1");
            lib.defineCMacro("HAVE_MMAP", "1");
            lib.defineCMacro("HAVE_MPROTECT", "1");
            lib.defineCMacro("HAVE_NANOSLEEP", "1");
            lib.defineCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.defineCMacro("HAVE_PTHREAD_PRIO_INHERIT", "1");
            lib.defineCMacro("HAVE_PTHREAD", "1");
            lib.defineCMacro("HAVE_RAISE", "1");
            lib.defineCMacro("HAVE_SYSCONF", "1");
            lib.defineCMacro("HAVE_SYS_AUXV_H", "1");
            lib.defineCMacro("HAVE_SYS_MMAN_H", "1");
            lib.defineCMacro("HAVE_SYS_PARAM_H", "1");
            lib.defineCMacro("HAVE_SYS_RANDOM_H", "1");
            lib.defineCMacro("HAVE_WEAK_SYMBOLS", "1");
        },
        .windows => {
            lib.defineCMacro("HAVE_RAISE", "1");
            lib.defineCMacro("HAVE_SYS_PARAM_H", "1");
            if (lib.isStaticLibrary()) {
                lib.defineCMacro("SODIUM_STATIC", "1");
            }
        },
        .macos => {
            lib.defineCMacro("ASM_HIDE_SYMBOL", ".private_extern");
            lib.defineCMacro("TLS", "_Thread_local");

            lib.defineCMacro("HAVE_ARC4RANDOM", "1");
            lib.defineCMacro("HAVE_ARC4RANDOM_BUF", "1");
            lib.defineCMacro("HAVE_CATCHABLE_ABRT", "1");
            lib.defineCMacro("HAVE_CATCHABLE_SEGV", "1");
            lib.defineCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.defineCMacro("HAVE_GETENTROPY", "1");
            lib.defineCMacro("HAVE_GETPID", "1");
            lib.defineCMacro("HAVE_MADVISE", "1");
            lib.defineCMacro("HAVE_MEMSET_S", "1");
            lib.defineCMacro("HAVE_MLOCK", "1");
            lib.defineCMacro("HAVE_MMAP", "1");
            lib.defineCMacro("HAVE_MPROTECT", "1");
            lib.defineCMacro("HAVE_NANOSLEEP", "1");
            lib.defineCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.defineCMacro("HAVE_PTHREAD", "1");
            lib.defineCMacro("HAVE_PTHREAD_PRIO_INHERIT", "1");
            lib.defineCMacro("HAVE_RAISE", "1");
            lib.defineCMacro("HAVE_SYSCONF", "1");
            lib.defineCMacro("HAVE_SYS_MMAN_H", "1");
            lib.defineCMacro("HAVE_SYS_PARAM_H", "1");
            lib.defineCMacro("HAVE_SYS_RANDOM_H", "1");
            lib.defineCMacro("HAVE_WEAK_SYMBOLS", "1");
        },
        .wasi => {
            lib.defineCMacro("HAVE_ARC4RANDOM", "1");
            lib.defineCMacro("HAVE_ARC4RANDOM_BUF", "1");
            lib.defineCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.defineCMacro("HAVE_GETENTROPY", "1");
            lib.defineCMacro("HAVE_NANOSLEEP", "1");
            lib.defineCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.defineCMacro("HAVE_SYS_AUXV_H", "1");
            lib.defineCMacro("HAVE_SYS_PARAM_H", "1");
            lib.defineCMacro("HAVE_SYS_RANDOM_H", "1");
        },
        else => {},
    }

    switch (target.result.cpu.arch) {
        .x86_64 => {
            lib.defineCMacro("HAVE_AMD64_ASM", "1");
            lib.defineCMacro("HAVE_AVX_ASM", "1");
            lib.defineCMacro("HAVE_CPUID", "1");
            lib.defineCMacro("HAVE_MMINTRIN_H", "1");
            lib.defineCMacro("HAVE_EMMINTRIN_H", "1");
            lib.defineCMacro("HAVE_PMMINTRIN_H", "1");
            lib.defineCMacro("HAVE_TMMINTRIN_H", "1");
            lib.defineCMacro("HAVE_SMMINTRIN_H", "1");
            lib.defineCMacro("HAVE_AVXINTRIN_H", "1");
            lib.defineCMacro("HAVE_AVX2INTRIN_H", "1");
            lib.defineCMacro("HAVE_AVX512FINTRIN_H", "1");
            lib.defineCMacro("HAVE_WMMINTRIN_H", "1");
            lib.defineCMacro("HAVE_RDRAND", "1");
        },
        .aarch64, .aarch64_be => {
            lib.defineCMacro("HAVE_ARMCRYPTO", "1");
        },
        .wasm32, .wasm64 => {
            lib.defineCMacro("__wasm__", "1");
        },
        else => {},
    }

    switch (target.result.os.tag) {
        .wasi => {
            lib.defineCMacro("__wasi__", "1");
        },
        else => {},
    }
}

pub fn build(b: *std.Build) !void {
    const root_path = b.pathFromRoot(".");
    var cwd = try fs.openDirAbsolute(root_path, .{});
    defer cwd.close();

    const src_path = "src/libsodium";
    const src_dir = try fs.Dir.openDir(cwd, src_path, .{ .iterate = true, .no_follow = true });

    var target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const enable_benchmarks = b.option(bool, "enable_benchmarks", "Whether tests should be benchmarks.") orelse false;
    const benchmarks_iterations = b.option(u32, "iterations", "Number of iterations for benchmarks.") orelse 200;
    var build_static = b.option(bool, "static", "Build libsodium as a static library.") orelse true;
    const build_shared = b.option(bool, "shared", "Build libsodium as a shared library.") orelse true;

    const build_tests = b.option(bool, "test", "Build the tests (implies -Dstatic=true)") orelse true;

    if (build_tests) {
        build_static = true;
    }

    switch (target.result.cpu.arch) {
        .aarch64, .aarch64_be, .aarch64_32 => {
            // ARM CPUs supported by Windows are assumed to have NEON support
            if (target.result.isMinGW()) {
                target.query.cpu_features_add.addFeature(@intFromEnum(Target.aarch64.Feature.neon));
            }
        },
        else => {},
    }

    const static_lib = b.addStaticLibrary(.{
        .name = "sodium",
        .target = target,
        .optimize = optimize,
    });
    const shared_lib = b.addSharedLibrary(.{
        .name = if (target.result.isMinGW()) "sodium_shared" else "sodium",
        .target = target,
        .optimize = optimize,
        .strip = optimize != .Debug and !target.result.isMinGW(),
    });

    // work out which libraries we are building
    var libs = std.ArrayList(*Compile).init(b.allocator);
    defer libs.deinit();
    if (build_static) {
        try libs.append(static_lib);
    }
    if (build_shared) {
        try libs.append(shared_lib);
    }

    const prebuilt_version_file_path = "builds/msvc/version.h";
    const version_file_path = "include/sodium/version.h";

    if (src_dir.access(version_file_path, .{ .mode = .read_only })) {} else |_| {
        try cwd.copyFile(prebuilt_version_file_path, src_dir, version_file_path, .{});
    }

    for (libs.items) |lib| {
        if (lib.isDynamicLibrary() and
            !(target.result.isDarwin() or target.result.isBSD() or target.result.isGnu() or target.result.isAndroid() or target.result.isMinGW()))
        {
            continue;
        }
        b.installArtifact(lib);
        lib.installHeader(.{ .path = src_path ++ "/include/sodium.h" }, "sodium.h");
        lib.installHeadersDirectory(.{ .path = src_path ++ "/include/sodium" }, "sodium", .{});

        initLibConfig(target, lib);

        const MFlags = enum {
            sse2,
            ssse3,
            sse41,
            avx,
            avx2,
            avx512f,
            aes,
            pclmul,
            rdrnd,
            crypto,

            fn f(flag: @This()) Target.Cpu.Feature.Set.Index {
                return switch (flag) {
                    .sse2 => @intFromEnum(Target.x86.Feature.sse2),
                    .ssse3 => @intFromEnum(Target.x86.Feature.ssse3),
                    .sse41 => @intFromEnum(Target.x86.Feature.sse4_1),
                    .avx => @intFromEnum(Target.x86.Feature.avx),
                    .avx2 => @intFromEnum(Target.x86.Feature.avx2),
                    .avx512f => @intFromEnum(Target.x86.Feature.avx512f),
                    .aes => @intFromEnum(Target.x86.Feature.aes),
                    .pclmul => @intFromEnum(Target.x86.Feature.pclmul),
                    .rdrnd => @intFromEnum(Target.x86.Feature.rdrnd),
                    .crypto => @intFromEnum(Target.aarch64.Feature.crypto),
                };
            }
        };

        const MLib = struct {
            name: []const u8,
            count: usize,
            sources: []const []const u8,
            flags: []const MFlags,
            arches: []const std.Target.Cpu.Arch,
            lib: *Compile = undefined,
        };

        var mlibs: [8]MLib = .{
            .{
                .name = "armcrypto",
                .count = 3,
                .sources = &.{
                    "crypto_aead/aegis128l/aegis128l_armcrypto.c",
                    "crypto_aead/aegis256/aegis256_armcrypto.c",
                    "crypto_aead/aes256gcm/armcrypto/aead_aes256gcm_armcrypto.c",
                },
                .flags = &.{.aes},
                .arches = &.{ .aarch64, .aarch64_be, .aarch64_32 },
            },

            .{
                .name = "sse2",
                .count = 1,
                .sources = &.{
                    "crypto_stream/salsa20/xmm6int/salsa20_xmm6int-sse2.c",
                },
                .flags = &.{.sse2},
                .arches = &.{ .x86_64, .x86 },
            },
            .{
                .name = "ssse3",
                .count = 3,
                .sources = &.{
                    "crypto_generichash/blake2b/ref/blake2b-compress-ssse3.c",
                    "crypto_pwhash/argon2/argon2-fill-block-ssse3.c",
                    "crypto_stream/chacha20/dolbeau/chacha20_dolbeau-ssse3.c",
                },
                .flags = &.{ .sse2, .ssse3 },
                .arches = &.{ .x86_64, .x86 },
            },
            .{
                .name = "sse41",
                .count = 1,
                .sources = &.{
                    "crypto_generichash/blake2b/ref/blake2b-compress-sse41.c",
                },
                .flags = &.{ .sse2, .ssse3, .sse41 },
                .arches = &.{ .x86_64, .x86 },
            },
            .{
                .name = "avx2",
                .count = 4,
                .sources = &.{
                    "crypto_generichash/blake2b/ref/blake2b-compress-avx2.c",
                    "crypto_pwhash/argon2/argon2-fill-block-avx2.c",
                    "crypto_stream/chacha20/dolbeau/chacha20_dolbeau-avx2.c",
                    "crypto_stream/salsa20/xmm6int/salsa20_xmm6int-avx2.c",
                },
                .flags = &.{ .sse2, .ssse3, .sse41, .avx, .avx2 },
                .arches = &.{.x86_64},
            },
            .{
                .name = "avx512f",
                .count = 1,
                .sources = &.{
                    "crypto_pwhash/argon2/argon2-fill-block-avx512f.c",
                },
                .flags = &.{ .sse2, .ssse3, .sse41, .avx, .avx2, .avx512f },
                .arches = &.{.x86_64},
            },
            .{
                .name = "aesni",
                .count = 3,
                .sources = &.{
                    "crypto_aead/aegis128l/aegis128l_aesni.c",
                    "crypto_aead/aegis256/aegis256_aesni.c",
                    "crypto_aead/aes256gcm/aesni/aead_aes256gcm_aesni.c",
                },
                .flags = &.{ .avx, .aes, .pclmul },
                .arches = &.{.x86_64},
            },
            .{
                .name = "mrdrnd",
                .count = 1,
                .sources = &.{
                    "randombytes/internal/randombytes_internal_random.c",
                },
                .flags = &.{.rdrnd},
                .arches = &.{ .x86_64, .x86 },
            },
        };

        const base_flags = &.{
            "-fvisibility=hidden",
            "-fno-strict-aliasing",
            "-fno-strict-overflow",
            "-fwrapv",
            "-flax-vector-conversions",
        };

        const allocator = heap.page_allocator;

        // compile CPU-specific library code
        for (&mlibs) |*mlib| {
            var target2 = target;
            for (mlib.arches) |arch| {
                if (target.result.cpu.arch == arch) {
                    for (mlib.flags) |flag| {
                        target2.query.cpu_features_add.addFeature(flag.f());
                    }
                    break;
                }
            }

            mlib.lib = b.addStaticLibrary(.{
                .name = mlib.name,
                .target = target2,
                .optimize = optimize,
            });
            const elib = mlib.lib;
            initLibConfig(target, elib);

            var flags = std.ArrayList([]const u8).init(allocator);
            defer flags.deinit();
            try flags.appendSlice(base_flags);

            for (mlib.sources) |path| {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, path });
                elib.addCSourceFiles(.{
                    .files = &.{full_path},
                    .flags = flags.items,
                });
            }
            lib.linkLibrary(elib);
        }

        // compile generic library code
        var walker = try src_dir.walk(allocator);
        files: while (try walker.next()) |entry| {
            var flags = std.ArrayList([]const u8).init(allocator);
            defer flags.deinit();
            try flags.appendSlice(base_flags);

            const name = entry.basename;

            if (mem.endsWith(u8, name, ".c")) {
                for (mlibs) |mlib| {
                    for (mlib.sources) |path| {
                        if (mem.eql(u8, entry.path, path)) continue :files;
                    }
                }

                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });

                lib.addCSourceFiles(.{
                    .files = &.{full_path},
                    .flags = flags.items,
                });
            } else if (mem.endsWith(u8, name, ".S")) {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });
                lib.addAssemblyFile(.{ .path = full_path });
            }
        }
    }

    const test_path = "test/default";
    const out_bin_path = "zig-out/bin";
    const test_dir = try fs.Dir.openDir(cwd, test_path, .{ .iterate = true, .no_follow = true });
    fs.Dir.makePath(cwd, out_bin_path) catch {};
    const out_bin_dir = try fs.Dir.openDir(cwd, out_bin_path, .{});
    try test_dir.copyFile("run.sh", out_bin_dir, "run.sh", .{});
    const allocator = heap.page_allocator;
    var walker = try test_dir.walk(allocator);
    if (build_tests) {
        while (try walker.next()) |entry| {
            const name = entry.basename;
            if (mem.endsWith(u8, name, ".exp")) {
                try test_dir.copyFile(name, out_bin_dir, name, .{});
                continue;
            }
            if (!mem.endsWith(u8, name, ".c")) {
                continue;
            }
            const exe_name = name[0 .. name.len - 2];
            var exe = b.addExecutable(.{
                .name = exe_name,
                .target = target,
                .optimize = optimize,
                .strip = true,
            });
            exe.linkLibC();
            exe.linkLibrary(static_lib);
            exe.addIncludePath(.{ .path = "src/libsodium/include" });
            exe.addIncludePath(.{ .path = "test/quirks" });
            const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ test_path, entry.path });
            exe.addCSourceFiles(.{ .files = &.{full_path} });
            if (enable_benchmarks) {
                exe.defineCMacro("BENCHMARKS", "1");
                var buf: [16]u8 = undefined;
                exe.defineCMacro("ITERATIONS", std.fmt.bufPrintIntToSlice(&buf, benchmarks_iterations, 10, .lower, .{}));
            }

            b.installArtifact(exe);
        }
    }
}
