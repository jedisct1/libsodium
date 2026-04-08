const std = @import("std");
const json = std.json;
const Allocator = std.mem.Allocator;

pub const Validity = enum { valid, invalid, acceptable };

pub const Result = struct {
    passed: u32 = 0,
    failed: u32 = 0,
    skipped: u32 = 0,
};

pub const AeadVector = struct {
    tc_id: u32,
    key: []const u8,
    nonce: []const u8,
    aad: []const u8,
    msg: []const u8,
    ct: []const u8,
    tag: []const u8,
    validity: Validity,
};

pub const HashVector = struct {
    tc_id: u32,
    msg: []const u8,
    digest: []const u8,
    out_len: usize,
    key: []const u8,
    validity: Validity,
};

pub const MacVector = struct {
    tc_id: u32,
    key: []const u8,
    msg: []const u8,
    mac: []const u8,
    validity: Validity,
};

pub const XdhVector = struct {
    tc_id: u32,
    public_key: []const u8,
    private_key: []const u8,
    shared: []const u8,
    validity: Validity,
};

pub const EddsaVerifyVector = struct {
    tc_id: u32,
    public_key: []const u8,
    msg: []const u8,
    sig: []const u8,
    validity: Validity,
};

pub const EddsaSignVector = struct {
    tc_id: u32,
    private_key: []const u8,
    public_key: []const u8,
    msg: []const u8,
    sig: []const u8,
    validity: Validity,
};

pub const AuthEncVector = struct {
    tc_id: u32,
    key: []const u8,
    nonce: []const u8,
    msg: []const u8,
    ct: []const u8,
    tag: []const u8,
    validity: Validity,
};

pub const BoxVector = struct {
    tc_id: u32,
    public_key: []const u8,
    private_key: []const u8,
    nonce: []const u8,
    msg: []const u8,
    ct: []const u8,
    validity: Validity,
    shared_zero: bool,
};

pub const StreamVector = struct {
    tc_id: u32,
    key: []const u8,
    nonce: []const u8,
    msg: []const u8,
    ct: []const u8,
    validity: Validity,
};

pub const HkdfVector = struct {
    tc_id: u32,
    ikm: []const u8,
    salt: []const u8,
    info: []const u8,
    out_len: usize,
    okm: []const u8,
    validity: Validity,
};

pub const ScryptVector = struct {
    tc_id: u32,
    password: []const u8,
    salt: []const u8,
    n: u64,
    r: u32,
    p: u32,
    dk_len: usize,
    dk: []const u8,
    validity: Validity,
};

pub const XofVector = struct {
    tc_id: u32,
    msg: []const u8,
    out_len: usize,
    output: []const u8,
    validity: Validity,
};

pub const MlkemVector = struct {
    tc_id: u32,
    seed: []const u8,
    ek: []const u8,
    ct: []const u8,
    ss: []const u8,
    validity: Validity,
};

const TestFile = struct {
    parsed: json.Parsed(json.Value),
    root: json.ObjectMap,
    tests: []const json.Value,
};

fn openTestFile(allocator: Allocator, data: []const u8) !TestFile {
    const parsed = try json.parseFromSlice(json.Value, allocator, data, .{
        .ignore_unknown_fields = true,
    });
    const root = getObj(parsed.value) orelse return error.InvalidVector;
    const tests = getArr(root.get("tests") orelse return error.InvalidVector) orelse return error.InvalidVector;
    return .{ .parsed = parsed, .root = root, .tests = tests };
}

fn getObj(val: json.Value) ?json.ObjectMap {
    return switch (val) {
        .object => |o| o,
        else => null,
    };
}

fn getArr(val: json.Value) ?[]const json.Value {
    return switch (val) {
        .array => |a| a.items,
        else => null,
    };
}

fn getStr(obj: json.ObjectMap, key: []const u8) ?[]const u8 {
    const v = obj.get(key) orelse return null;
    return switch (v) {
        .string => |s| s,
        else => null,
    };
}

