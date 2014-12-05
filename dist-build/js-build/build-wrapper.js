var fs = require('fs');
var path = require('path');

//Loading preset macros
var macros = {};
var macrosFiles = fs.readdirSync(path.join(__dirname, 'macros'));
for (var i = 0; i < macrosFiles.length; i++){
	var macroName = macrosFiles[i].replace('.js', '');
	var macroCode = fs.readFileSync(path.join(__dirname, 'macros', macrosFiles[i]), {encoding: 'utf8'});
	macros[macroName] = macroCode;
}
//console.log('Macros loaded: ' + JSON.stringify(macros, '', '\t'));

var templateCode = fs.readFileSync(path.join(__dirname, 'wrap-template.js'), {encoding: 'utf8'});

var scriptBuffer = templateCode;
var functionsCode = '';
var exportsCode = '';

//Load symbols. Write their wrapping code without checking their existence in emcc-built library
var symbols = [];
var symbolsFiles = fs.readdirSync(path.join(__dirname, 'symbols')).sort();
for (var i = 0; i < symbolsFiles.length; i++){
	var currentSymbol = fs.readFileSync(path.join(__dirname, 'symbols', symbolsFiles[i]), {encoding: 'utf8'});
	try {
		currentSymbol = JSON.parse(currentSymbol);
	} catch (e){
		console.error('Invalid symbol file for ' + symbolsFiles[i]);
		process.exit(1);
	}
	symbols.push(currentSymbol);
}
//console.log('Symbols descriptions loaded: ' + JSON.stringify(symbols, '', '\t'));

//Load constants symbols
loadConstants();

for (var i = 0; i < symbols.length; i++){
	buildSymbol(symbols[i]);
}
finalizeWrapper();

function buildSymbol(symbolDescription){
	if (typeof symbolDescription != 'object') throw new TypeError('symbolDescription must be a function');
	//console.log('Building symbol: ' + JSON.stringify(symbolDescription));
	//console.log('symbol type: ' + symbolDescription.type);
	if (symbolDescription.type == 'function'){
		var targetName = 'libsodium_raw._' + symbolDescription.name;
		//Add encoding parameter to input list if encodingChoice is true
		if (!symbolDescription.noEncoding){
			symbolDescription.inputs.push({name: "resultEncoding", type: "encoding"})
		}
		var funcCode = '\n\tfunction ' + symbolDescription.name + '(';
		var funcBody = '';
		//Adding parameters array in function's interface, their conversions in the function's body
		var paramsArray = '';
		for (var i = 0; i < symbolDescription.inputs.length; i++){
			//Adding parameter in function's parameter list
			var currentParameter = symbolDescription.inputs[i];
			var currentParameterCode;
			paramsArray += currentParameter.name;
			if (i != symbolDescription.inputs.length - 1) paramsArray += ', ';
			//Adding the correspondant parameter handling macro, into the function body
			if (currentParameter.type == 'buffer'){
				currentParameterCode = macros['input_buf'];
				currentParameterCode = applyMacro(currentParameterCode, ['{var_name}', '{var_size}'], [currentParameter.name, currentParameter.size]);
			} else if (currentParameter.type == 'uint'){
				currentParameterCode = macros['input_uint'];
				currentParameterCode = applyMacro(currentParameterCode, ['{var_name}'], [currentParameter.name]);
			} else if (currentParameter.type == 'unsized_buffer'){
				currentParameterCode = macros['input_unsized_buf'];
				currentParameterCode = applyMacro(currentParameterCode, ['{var_name}'], [currentParameter.name]);
			} else if (currentParameter.type == 'encoding'){
				currentParameterCode = macros['input_encoding'];
				currentParameterCode = applyMacro(currentParameterCode, ['{var_name}'], [currentParameter.name]);
			} else {
				//Unknown parameter type. What to do?
				console.log('What the hell is the input type ' + currentParameter.type + ' ?');
				process.exit(1);
			}
			//console.log(currentParameterCode);
			funcBody += currentParameterCode + '\n';
		}
		funcCode += paramsArray + '){\n\t';
		//funcCode += 'if (typeof ' + targetName + ' != \'function\') throw new TypeError(\'Undefined function\');\n\t';
		//Writing the outputs declaration code
		for (var i = 0; i < symbolDescription.outputs.length; i++){
			var currentOutput = symbolDescription.outputs[i];
			var currentOutputCode;
			if (currentOutput.type == 'buffer'){
				currentOutputCode = macros['output_buf'];
				currentOutputCode = applyMacro(currentOutputCode, ['{var_name}', '{var_size}'], [currentOutput.name, currentOutput.size]);
			} else if (!(currentOutput.type == 'uint')) {
				console.log('What the hell is the output type ' + currentOutput.type + ' ?');
				process.exit(1);
			}
			//console.log(currentOutputCode);
			funcBody += currentOutputCode + '\n';
		}
		//Writing the target call
		funcBody += sc(symbolDescription.target) + '\n';
		funcBody += sc(symbolDescription.return) + '\n';
		funcBody = injectTabs(funcBody);
		funcCode += funcBody + '}\n';

		functionsCode += funcCode;
		//console.log('Function code:\n' + funcCode);
		exportsCode += '\n\tif (typeof ' + targetName + ' == \'function\')  exports.' + symbolDescription.name + ' = ' + symbolDescription.name + ';';
	} else if (symbolDescription.type == 'uint'){
		var constVal = symbolDescription.target;
		var symbolName = symbolDescription.target.replace(new RegExp(/\(\)$/), '');
		if (!(/\(\)$/.test(constVal))) constVal += '()'; //Add the () for a function call
		exportsCode += '\n\tif (typeof ' + symbolName + ' == \'function\') exports.' + symbolDescription.name + ' = ' + constVal + ';';
	} else {
		console.log('What the hell is the symbol type ' + symbolDescription.type + ' ?');
		process.exit(1);
	}

}

