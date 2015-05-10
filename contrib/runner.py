import itertools
import serial
import time
import sys

# Number of bytes per test to collect
COUNT = 1024 * 128  # 256kb

# Properties to vary
PROPERTIES = {
    "sampleReset": [0, 1],
    "sampleBucket": [1, 64, 128],
    "sampleMultiple": [1, 128],
    "delayHold": [0, 100, 1000],
    "delaySkip": [0, 100],
    "delayRound": [0],
    "sampleMask": [0b00011111, 0b00001111],
    "sampleShift": [0],
    "sampleTake": [5, 4],
    "mode": [1, 2],
    "extractor": [1, 2, 3],
    "generator": [1],
    "output": [1, 2],
    "outputMode": [1],
    "outputBucket": [1]
}


def main():
    # Determine all combinations
    names = sorted(PROPERTIES)
    combinations = [
        [{k: v} for k, v in zip(names, product)]
        for product in itertools.product(*(PROPERTIES[name] for name in names))
    ]

    # Open serial device
    device = serial.Serial(sys.argv[1], baud=115200, dsrdtr=True)

    # Start iterating
    sys.stdout.write("In total, %d to execute\n" % len(combinations))

    for index, combination in enumerate(combinations):
        data = bytearray()

        # Info
        sys.stdout.write(
            "Executing test %d/%d\n", index + 1, len(combinations))

        # Reset Arduino
        device.setDTR(True)
        time.sleep(0.1)
        device.setDTR(False)

        # Set the settings
        for key, value in combination:
            device.write("%s %s\n" % (key, value))

        # Start data collection
        device.write("run\n")
        device.flushInput()

        while len(data) < COUNT:
            data.append(device.read(1))

        with open("test_%d.bin" % index, "wb") as fp:
            fp.write(data)


# E.g. `python runner.py /dev/ttyACM0`
if __name__ == "__main__":
    sys.exit(main())
