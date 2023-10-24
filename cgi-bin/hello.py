import sys
import os

if __name__ == "__main__":
    for line in sys.stdin:
        print(line)
    print("Hello from python", file=sys.stderr)
    # while True:
    #     pass
    exit(0)