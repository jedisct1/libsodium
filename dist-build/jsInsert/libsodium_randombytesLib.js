LibraryRandomBytes = {
    $RandomBytes__postset: "Module['RandomBytes'] = RandomBytes;\n",
    // ^ for later mutation by client modules
    $RandomBytes: {
	crypto: null
    },

    randombytes: function (target, countlow, counthigh) {
	if (counthigh) throw {message: "_randombytes count overflow"};
	HEAPU8.set(RandomBytes.crypto["randomBytes"](countlow), target);
	return 0;
    }
};

autoAddDeps(LibraryRandomBytes, "$RandomBytes");
mergeInto(LibraryManager.library, LibraryRandomBytes);
