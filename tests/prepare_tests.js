#!/usr/bin/env node

const sevenBin = require('7zip-bin')
const { extractFull } = require('node-7z')
const del = require('del');
const { existsSync } = require('fs')

// Delete current extracted folders
let extractedTests = './extracted_tests'
let testArchive = './test-archive.7z'
if (existsSync(extractedTests)) {
    console.log(`Deleting existing ${extractedTests} folder...`)
    del.sync(extractedTests)
}

// Make new test archive
const pathTo7zip = sevenBin.path7za
const archiveStream = extractFull(testArchive, '.', {
  $bin: pathTo7zip
})

archiveStream.on('end', () => {
    console.log(`Tests extracted into ${extractedTests}. Run them with test_hobbits.js`)
})