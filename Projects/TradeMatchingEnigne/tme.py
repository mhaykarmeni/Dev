#!/usr/bin/env python3
import argparse
import subprocess
import sys
import os

def main():
    parser = argparse.ArgumentParser(
        description="Trade Matching Engine script"
    )

    # Define flags
    parser.add_argument("-n", "--num", type=int, required=True, help="Number of orders (integer)")
    parser.add_argument("-m", "--map", choices=["std_map", "btree_map"], required=True, help="Map type")
    parser.add_argument("-d", "--dbg", action="store_true", help="Enable debug mode")
    parser.add_argument("-g", "--gen",action="store_true", help="Enable input generation")
    parser.add_argument("-b", "--build", action="store_true", help="Force build (always run build.sh)")

    args = parser.parse_args()

    exec_path = "./TradeMatchingEngine"

    # Build logic
    if args.build:
        print("--- Forcing build... ---")
        ret = subprocess.run(["./build.sh"])
        if ret.returncode != 0:
            print("--- Build failed! ---")
            sys.exit(1)
    else:
        if os.path.isfile(exec_path) and os.access(exec_path, os.X_OK):
            print("--- Executable already exists → skipping build. ---")
        else:
            print("--- Executable not found → running build... ---")
            ret = subprocess.run(["./build.sh"])
            if ret.returncode != 0:
                print("--- Build failed! ---")
                sys.exit(1)

    # Ensure executable exists
    if not (os.path.isfile(exec_path) and os.access(exec_path, os.X_OK)):
        print(f"--- Error: executable {exec_path} not found after build. ---")
        sys.exit(1)

    if not args.gen:
        input_file = "tme_input.txt"
    if not os.path.exists(input_file):
        print(f"Error: '{input_file}' not found. Nothing to load for input. Exiting the program.")
        sys.exit(1)

    # Prepare command
    cmd = [
        exec_path,
        str(args.num),
        args.map,
        "1" if args.dbg else "0",
        "1" if args.gen else "0"
    ]

    # Run the executable
    print(f"--- Running: {' '.join(cmd)} ---")
    subprocess.run(cmd)

if __name__ == "__main__":
    main()
