import os
import subprocess

ci = os.getenv("CI").lower() == "true"

def exec(command):
    return subprocess.run(command.split())


if ci:
    print("[ci]: building project\n")
    exec("pio run -e main")
    exit()

print("[local]: testing project\n")

ret = exec("pio test -e test")

if ret.returncode != 0:
    print("testing failed\n")
    exit()

print("[local]: uploading project\n")

exec("pio run -t upload -e main")
