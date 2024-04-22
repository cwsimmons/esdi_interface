from subprocess import check_output
import itertools

files = [("s%d.bin" % x) for x in [1, 2, 3, 4, 5, 6, 7, 8, 9]]

for subset in itertools.combinations(files, 4):

    command_str = "reveng.exe -s -w 32 -f %s" % (" ".join(subset))
    print(command_str)
    try:
        print(check_output(command_str, shell=True).decode("utf-8"))
    except:
        pass