#!/usr/bin/python3

import subprocess


def command(cmd: str) -> int:
    ret = subprocess.run(cmd, shell=True)
    return ret.returncode

def main() -> None:

    ret = command("idf.py build")

    if ret == 0:

        ret = command("idf.py flash")

        if ret == 0:
            ret = command("idf.py monitor")
        else:
            print("\033[31mFalhar ao gravar código na flash!")

    else:
        print("\033[31mFalha ao compilar código!")


if __name__ == "__main__":
    main()