import sys
import serial # install 'pyserial' only!
import time
import argparse
import query_yes_no

import json

import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--experimenter","-e",type=str,help="Name of person making experiments. Use capital intials")
    parser.add_argument("--dose", "-d", type=int, help="Used dose [milligrams]")
    parser.add_argument("--grind", "-g", type=int, help="Used grind (25 for 2.5)")
    parser.add_argument("--message", "-m", type=str, help="Annotate measuerment with this message")

    args = parser.parse_args()

    print("Welcome. Please describe the measurement.")
    print("You can also use console args. Try python3 make_measurement.py --help.")

    experimenter = input("Enter initials (for example RW)              ") if args.experimenter is None else args.experimenter
    dose =     int(input("Enter dose in milligrams (for example 18000) ")) if args.dose is None else args.dose
    grind =    int(input("Enter grind (for example 25 for 2.5)         ")) if args.grind is None else args.grind
    message =      input("Enter message (e.g. 'notamp') or [ENTER]     ") if args.message is None else args.message

    moment = time.strftime("%Y-%m-%d_%H-%M-%S", time.localtime())

    fname = f"measurement_e={experimenter}_d={dose}_g={grind}_m={message}_t={moment}.csv"

    print("")
    print(fname)
    print("")

    if not query_yes_no.query_yes_no("Does this filename look correct?"):
        print("Aborting.")
        sys.exit(1)

    print("Proceeding")

    with open('device_config.json', "r") as config_file:
         device_config = json.load(config_file)

    ser = serial.Serial(device_config["serial_port_path"], 9600, timeout=0)
    payload = b""
    buf = b""
    data = b""

    print("Press [CTRL+C] to stop recording and exit.")

try:
    with open("./data/" + fname, "w") as f:
        while True:
            payload = ser.readline()
            if payload != b"":
                buf += payload
                try:
                    if "\n" in buf.decode("utf8"):
                        print(f'{buf.decode("utf8")}', end="")
                        if "Flow" in buf.decode("utf8"):  # Detect Arduino reset
                            print("Resetting stream")
                            f.truncate(0)
                            f.seek(0)
                            data = b""
                            buf = b""
                        f.write(buf.decode("utf8").replace('\x00','')) # Remove null bytes
                        data += buf
                        buf = b""
                except UnicodeDecodeError as e:
                    print('Error in decoding buffer')
                    print(e)
                    print('Starting over')
                    buf = b""
                    payload = b""
                    data = b""

except KeyboardInterrupt:
    print("Recording complete.")


print(data.decode("utf8"))

# ============================= SHOW FIGURE ===========================

src = data.decode("utf8")

def line_to_list(line):
    return [float(x) for x in line.split(',')]
arr = np.array([line_to_list(line) for line in src.split('\n') if line != ''])

timestamps = arr[:,0]
weight_one = arr[:,1]
weight_two = arr[:,2]


(fig, massax)= plt.subplots(1, 1, sharex=True)
massax.plot(timestamps, weight_one)
massax.plot(timestamps, weight_two)
massax.set_ylabel('Mass [g]')


def on_keypress(key):
    if key == 'enter':
        plt.close()
fig.canvas.mpl_connect('key_press_event', lambda e: on_keypress(e.key))

print('Displaying figure. Press [ENTER] to close and exit.')
plt.savefig(f"./flow_plots/{fname[:-4]}.jpg")
plt.show()
