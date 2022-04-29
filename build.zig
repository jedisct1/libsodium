const std = @import("std");
const builtin = @import("builtin");
const fmt = std.fmt;
const fs = std.fs;
const heap = std.heap;
const mem = std.mem;

pub fn build(b: *std.build.Builder) !void {
    var target = b.standardTargetOptions(.{});
    var mode = b.standardReleaseOptions();

    const libsodium = b.addStaticLibrary("sodium", null);
    libsodium.setTarget(target);
    libsodium.setBuildMode(mode);
    libsodium.install();
    if (mode != .Debug) {
        libsodium.strip = true;
    }
    libsodium.linkLibC();

    libsodium.addIncludeDir("src/libsodium/include/sodium");
    libsodium.defineCMacro("CONFIGURED", "1");
    libsodium.defineCMacro("DEV_MODE", "1");
    libsodium.defineCMacro("_GNU_SOURCE", "1");
    libsodium.defineCMacro("HAVE_INLINE_ASM", "1");
    libsodium.defineCMacro("HAVE_TI_MODE", "1");
    libsodium.defineCMacro("HAVE_ATOMIC_OPS", "1");

    switch (target.getCpuArch()) {
        .x86_64 => {
            libsodium.defineCMacro("HAVE_AMD64_ASM", "1");
            libsodium.defineCMacro("HAVE_AVX_ASM", "1");
            libsodium.defineCMacro("HAVE_CPUID", "1");
            libsodium.defineCMacro("HAVE_MMINTRIN_H", "1");
            libsodium.defineCMacro("HAVE_EMMINTRIN_H", "1");
            libsodium.defineCMacro("HAVE_PMMINTRIN_H", "1");
        },
        .aarch64, .aarch64_be => {
            libsodium.defineCMacro("HAVE_ARMCRYTO", "1");
        },
        .wasm32, .wasm64 => {
            libsodium.defineCMacro("__wasm__", "1");
        },
        else => {},
    }

    switch (target.getOsTag()) {
        .wasi => {
            libsodium.defineCMacro("__wasi__", "1");
        },
        else => {},
    }

    const base = "src/libsodium";
    const dir = try fs.Dir.openDir(fs.cwd(), base, .{ .iterate = true, .no_follow = true });
    var allocator = heap.page_allocator;
    var walker = try dir.walk(allocator);
    while (try walker.next()) |entry| {
        const name = entry.basename;
        if (mem.endsWith(u8, name, ".c")) {
            const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ base, entry.path });
            libsodium.addCSourceFiles(&.{full_path}, &.{
                "-fvisibility=hidden",
                "-fno-strict-aliasing",
                "-fno-strict-overflow",
                "-fwrapv",
                "-flax-vector-conversions",
            });
        } else if (mem.endsWith(u8, name, ".S")) {
            const full_path = try fmt.allocPrint(allocator, "{s}/{s}", .{ base, entry.path });
            libsodium.addAssemblyFile(full_path);
        }
    }
}
