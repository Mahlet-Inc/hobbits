#!/usr/bin/env node

const { readdirSync, unlinkSync, existsSync } = require('fs')
const { join } = require('path')
const { execFileSync } = require("child_process");
const glob = require('glob');
const filecompare = require('filecompare');

const argv = require('yargs').command('* <hobbits_runner>', 'Tests hobbits processing with known input/output files for various batches', (yargs) => {
    yargs.positional('hobbits_runner', {
        describe: 'the path of the hobbits-runner binary you want to test',
        type: 'string'
    });
}).help().alias('help', 'h').argv;


async function runTests() {
    let tests = 0;
    let failures = 0;
    let successes = 0;

    let baseDir = join(__dirname, 'extracted_tests')
    
    testDirs = readdirSync(baseDir, { withFileTypes: true })
    .filter(dirent => dirent.isDirectory())
    .map(dirent => dirent.name)
    
    for (let testDir of testDirs) {
        tests++
        console.log(`Testing ${testDir}...`)
        testDir = join(baseDir, testDir)
    
        try {
            let inputGlob = join(testDir, "input*.bits")
            let inputMatches = glob.sync(inputGlob, {nonull: false})
            if (inputMatches.length < 1) {
                throw Error(`Failed to find input files matching pattern ${inputGlob}!`)
            }
            let outputGlob = join(testDir, "output.bits*")
            let outputMatches = glob.sync(outputGlob, {nonull: false})
            if (outputMatches.length < 1) {
                throw Error(`Failed to find output files matching pattern ${outputGlob}!`)
            }
            let testOutputPrefix = join(testDir, "testrunoutput.")
            let testOutputGlob = testOutputPrefix+"*"
            let batchGlob = join(testDir, "*.hobbits_batch")
            let batchMatches = glob.sync(batchGlob, {nonull: false})
            if (batchMatches.length < 1) {
                throw Error(`Failed to find a test batch file matching ${batchGlob}`)
            }
            let batch = batchMatches[0]

            let testOutputMatches = glob.sync(testOutputGlob, {nonull: false})
            for (let oldOutput of testOutputMatches) {
                unlinkSync(oldOutput)
            }

            let args = [
                'run',
                '-b', batch,
                '-o', testOutputPrefix
            ]

            inputMatches.sort()
            inputMatches.forEach(input => {
                args.push('-i')
                args.push(input)
            })

            execFileSync(argv.hobbits_runner, args, {stdio: 'inherit'});
    
            testOutputMatches = glob.sync(testOutputGlob, {nonull: false})
            if (testOutputMatches.length < 1) {
                throw Error(`Failed to find an output file matching ${testOutputGlob}`)
            }
            let testOutputs = {}
            let max = -1;
            let lastOutput = testOutputMatches[0]
            for (let outputFile of testOutputMatches) {
                let thisVal = parseInt(outputFile.split(".")[1]);
                if (thisVal > max) {
                    lastOutput = outputFile;
                }
                testOutputs[thisVal] = outputFile
            }

    
            let match = false
            let outputIndexedGlob = join(testDir, "output.bits.*")
            let outputIndexedMatches = glob.sync(outputIndexedGlob, {nonull: false})
            if (outputIndexedMatches.length < 1) {
                match = await new Promise((resolve, reject) => {
                    filecompare(outputMatches[0], lastOutput, isEqual => resolve(isEqual))
                })
            }
            else {
                for (let outputFile of outputIndexedMatches) {
                    let key = parseInt(outputFile.split(".")[2]);
                    match = await new Promise((resolve, reject) => {
                        filecompare(outputFile, testOutputs[key], isEqual => resolve(isEqual))
                    })
                    if (!match) {
                        break;
                    }
                }
            }
            
            
    
            if (match) {
                successes++;
                console.log("=> PASS");
            }
            else {
                failures++;
                console.log("=> FAIL");
            }
    
        } catch (err) {
            console.error(`=> ERROR: ${err}`)
            failures++;
        }
    
    }
    
    console.log(`Finished running ${tests} tests`)
    console.log(`Successes: ${successes}`)
    console.log(`Failures: ${failures}`)
    
    if (failures > 0) {
        return -1
    }
    
    if (tests < 1) {
        console.log("Treating lack of tests as failure...")
        return -1
    }

    return 0
}

runTests()
.then(val => process.exitCode = val)
.catch((err) => {
    console.error(err)
    process.exitCode = -1
})