fn getInt(obj: json.ObjectMap, key: []const u8) ?i64 {
    const v = obj.get(key) orelse return null;
    return switch (v) {
        .integer => |i| i,
        else => null,
    };
}

fn getBool(obj: json.ObjectMap, key: []const u8) ?bool {
    const v = obj.get(key) orelse return null;
    return switch (v) {
        .bool => |b| b,
        else => null,
    };
}

fn hasFlag(obj: json.ObjectMap, flag: []const u8) bool {
    const flags = switch (obj.get("flags") orelse return false) {
        .array => |a| a.items,
        else => return false,
    };
    for (flags) |f| {
        const s = switch (f) {
            .string => |s| s,
            else => continue,
        };
        if (std.mem.eql(u8, s, flag)) return true;
    }
    return false;
}

fn hexField(allocator: Allocator, obj: json.ObjectMap, key: []const u8) ![]const u8 {
    const s = getStr(obj, key) orelse return &.{};
    const out = try allocator.alloc(u8, s.len / 2);
    return std.fmt.hexToBytes(out, s) catch |err| {
        allocator.free(out);
        return err;
    };
}

fn rooterbergValidity(obj: json.ObjectMap) Validity {
    const v = getBool(obj, "valid") orelse return .valid;
    return if (v) .valid else .invalid;
}

