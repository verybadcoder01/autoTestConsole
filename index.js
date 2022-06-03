const addon = require('./build/Release/testaddon.node')

const TestSets = { B2C: 'b2c', B2B_SMOKE: 'b2b-smoke', B2B_REGRESS: 'b2b-regress'};

addon.chooseTestSet(TestSets.B2C);

console.log("ok");