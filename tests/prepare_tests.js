#!/usr/bin/env node

const sevenBin = require('7zip-bin')
const { extractFull } = require('node-7z')
const del = require('del');

// Delete current extracted folders
let extractedTests = './extracted_tests'
let testArchive = './test-archive.7z'
del.sync(extractedTests)

// Make new test archive
const pathTo7zip = sevenBin.path7za
const seven = extractFull(testArchive, '.', {
  $bin: pathTo7zip
})