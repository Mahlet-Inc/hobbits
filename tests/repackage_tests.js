#!/usr/bin/env node

const sevenBin = require('7zip-bin')
const { add } = require('node-7z')

const { unlinkSync, existsSync } = require('fs')
const glob = require('glob');

// Delete test output
console.log(`Removing test output before archiving...`)
let testOutputMatches = glob.sync('extracted_tests/**/testrunoutput.*', {nonull: false})
for (let oldOutput of testOutputMatches) {
    unlinkSync(oldOutput)
}

// Delete old test archive
let testArchive = './test-archive.7z'
if (existsSync(testArchive)) {
    console.log(`Removing old ${testArchive}...`)
    unlinkSync(testArchive)
}

// Make new test archive
console.log(`Creating new archive...`)
const pathTo7zip = sevenBin.path7za
const archiveStream = add(testArchive, './extracted_tests', {
    latestTimestamp: true,
    recursive: true,
    $bin: pathTo7zip
})

archiveStream.on('end', () => {
    console.log(`Tests packaged into ${testArchive} and ready to commit`)
})