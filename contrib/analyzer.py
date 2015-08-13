from datetime import datetime

import sys
import os
import pipes
import multiprocessing
import subprocess
import formic
import time
import random

# Which DIEHARDER tests to include
TESTS = [0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 15, 16, 100, 101, 102]
#TESTS = [0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 15, 16, 17, 100, 101, 102, 201, 204, 205]

# Skip test if output file already exists
SKIP_IF_EXISTS = True

# Run DIEHARDER
RUN_DIEHARDER = True

# Run Ent
RUN_ENT = True

def main(argv):
    if len(argv) not in [2, 3]:
        sys.stdout.write("Syntax: %s <input_dir> [<cpus>]\n" % argv[0])
        return 1

    # Parse arguments
    input_dir = os.path.abspath(argv[1])

    try:
        cpus = int(argv[2])
    except (LookupError, ValueError):
        cpus = multiprocessing.cpu_count()

    # Find files
    file_set = formic.FileSet(include="**/*.bin", directory=input_dir)
    filenames = list(file_set)
    sys.stdout.write("Found %d files\n" % len(filenames))

    # Create a pool
    pool = multiprocessing.Pool(processes=cpus)

    # Wait for completion
    r = pool.map_async(run_job, filenames)
    r.wait()
    sys.stdout.write("Done!\n")

    # Done
    return 0

def run_job(filename):
    def _dieharder():
        output_file = filename.rsplit(".", 1)[0] + ".diehard"

        # Remove old file since we concatenate
        if os.path.exists(output_file):
            if SKIP_IF_EXISTS and os.stat(output_file).st_size > 0:
                sys.stdout.write("Skipping test for '%s'\n" % filename)
                return

            os.remove(output_file)

        pre_cmd = ["dieharder", "-f", pipes.quote(filename), "-g", "201"]
        post_cmd = ["-D", "default", "-D", "histogram", ">>", pipes.quote(output_file), "2>", "/dev/null"]

        # Run commands
        for test in TESTS:
            command = pre_cmd + ["-d", str(test)] + post_cmd
            subprocess.call(" ".join(command), shell=True)

    def _ent():
        output_file = filename.rsplit(".", 1)[0] + ".ent"

        # Remove old file since we concatenate
        if os.path.exists(output_file):
            if SKIP_IF_EXISTS and os.stat(output_file).st_size > 0:
                sys.stdout.write("Skipping test for '%s'\n" % filename)
                return

            os.remove(output_file)

        command = ["ent", "-t", pipes.quote(filename), ">", pipes.quote(output_file), "2>", "/dev/null"]
        subprocess.call(" ".join(command), shell=True)

    # Wait a few seconds to 'randomize' start times
    time.sleep(random.random())

    # Run commands
    sys.stdout.write("Job starting for file: %s\n" % filename)
    start = datetime.now()

    if RUN_DIEHARDER:
        _dieharder()

    if RUN_ENT:
        _ent()

    end = datetime.now()
    sys.stdout.write("Job time: %s\n" % (end - start))

# E.g. `python main.py ../Results/upload 4'
if __name__ == "__main__":
    sys.exit(main(sys.argv))
