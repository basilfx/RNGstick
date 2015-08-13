import sys
import os
import formic

# Tests to disable
DISABLED_TESTS = [
    "dab_bytedistrib",
    "marsaglia_tsang_gcd",
    "rgb_minimum_distance",
    "rgb_kstest_test"
]

PROPERTIES = {
    "sampleReset": ("Sample reset", lambda x: "Yes" if x else "No"),
    "sampleBucket": ("Sample bucket size", lambda x:  "%s b" % x),
    "sampleMultiple": ("Sample multiple", lambda x: "%s b" % x),
    "delayHold": ("Sample hold", lambda x: "%s us" % x),
    "delaySkip": ("Sample skip", lambda x:  "%s us" % x),
    "delayRound": ("Delay round", lambda x: "%s us" % x),
    "sampleMask": ("Sample mask", hex),
    "sampleShift": ("Sample shift", str),
    "sampleTake": ("Sample take", str),
    "mode": ("Mode", lambda x: "Raw" if x == 1 else "Ones-majority"),
    "extractor": ("Extractor", lambda x: "None" if x == 1 else "Von Neumann" if x == 2 else "Improved Von Neumann"),
    "generator": ("Generator", lambda x: "None" if x == 1 else "SHA-1"),
    "output": ("Output", lambda x: "Bits" if x == 1 else "Bytes"),
    "outputMode": ("Output mode", lambda x: "Raw" if x == 1 else "ASCII" if x == 2 else "ASCII + Newline"),
    "outputBucket": ("Output bucket", lambda x: "%s b" % x)
}


def main(argv):
    if len(argv) < 2:
        sys.stdout.write("Syntax: %s <input_dir>\n" % argv[0])
        return 1

    input_dir = os.path.abspath(argv[1])

    # Find files
    file_set = formic.FileSet(include="**/*.diehard", directory=input_dir)
    filenames = list(file_set)

    # Print header
    sys.stdout.write("ID,File size,ENT,Entropy,Chi-square,Mean,Monte-Carlo-Pi,Serial-Correlation,DIEHARDER,Total,Passed,Failed,Weak,CONFIGURATION,")
    sys.stdout.write(",".join([x[0] for x in PROPERTIES.itervalues()]) + "\n")

    for filename in filenames:
        # Resolve filenames
        test_id = os.path.basename(filename).rsplit(".", 1)[0]
        filename_dh = filename
        filename_conf = filename.rsplit(".", 1)[0] + ".txt"
        filename_ent = filename.rsplit(".", 1)[0] + ".ent"
        filename_data = filename.rsplit(".", 1)[0] + ".bin"

        file_size = os.stat(filename_data).st_size

        # Initialize counters
        total = 0
        weak = 0
        failed = 0
        passed = 0
        state = 0

        # Read DIEHARDER file and scan for test results
        with open(filename_dh, "r") as fp:
            for line in fp:
                # Search for 'test_name'
                if state == 0:
                    # Unpack line
                    parts = map(lambda x: x.strip(), line.split("|"))

                    if parts[0] == "test_name":
                        state = 1

                # Next line is comment
                elif state == 1:
                    state = 2

                # Read all test results
                elif state == 2:
                    # Test for test end
                    if line[:3] == "#==":
                        state = 0
                        continue

                    # Split results
                    parts = map(lambda x: x.strip(), line.split("|"))

                    # Unpack line
                    try:
                        test_name, ntup, tsamples, psamples, p_value, assessment = parts
                    except ValueError:
                        continue

                    # Check if test is not ignored
                    if test_name in DISABLED_TESTS:
                        continue

                    # Count results
                    total += 1

                    if assessment == "FAILED":
                        failed += 1
                    elif assessment == "PASSED":
                        passed += 1
                    elif assessment == "WEAK":
                        weak += 1

        # Read Ent file
        with open(filename_ent, "r") as fp:
            lines = []

            for line in fp:
                lines.append(line.strip())

            ent = lines[1].split(",", 1)[1]

        # Read configuration file
        sys.stdout.write("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," % (test_id, file_size, "", ent, "", "", "", "", "", total, passed, failed, weak, ""))

        with open(filename_conf, "r") as fp:
            configuration = eval(fp.read())

            for key, value in PROPERTIES.iteritems():
                sys.stdout.write(value[1](configuration[key]))
                sys.stdout.write(",")

        sys.stdout.write("\n")

    # Done
    return 0

# E.g. `python main.py input'
if __name__ == "__main__":
    sys.exit(main(sys.argv))
