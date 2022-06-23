const addon = require('./build/Release/testaddon.node')
const fs = require('fs');
const TestSets = { B2C: 'b2c', B2B_SMOKE: 'b2b-smoke', B2B_REGRESS: 'b2b-regress'};

function writeToFile(fileName, array){
    var file = fs.createWriteStream(fileName);
    file.write(array);
    file.close();
}

function appendToFile(fileName, array){
    array.forEach(function(v) { fs.appendFileSync(fileName, v + "\n") });
}

async function stopTests(isStop){
    if (isStop){
        addon.stopTests();
    }
}

addon.setup();

var arr = addon.chooseTestSet(TestSets.B2C);
addon.deleteTemplate("templ3");
addon.copyTemplate("templ1", "templ3");
addon.renameFile("newMenu.spec.ts", "menu.spec.ts", "templ3");

console.log("ok");
