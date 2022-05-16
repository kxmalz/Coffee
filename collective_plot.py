import serial
import time
import os
import csv
import matplotlib as mpl
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Font parameters
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = ["Times New Roman"] + plt.rcParams["font.serif"]
plt.rcParams['mathtext.fontset'] = 'cm'


plt.figure(0)

with os.scandir('Data/') as entries:
    for entry in entries:

        ignore_number = 0

        with open(entry) as file:
            content = file.readlines()
        for line in content:
            if line == "Pressure and weight measurements\n":
                break
            ignore_number = ignore_number + 1
        
        dose = file.name[len(file.name) - 16 : len(file.name) - 12]
        grind = file.name[len(file.name) - 8 : len(file.name) - 4]
        
        grind = float(grind[:1] + '.' + grind[2:])
        dose = float(dose[:2] + '.' + dose[3:])

        df = pd.read_csv(entry, skiprows=ignore_number+1, sep=",", header=None)

        mass_point = 0
        for i in range(len(df[1])):
        	if df.iloc[i:i+20][1].mean() > 5.0:
        		mass_point = i
        		break
        
        time_trimmed = df.iloc[mass_point : (len(df[0]) - 30)][0]
        mass_trimmed = df.iloc[mass_point : (len(df[0]) - 30)][1]
        
        fit = np.polyfit(time_trimmed, mass_trimmed, 1)
        
        dp = df.iloc[mass_point : (len(df[0]) - 30)][3].mean()

        plt.style.use('seaborn-dark-palette')

        plt.xlabel('time [s]')
        plt.ylabel('mass [g]', )
        
        #ax1[0].plot(df[0], df[1], color='red')
        #ax1[0].plot(df[0], -df[2], color='yellow')
        
        plt.plot(1e-3 * df[0], (-df[1]-df[2]).rolling(window = 5).mean(), label = 'Q: ' + str(round(1e3 * fit[0], 2)))
        
        #plt.plot(df[0], fit[0] * df[0] + fit[1], color = 'black', linewidth = 3, alpha = 0.3)

        color = 'tab:blue'
        plt.grid()
        plt.legend()
        plt.savefig('Plots/collective_plot.png')