function applyMacro(macroCode, symbols, substitutes){
	if (typeof macroCode != 'string') throw new TypeError('macroCode must be a string');
	if (!(Array.isArray(symbols) && checkStrArray(symbols))) throw new TypeError('symbols must be an array of strings');
	if (!(Array.isArray(substitutes) && checkStrArray(substitutes))) throw new TypeError('substitutes must be an array of strings');
	if (symbols.length > substitutes.length) throw new TypeError('invalid array length for substitutes');

	for (var i = 0; i < symbols.length; i++){
		macroCode = macroCode.replace(new RegExp(symbols[i], 'g'), substitutes[i]);
	}

	return macroCode;
}

function finalizeWrapper(){
	scriptBuffer = applyMacro(scriptBuffer, ['{wraps_here}', '{exports_here}'], [functionsCode, exportsCode]);
	fs.writeFileSync(path.join(__dirname, '../../libsodium-js/lib', 'libsodium-wrap.js'), scriptBuffer);
}

function injectTabs(code){
	return code.replace(/\n/g, '\n\t');
}

function loadConstants(){
	var constList = fs.readFileSync(path.join(__dirname, 'constants.json'), {encoding: 'utf8'});
	try {
		constList = JSON.parse(constList);
	} catch (e){
		console.error('Invalid constants list');
		process.exit(1);
	}
	if (!(Array.isArray(constList) && checkStrArray(constList))){
		console.error('constants file must contain an array of strings');
		process.exit(1);
	}
	for (var i = 0; i < constList.length; i++){
		var currentConstant = {
			name: constList[i],
			type: "uint",
			target: "libsodium_raw._" + constList[i] + '()'
		}
		symbols.push(currentConstant);
	}
}

function checkStrArray(a){
	for (var i = 0; i < a.length; i++) if (typeof a[i] != 'string') return false;
	return true;
}

//Inject a semi-colon at the end of the line, if one is missing
function sc(s){
	if (s.lastIndexOf(';') != s.length - 1) return s + ';';
	else return s;
}
