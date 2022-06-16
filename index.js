const addon = require('./build/Release/testaddon.node')
const fs = require('fs')
const TestSets = { B2C: 'b2c', B2B_SMOKE: 'b2b-smoke', B2B_REGRESS: 'b2b-regress'};

function writeToFile(fileName, array){
    var file = fs.createWriteStream(fileName);
    array.forEach(function(v) { file.write(v + '\n'); });
}


function appendToFile(fileName, array){
    array.forEach(function(v) { fs.appendFileSync(fileName, v + "\n") });
}

addon.setup();

var arr = addon.chooseTestSet(TestSets.B2C);
var tc;
for (var i = 0; i < arr.length; ++i){
    tc = addon.getTestsFromFile(arr[i]);
    appendToFile('out.txt', tc);
}

console.log("ok");
