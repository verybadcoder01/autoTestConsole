const addon = require('./build/Release/testaddon.node')
const fs = require('fs')
const TestSets = { B2C: 'b2c', B2B_SMOKE: 'b2b-smoke', B2B_REGRESS: 'b2b-regress'};

function writeToFile(fileName, array){
    var file = fs.createWriteStream(fileName);
    array.forEach(function(v) { file.write(v + '\n'); });
    file.end();
}

addon.chooseTestSet(TestSets.B2C);
addon.createTemplate("templ1");
addon.addExistingTest("templ1", "rejected.spec.ts");
var arr = addon.runTestsInTemplate("templ1");
writeToFile("out.rtf", arr);
addon.deleteTemplate("templ1");


console.log("ok");
