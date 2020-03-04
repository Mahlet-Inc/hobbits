#!/usr/bin/env node

const sevenBin = require('7zip-bin')
const { extractFull, add } = require('node-7z')

const { readdirSync, unlinkSync, existsSync } = require('fs')
const { join } = require('path')
const { execFileSync } = require("child_process");
const glob = require('glob');

// Delete test output
let testOutputMatches = glob.sync('extracted_tests/**/testrunoutput.*', {nonull: false})
for (let oldOutput of testOutputMatches) {
    unlinkSync(oldOutput)
}

// Delete old test archive
let testArchive = './test-archive.7z'
if (existsSync(testArchive)) {
    unlinkSync(testArchive)
}

// Make new test archive
const pathTo7zip = sevenBin.path7za

const myStream = add(testArchive, './extracted_tests', {
    recursive: true,
    $bin: pathTo7zip
})