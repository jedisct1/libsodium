const std = @import("std");
const fmt = std.fmt;
const heap = std.heap;
const mem = std.mem;
const Compile = std.Build.Step.Compile;
const Target = std.Target;

// Zig 0.16+ uses std.Io.Dir, 0.15 uses std.fs
const is_zig_16 = @hasDecl(std, "Io") and @hasDecl(std.Io, "Dir");
const Dir = if (is_zig_16) std.Io.Dir else std.fs.Dir;
const Io = if (is_zig_16) std.Io else void;

fn initLibConfig(b: *std.Build, target: std.Build.ResolvedTarget, lib: *Compile) void {
    lib.root_module.link_libc = true;
    lib.lto = null;
    lib.root_module.addIncludePath(b.path("src/libsodium/include/sodium"));
    lib.root_module.addCMacro("_GNU_SOURCE", "1");
    lib.root_module.addCMacro("CONFIGURED", "1");
    lib.root_module.addCMacro("DEV_MODE", "1");
    lib.root_module.addCMacro("HAVE_ATOMIC_OPS", "1");
    lib.root_module.addCMacro("HAVE_C11_MEMORY_FENCES", "1");
    lib.root_module.addCMacro("HAVE_CET_H", "1");
    lib.root_module.addCMacro("HAVE_GCC_MEMORY_FENCES", "1");
    lib.root_module.addCMacro("HAVE_INLINE_ASM", "1");
    lib.root_module.addCMacro("HAVE_INTTYPES_H", "1");
    lib.root_module.addCMacro("HAVE_STDINT_H", "1");
    lib.root_module.addCMacro("HAVE_TI_MODE", "1");

    const endian = target.result.cpu.arch.endian();
    switch (endian) {
        .big => lib.root_module.addCMacro("NATIVE_BIG_ENDIAN", "1"),
        .little => lib.root_module.addCMacro("NATIVE_LITTLE_ENDIAN", "1"),
    }

    switch (target.result.os.tag) {
        .linux => {
            lib.root_module.addCMacro("ASM_HIDE_SYMBOL", ".hidden");
            lib.root_module.addCMacro("TLS", "_Thread_local");

            lib.root_module.addCMacro("HAVE_CATCHABLE_ABRT", "1");
            lib.root_module.addCMacro("HAVE_CATCHABLE_SEGV", "1");
            lib.root_module.addCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.root_module.addCMacro("HAVE_GETPID", "1");
            lib.root_module.addCMacro("HAVE_MADVISE", "1");
            lib.root_module.addCMacro("HAVE_MLOCK", "1");
            lib.root_module.addCMacro("HAVE_MMAP", "1");
            lib.root_module.addCMacro("HAVE_MPROTECT", "1");
            lib.root_module.addCMacro("HAVE_NANOSLEEP", "1");
            lib.root_module.addCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD_PRIO_INHERIT", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD", "1");
            lib.root_module.addCMacro("HAVE_RAISE", "1");
            lib.root_module.addCMacro("HAVE_SYSCONF", "1");
            lib.root_module.addCMacro("HAVE_SYS_AUXV_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_MMAN_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_PARAM_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_RANDOM_H", "1");
            lib.root_module.addCMacro("HAVE_WEAK_SYMBOLS", "1");
        },
        .windows => {
            lib.root_module.addCMacro("HAVE_RAISE", "1");
            lib.root_module.addCMacro("HAVE_SYS_PARAM_H", "1");
            if (lib.isStaticLibrary()) {
                lib.root_module.addCMacro("SODIUM_STATIC", "1");
            }
        },
        .macos => {
            lib.root_module.addCMacro("ASM_HIDE_SYMBOL", ".private_extern");
            lib.root_module.addCMacro("TLS", "_Thread_local");

            lib.root_module.addCMacro("HAVE_ARC4RANDOM", "1");
            lib.root_module.addCMacro("HAVE_ARC4RANDOM_BUF", "1");
            lib.root_module.addCMacro("HAVE_CATCHABLE_ABRT", "1");
            lib.root_module.addCMacro("HAVE_CATCHABLE_SEGV", "1");
            lib.root_module.addCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.root_module.addCMacro("HAVE_GETENTROPY", "1");
            lib.root_module.addCMacro("HAVE_GETPID", "1");
            lib.root_module.addCMacro("HAVE_MADVISE", "1");
            lib.root_module.addCMacro("HAVE_MEMSET_S", "1");
            lib.root_module.addCMacro("HAVE_MLOCK", "1");
            lib.root_module.addCMacro("HAVE_MMAP", "1");
            lib.root_module.addCMacro("HAVE_MPROTECT", "1");
            lib.root_module.addCMacro("HAVE_NANOSLEEP", "1");
            lib.root_module.addCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD_PRIO_INHERIT", "1");
            lib.root_module.addCMacro("HAVE_RAISE", "1");
            lib.root_module.addCMacro("HAVE_SYSCONF", "1");
            lib.root_module.addCMacro("HAVE_SYS_MMAN_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_PARAM_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_RANDOM_H", "1");
            lib.root_module.addCMacro("HAVE_WEAK_SYMBOLS", "1");
        },
        .wasi => {
            lib.root_module.addCMacro("HAVE_ARC4RANDOM", "1");
            lib.root_module.addCMacro("HAVE_ARC4RANDOM_BUF", "1");
            lib.root_module.addCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.root_module.addCMacro("HAVE_GETENTROPY", "1");
            lib.root_module.addCMacro("HAVE_NANOSLEEP", "1");
            lib.root_module.addCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.root_module.addCMacro("HAVE_SYS_AUXV_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_PARAM_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_RANDOM_H", "1");
        },
        .freebsd => {
            lib.root_module.addCMacro("ASM_HIDE_SYMBOL", ".hidden");
            lib.root_module.addCMacro("TLS", "_Thread_local");

            lib.root_module.addCMacro("HAVE_ARC4RANDOM", "1");
            lib.root_module.addCMacro("HAVE_ARC4RANDOM_BUF", "1");
            lib.root_module.addCMacro("HAVE_CATCHABLE_ABRT", "1");
            lib.root_module.addCMacro("HAVE_CATCHABLE_SEGV", "1");
            lib.root_module.addCMacro("HAVE_CLOCK_GETTIME", "1");
            lib.root_module.addCMacro("HAVE_GETPID", "1");
            lib.root_module.addCMacro("HAVE_MADVISE", "1");
            lib.root_module.addCMacro("HAVE_MLOCK", "1");
            lib.root_module.addCMacro("HAVE_MMAP", "1");
            lib.root_module.addCMacro("HAVE_MPROTECT", "1");
            lib.root_module.addCMacro("HAVE_NANOSLEEP", "1");
            lib.root_module.addCMacro("HAVE_POSIX_MEMALIGN", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD_PRIO_INHERIT", "1");
            lib.root_module.addCMacro("HAVE_PTHREAD", "1");
            lib.root_module.addCMacro("HAVE_RAISE", "1");
            lib.root_module.addCMacro("HAVE_SYSCONF", "1");
            lib.root_module.addCMacro("HAVE_SYS_MMAN_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_PARAM_H", "1");
            lib.root_module.addCMacro("HAVE_SYS_RANDOM_H", "1");
            lib.root_module.addCMacro("HAVE_WEAK_SYMBOLS", "1");
        },
        else => {},
    }

    switch (target.result.cpu.arch) {
        .x86_64 => {
            switch (target.result.os.tag) {
                .windows => {},
                else => {
                    lib.root_module.addCMacro("HAVE_AMD64_ASM", "1");
                    lib.root_module.addCMacro("HAVE_AVX_ASM", "1");
                },
            }
            lib.root_module.addCMacro("HAVE_CPUID", "1");
            lib.root_module.addCMacro("HAVE_MMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_EMMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_PMMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_TMMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_SMMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_AVXINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_AVX2INTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_AVX512FINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_WMMINTRIN_H", "1");
            lib.root_module.addCMacro("HAVE_RDRAND", "1");
        },
        .aarch64, .aarch64_be => {
            lib.root_module.addCMacro("HAVE_ARMCRYPTO", "1");
        },
        .wasm32, .wasm64 => {
            lib.root_module.addCMacro("__wasm__", "1");
        },
        else => {},
    }

    switch (target.result.os.tag) {
        .wasi => {
            lib.root_module.addCMacro("__wasi__", "1");
        },
        else => {},
    }
}

