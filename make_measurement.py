import sys
import serial
import time
import os
import argparse
import query_yes_no

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--experimenter","-e",type=str,help="Name of person making experiments. Use capital intials")
    parser.add_argument("--dose", "-d", type=int, help="Used dose [milligrams]")
    parser.add_argument("--grind", "-g", type=int, help="Used grind (25 for 2.5)")
    parser.add_argument("--message", "-m", type=str, help="Annotate measuerment with this message")

    args = parser.parse_args()

    print("Welcome. Please describe the measurement.")
    print("You can also use console args. Try python3 make_measurement --help.")

    experimenter = input("Enter initials (for example RW)              ") if args.experimenter is None else args.experimenter
    dose = int(input("Enter dose in milligrams (for example 18000) ")) if args.dose is None else args.dose
    grind = int(input("Enter grind (for example 25 for 2.5)         ")) if args.grind is None else args.grind
    message = input("Enter message (e.g. 'notamp') or press ENTER ") if args.message is None else args.message

    moment = time.strftime("%Y-%m-%d_%H-%M-%S", time.localtime())

    fname = f"measurement_e={experimenter}_d={dose}_g={grind}_m={message}_t={moment}.csv"

    print(fname)

    if not query_yes_no.query_yes_no("Does this filename look correct?"):
        print("Aborting.")
        sys.exit(1)

    print("Proceeding")
    ser = serial.Serial("/dev/ttyACM0", 9600, timeout=0)
    payload = b""
    buf = b""

    print("Press [CTRL+C] to stop recording and exit.")

try:
    with open("./Data/" + fname, "w") as f:
        while True:
            payload = ser.readline()
            if payload != b"":
                buf += payload
                if "\n" in buf.decode("utf8"):
                    print(f'{buf.decode("utf8")}', end="")
                    if "Pressure" in buf.decode("utf8"):  # Detect Arduino reset
                        f.truncate(0)
                        buf = b""
                    f.write(buf.decode("utf8"))
                    buf = b""

except KeyboardInterrupt:
    print("Recording complete.")
    sys.exit(0)
