const std = @import("std");
const fmt = std.fmt;
const fs = std.fs;
const heap = std.heap;
const mem = std.mem;
const LibExeObjStep = std.build.LibExeObjStep;
const Target = std.Target;

pub fn build(b: *std.build.Builder) !void {
    const root_path = b.pathFromRoot(".");
    var cwd = try fs.openDirAbsolute(root_path, .{});
    defer cwd.close();

    const src_path = "src/libsodium";
    const src_dir = try fs.Dir.openIterableDir(cwd, src_path, .{ .no_follow = true });

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

    switch (target.getCpuArch()) {
        // Features we assume are always available because they won't affect
        // code generation in files that don't use them.
        .x86_64 => {
            target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.aes));
            target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.pclmul));
            target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.rdrnd));
        },
        .aarch64, .aarch64_be => {
            target.cpu_features_add.addFeature(@intFromEnum(Target.aarch64.Feature.crypto));
            // ARM CPUs supported by Windows also support NEON.
            if (target.isWindows()) {
                target.cpu_features_add.addFeature(@intFromEnum(Target.aarch64.Feature.neon));
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
        .name = if (target.isWindows()) "sodium_shared" else "sodium",
        .target = target,
        .optimize = optimize,
    });

    // work out which libraries we are building
    var libs = std.ArrayList(*LibExeObjStep).init(b.allocator);
    defer libs.deinit();
    if (build_static) {
        try libs.append(static_lib);
    }
    if (build_shared) {
        try libs.append(shared_lib);
    }

    const prebuilt_version_file_path = "builds/msvc/version.h";
    const version_file_path = "include/sodium/version.h";

    if (src_dir.dir.access(version_file_path, .{ .mode = .read_only })) {} else |_| {
        try cwd.copyFile(prebuilt_version_file_path, src_dir.dir, version_file_path, .{});
    }

    for (libs.items) |lib| {
        if (lib.isDynamicLibrary() and
            !(target.isDarwin() or target.isDragonFlyBSD() or target.isFreeBSD() or
            target.isLinux() or target.isNetBSD() or target.isOpenBSD() or target.isWindows()))
        {
            continue;
        }
        if (optimize != .Debug and !target.isWindows() and !lib.isStaticLibrary()) {
            lib.strip = true;
        }
        b.installArtifact(lib);
        lib.installHeader(src_path ++ "/include/sodium.h", "sodium.h");
        lib.installHeadersDirectory(src_path ++ "/include/sodium", "sodium");
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

        if (target.cpu_arch) |arch| {
            switch (arch.endian()) {
                .Big => lib.defineCMacro("NATIVE_BIG_ENDIAN", "1"),
                .Little => lib.defineCMacro("NATIVE_LITTLE_ENDIAN", "1"),
            }
        }

        switch (target.getOsTag()) {
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

        switch (target.getCpuArch()) {
            .x86_64 => {
                lib.defineCMacro("HAVE_AMD64_ASM", "1");
                lib.defineCMacro("HAVE_AVX_ASM", "1");
                lib.defineCMacro("HAVE_CPUID", "1");
                lib.defineCMacro("HAVE_MMINTRIN_H", "1");
                lib.defineCMacro("HAVE_EMMINTRIN_H", "1");

                const cpu_features = target.getCpuFeatures();
                const has_sse3 = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.sse3));
                const has_ssse3 = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.ssse3));
                const has_sse4_1 = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.sse4_1));
                const has_avx = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.avx));
                const has_avx2 = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.avx2));
                const has_avx512f = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.avx512f));
                const has_aes = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.aes));
                const has_pclmul = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.pclmul));
                const has_rdrnd = cpu_features.isEnabled(@intFromEnum(Target.x86.Feature.rdrnd));

                if (has_sse3) lib.defineCMacro("HAVE_PMMINTRIN_H", "1");
                if (has_ssse3) lib.defineCMacro("HAVE_TMMINTRIN_H", "1");
                if (has_sse4_1) lib.defineCMacro("HAVE_SMMINTRIN_H", "1");
                if (has_avx) lib.defineCMacro("HAVE_AVXINTRIN_H", "1");
                if (has_avx2) lib.defineCMacro("HAVE_AVX2INTRIN_H", "1");
                if (has_avx512f) lib.defineCMacro("HAVE_AVX512FINTRIN_H", "1");
                if (has_aes and has_pclmul) lib.defineCMacro("HAVE_WMMINTRIN_H", "1");
                if (has_rdrnd) lib.defineCMacro("HAVE_RDRAND", "1");
            },
            .aarch64, .aarch64_be => {
                const cpu_features = target.getCpuFeatures();
                const has_neon = cpu_features.isEnabled(@intFromEnum(Target.aarch64.Feature.neon));
                const has_crypto = cpu_features.isEnabled(@intFromEnum(Target.aarch64.Feature.crypto));
                if (has_neon and has_crypto) {
                    lib.defineCMacro("HAVE_ARMCRYPTO", "1");
                }
            },
            .wasm32, .wasm64 => {
                lib.defineCMacro("__wasm__", "1");
            },
            else => {},
        }

        switch (target.getOsTag()) {
            .wasi => {
                lib.defineCMacro("__wasi__", "1");
            },
            else => {},
        }

        switch (target.getCpuArch()) {
            .x86_64 => {
                lib.target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.sse4_1));
                lib.target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.aes));
                lib.target.cpu_features_add.addFeature(@intFromEnum(Target.x86.Feature.pclmul));
            },
            else => {},
        }

        var allocator = heap.page_allocator;
        var walker = try src_dir.walk(allocator);
        while (try walker.next()) |entry| {
            const name = entry.basename;
            if (mem.endsWith(u8, name, ".c")) {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });
                lib.addCSourceFiles(&.{full_path}, &.{
                    "-fvisibility=hidden",
                    "-fno-strict-aliasing",
                    "-fno-strict-overflow",
                    "-fwrapv",
                    "-flax-vector-conversions",
                });
            } else if (mem.endsWith(u8, name, ".S")) {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });
                lib.addAssemblyFile(.{ .path = full_path });
            }
        }
    }

    const test_path = "test/default";
    const out_bin_path = "zig-out/bin";
    const test_dir = try fs.Dir.openIterableDir(cwd, test_path, .{ .no_follow = true });
    fs.Dir.makePath(cwd, out_bin_path) catch {};
    const out_bin_dir = try fs.Dir.openDir(cwd, out_bin_path, .{});
    try test_dir.dir.copyFile("run.sh", out_bin_dir, "run.sh", .{});
    var allocator = heap.page_allocator;
    var walker = try test_dir.walk(allocator);
    if (build_tests) {
        while (try walker.next()) |entry| {
            const name = entry.basename;
            if (mem.endsWith(u8, name, ".exp")) {
                try test_dir.dir.copyFile(name, out_bin_dir, name, .{});
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
            });
            exe.linkLibC();
            exe.strip = true;
            exe.linkLibrary(static_lib);
            exe.addIncludePath(.{ .path = "src/libsodium/include" });
            exe.addIncludePath(.{ .path = "test/quirks" });
            const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ test_path, entry.path });
            exe.addCSourceFiles(&.{full_path}, &.{});

            if (enable_benchmarks) {
                exe.defineCMacro("BENCHMARKS", "1");
                var buf: [16]u8 = undefined;
                exe.defineCMacro("ITERATIONS", std.fmt.bufPrintIntToSlice(&buf, benchmarks_iterations, 10, .lower, .{}));
            }

            b.installArtifact(exe);
        }
    }
}
