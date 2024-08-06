
# Use this tool to find a CRC when some of your data might be corrupted

from subprocess import check_output
import itertools
import glob
import sys

if len(sys.argv) != 3:
    print("Usage: python find.py <glob> <crc width>")
    print("Example: python find.py ./mydataset/s*.bin 32")
else:
    files = glob.glob(sys.argv[1])

    for subset in itertools.combinations(files, 4):

        command_str = "reveng.exe -s -w %s -f %s" % (sys.argv[2], " ".join(subset))
        print(command_str)
        try:
            print(check_output(command_str, shell=True).decode("utf-8"))
        except:
            pass