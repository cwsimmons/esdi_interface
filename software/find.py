from subprocess import check_output
import itertools
import glob

files = glob.glob("s_*.bin")

for subset in itertools.combinations(files, 4):

    command_str = "reveng.exe -s -w 32 -f %s" % (" ".join(subset))
    print(command_str)
    try:
        print(check_output(command_str, shell=True).decode("utf-8"))
    except:
        pass