pub fn loadAeadVectors(allocator: Allocator, data: []const u8) ![]AeadVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(AeadVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .key = try hexField(allocator, obj, "key"),
            .nonce = try hexField(allocator, obj, "iv"),
            .aad = try hexField(allocator, obj, "aad"),
            .msg = try hexField(allocator, obj, "msg"),
            .ct = try hexField(allocator, obj, "ct"),
            .tag = try hexField(allocator, obj, "tag"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadHashVectors(allocator: Allocator, data: []const u8) ![]HashVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var out_len: usize = 0;
    if (getObj(tf.root.get("algorithm") orelse return error.InvalidVector)) |algo| {
        if (getInt(algo, "digestSize")) |ds| out_len = @intCast(@divExact(ds, 8));
    }

    var list = try std.ArrayList(HashVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .msg = try hexField(allocator, obj, "msg"),
            .digest = try hexField(allocator, obj, "digest"),
            .out_len = out_len,
            .key = try hexField(allocator, obj, "key"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadMacVectors(allocator: Allocator, data: []const u8) ![]MacVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(MacVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .key = try hexField(allocator, obj, "key"),
            .msg = try hexField(allocator, obj, "msg"),
            .mac = try hexField(allocator, obj, "mac"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadXdhVectors(allocator: Allocator, data: []const u8) ![]XdhVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(XdhVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .public_key = try hexField(allocator, obj, "publicKey"),
            .private_key = try hexField(allocator, obj, "privateKey"),
            .shared = try hexField(allocator, obj, "shared"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadEddsaVerifyVectors(allocator: Allocator, data: []const u8) ![]EddsaVerifyVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(EddsaVerifyVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .public_key = try hexField(allocator, obj, "publicKey"),
            .msg = try hexField(allocator, obj, "msg"),
            .sig = try hexField(allocator, obj, "sig"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadEddsaSignVectors(allocator: Allocator, data: []const u8) ![]EddsaSignVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(EddsaSignVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .private_key = try hexField(allocator, obj, "privateKey"),
            .public_key = try hexField(allocator, obj, "publicKey"),
            .msg = try hexField(allocator, obj, "msg"),
            .sig = try hexField(allocator, obj, "sig"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadAuthEncVectors(allocator: Allocator, data: []const u8) ![]AuthEncVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(AuthEncVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .key = try hexField(allocator, obj, "key"),
            .nonce = try hexField(allocator, obj, "iv"),
            .msg = try hexField(allocator, obj, "msg"),
            .ct = try hexField(allocator, obj, "ct"),
            .tag = try hexField(allocator, obj, "tag"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadBoxVectors(allocator: Allocator, data: []const u8) ![]BoxVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(BoxVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .public_key = try hexField(allocator, obj, "publicKey"),
            .private_key = try hexField(allocator, obj, "privateKey"),
            .nonce = try hexField(allocator, obj, "iv"),
            .msg = try hexField(allocator, obj, "msg"),
            .ct = try hexField(allocator, obj, "ct"),
            .validity = rooterbergValidity(obj),
            .shared_zero = hasFlag(obj, "SharedZero"),
        });
    }
    return list.items;
}

pub fn loadStreamVectors(allocator: Allocator, data: []const u8) ![]StreamVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(StreamVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .key = try hexField(allocator, obj, "key"),
            .nonce = try hexField(allocator, obj, "iv"),
            .msg = try hexField(allocator, obj, "msg"),
            .ct = try hexField(allocator, obj, "ct"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadHkdfVectors(allocator: Allocator, data: []const u8) ![]HkdfVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(HkdfVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        const out_len: usize = @intCast(getInt(obj, "outLen") orelse continue);
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .ikm = try hexField(allocator, obj, "ikm"),
            .salt = try hexField(allocator, obj, "salt"),
            .info = try hexField(allocator, obj, "info"),
            .out_len = out_len,
            .okm = try hexField(allocator, obj, "okm"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadScryptVectors(allocator: Allocator, data: []const u8) ![]ScryptVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(ScryptVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .password = try hexField(allocator, obj, "password"),
            .salt = try hexField(allocator, obj, "salt"),
            .n = @intCast(getInt(obj, "n") orelse continue),
            .r = @intCast(getInt(obj, "r") orelse continue),
            .p = @intCast(getInt(obj, "p") orelse continue),
            .dk_len = @intCast(getInt(obj, "dkLen") orelse continue),
            .dk = try hexField(allocator, obj, "dk"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

pub fn loadXofVectors(allocator: Allocator, data: []const u8) ![]XofVector {
    var tf = try openTestFile(allocator, data);
    defer tf.parsed.deinit();

    var list = try std.ArrayList(XofVector).initCapacity(allocator, tf.tests.len);
    for (tf.tests) |t| {
        const obj = getObj(t) orelse continue;
        const out_len: usize = @intCast(getInt(obj, "size") orelse
            (getInt(obj, "outLen") orelse continue));
        try list.append(allocator, .{
            .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
            .msg = try hexField(allocator, obj, "ikm"),
            .out_len = out_len,
            .output = try hexField(allocator, obj, "okm"),
            .validity = rooterbergValidity(obj),
        });
    }
    return list.items;
}

fn wycheproofValidity(obj: json.ObjectMap) Validity {
    const s = getStr(obj, "result") orelse return .valid;
    if (std.mem.eql(u8, s, "valid")) return .valid;
    if (std.mem.eql(u8, s, "acceptable")) return .acceptable;
    return .invalid;
}

pub fn loadMlkemVectors(allocator: Allocator, data: []const u8) ![]MlkemVector {
    const parsed = try json.parseFromSlice(json.Value, allocator, data, .{
        .ignore_unknown_fields = true,
    });
    defer parsed.deinit();
    const root = getObj(parsed.value) orelse return error.InvalidVector;
    const groups = getArr(root.get("testGroups") orelse return error.InvalidVector) orelse return error.InvalidVector;

    var list: std.ArrayList(MlkemVector) = .empty;
    for (groups) |g| {
        const group = getObj(g) orelse continue;
        const tests = getArr(group.get("tests") orelse continue) orelse continue;
        for (tests) |t| {
            const obj = getObj(t) orelse continue;
            try list.append(allocator, .{
                .tc_id = @intCast(getInt(obj, "tcId") orelse continue),
                .seed = try hexField(allocator, obj, "seed"),
                .ek = try hexField(allocator, obj, "ek"),
                .ct = try hexField(allocator, obj, "c"),
                .ss = try hexField(allocator, obj, "K"),
                .validity = wycheproofValidity(obj),
            });
        }
    }
    return list.items;
}