pub fn build(b: *std.Build) !void {
    const io: Io = if (is_zig_16) b.graph.io else {};
    const root_path = b.pathFromRoot(".");
    const cwd = try if (is_zig_16) Dir.cwd().openDir(io, root_path, .{}) else std.fs.cwd().openDir(root_path, .{});

    const src_path = "src/libsodium";
    const src_dir = if (is_zig_16)
        try cwd.openDir(io, src_path, .{ .iterate = true })
    else if (@hasField(Dir.OpenOptions, "follow_symlinks"))
        try cwd.openDir(src_path, .{ .iterate = true, .follow_symlinks = false })
    else
        try cwd.openDir(src_path, .{ .iterate = true, .no_follow = true });

    var target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const enable_benchmarks = b.option(bool, "enable_benchmarks", "Whether tests should be benchmarks.") orelse false;
    const benchmarks_iterations = b.option(u32, "iterations", "Number of iterations for benchmarks.") orelse 200;
    var build_static = b.option(bool, "static", "Build libsodium as a static library.") orelse true;
    var build_shared = b.option(bool, "shared", "Build libsodium as a shared library.") orelse true;

    const build_tests = b.option(bool, "test", "Build the tests (implies -Dstatic=true)") orelse true;

    if (target.result.cpu.arch.isWasm()) {
        build_shared = false;
    }
    if (build_tests) {
        build_static = true;
    }

    switch (target.result.cpu.arch) {
        .aarch64, .aarch64_be => {
            // ARM CPUs supported by Windows are assumed to have NEON support
            if (target.result.isMinGW()) {
                target.query.cpu_features_add.addFeature(@intFromEnum(Target.aarch64.Feature.neon));
            }
        },
        else => {},
    }

    const static_lib = b.addLibrary(.{
        .name = if (target.result.isMinGW()) "libsodium-static" else "sodium",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    static_lib.pie = true;
    const shared_lib = b.addLibrary(.{
        .name = if (target.result.isMinGW()) "libsodium" else "sodium",
        .linkage = .dynamic,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .strip = optimize != .Debug and !target.result.isMinGW(),
        }),
    });

    // work out which libraries we are building
    var libs = std.ArrayList(*Compile){};
    defer libs.deinit(heap.page_allocator);
    if (build_static) {
        try libs.append(heap.page_allocator, static_lib);
    }
    if (build_shared) {
        try libs.append(heap.page_allocator, shared_lib);
    }

    const prebuilt_version_file_path = "builds/msvc/version.h";
    const version_file_path = "include/sodium/version.h";

    if (is_zig_16) {
        src_dir.access(io, version_file_path, .{}) catch {
            try Dir.copyFile(cwd, prebuilt_version_file_path, src_dir, version_file_path, io, .{});
        };
    } else if (@hasField(Dir.OpenOptions, "follow_symlinks")) {
        src_dir.access(version_file_path, .{ .read = true }) catch {
            try cwd.copyFile(prebuilt_version_file_path, src_dir, version_file_path, .{});
        };
    } else {
        src_dir.access(version_file_path, .{ .mode = .read_only }) catch {
            try cwd.copyFile(prebuilt_version_file_path, src_dir, version_file_path, .{});
        };
    }

    for (libs.items) |lib| {
        b.installArtifact(lib);
        lib.installHeader(b.path(src_path ++ "/include/sodium.h"), "sodium.h");
        lib.installHeadersDirectory(b.path(src_path ++ "/include/sodium"), "sodium", .{});

        initLibConfig(b, target, lib);

        const flags = &.{
            "-fvisibility=hidden",
            "-fno-strict-aliasing",
            "-fno-strict-overflow",
            "-fwrapv",
            "-flax-vector-conversions",
            "-Werror=vla",
        };

        const allocator = heap.page_allocator;

        var walker = try src_dir.walk(allocator);
        while (if (is_zig_16) try walker.next(io) else try walker.next()) |entry| {
            const name = entry.basename;
            if (mem.endsWith(u8, name, ".c")) {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });

                lib.root_module.addCSourceFiles(.{
                    .files = &.{full_path},
                    .flags = flags,
                });
            } else if (mem.endsWith(u8, name, ".S")) {
                const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ src_path, entry.path });
                lib.root_module.addAssemblyFile(b.path(full_path));
            }
        }
    }

    const test_path = "test/default";
    const out_bin_path = "zig-out/bin";
    const test_dir = if (is_zig_16)
        try cwd.openDir(io, test_path, .{ .iterate = true })
    else if (@hasField(Dir.OpenOptions, "follow_symlinks"))
        try cwd.openDir(test_path, .{ .iterate = true, .follow_symlinks = false })
    else
        try cwd.openDir(test_path, .{ .iterate = true, .no_follow = true });

    if (is_zig_16) {
        cwd.createDirPath(io, out_bin_path) catch {};
    } else {
        cwd.makePath(out_bin_path) catch {};
    }
    const out_bin_dir = if (is_zig_16)
        try cwd.openDir(io, out_bin_path, .{})
    else
        try cwd.openDir(out_bin_path, .{});

    if (is_zig_16) {
        try Dir.copyFile(test_dir, "run.sh", out_bin_dir, "run.sh", io, .{});
    } else {
        try test_dir.copyFile("run.sh", out_bin_dir, "run.sh", .{});
    }

    const allocator = heap.page_allocator;
    var walker = try test_dir.walk(allocator);

    const test_step = b.step("test", "Run all libsodium tests");

    if (build_tests) {
        while (if (is_zig_16) try walker.next(io) else try walker.next()) |entry| {
            const name = entry.basename;
            if (mem.endsWith(u8, name, ".exp")) {
                if (is_zig_16) {
                    try Dir.copyFile(test_dir, name, out_bin_dir, name, io, .{});
                } else {
                    try test_dir.copyFile(name, out_bin_dir, name, .{});
                }
                continue;
            }
            if (!mem.endsWith(u8, name, ".c")) {
                continue;
            }
            const exe_name = name[0 .. name.len - 2];
            var exe = b.addExecutable(.{
                .name = exe_name,
                .root_module = b.createModule(.{
                    .target = target,
                    .optimize = optimize,
                    .strip = true,
                    .link_libc = true,
                }),
            });
            exe.root_module.linkLibrary(static_lib);
            exe.root_module.addIncludePath(b.path("src/libsodium/include"));
            exe.root_module.addIncludePath(b.path("test/quirks"));
            const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ test_path, entry.path });
            exe.root_module.addCSourceFiles(.{ .files = &.{full_path} });
            if (enable_benchmarks) {
                exe.root_module.addCMacro("BENCHMARKS", "1");
                var buf: [16]u8 = undefined;
                const n = std.fmt.bufPrint(&buf, "{d}", .{benchmarks_iterations}) catch unreachable;
                exe.root_module.addCMacro("ITERATIONS", n);
            }

            b.installArtifact(exe);

            // Add a run step for this test
            const run_test = b.addRunArtifact(exe);
            run_test.setCwd(b.path(test_path));
            test_step.dependOn(&run_test.step);
        }
    }
}
