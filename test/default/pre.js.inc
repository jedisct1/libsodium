try {
    this['Module'] = Module;
    Module.test;
} catch(e) {
    this['Module'] = Module = {};
}
Module['preRun'] = Module['preRun'] || [];
Module['preRun'].push(function(){
    FS.init();
    FS.mkdir('/test-data');
    FS.mount(NODEFS,  { root: '.' }, '/test-data');
});
