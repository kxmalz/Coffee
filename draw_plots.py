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

k_4_50 = []
k_5_00 = []
k_5_25 = []
doses_4_50 = []
doses_5_00 = []
doses_5_25 = []

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

        
        # Finding point for which mass > 5 g (or other treshold)
        mass_point = 0
        for i in range(len(df[1])):
        	if df.iloc[i:i+20][1].mean() > 5.0:
        		mass_point = i
        		break
        
        time_trimmed = df.iloc[mass_point : (len(df[0]) - 30)][0]
        mass_trimmed = df.iloc[mass_point : (len(df[0]) - 30)][1]
        
        fit = np.polyfit(time_trimmed, mass_trimmed, 1)
        
        dp = df.iloc[mass_point : (len(df[0]) - 30)][3].mean()
        
        if grind == 5.0:
	        doses_5_00.append(dose)
        	k_5_00.append(fit[0] / dp)
        elif grind == 5.25:
        	doses_5_25.append(dose)
        	k_5_25.append(fit[0] / dp)
        elif grind == 4.50:
        	doses_4_50.append(dose)
        	k_4_50.append(fit[0] / dp)

        fig, ax1 = plt.subplots(2, figsize=(10,10))
        plt.style.use('seaborn-dark-palette')

        df[0]=df[0]/1000

        ax1[1].set_xlabel('time [s]')
        ax1[1].set_ylabel('mass [g]', )
        
        ax1[1].plot(df[0], df[1], color='red',label='Cup')
        ax1[1].plot(df[0], -df[2], color='yellow',label='Water container')
        ax1[1].plot(df[0], -df[1]-df[2], color = 'green', label = 'Difference')
        
        ax1[1].plot(df[0], 1e3 * fit[0] * df[0] + fit[1], color = 'black', linewidth = 3, alpha = 0.3, label = 'Fit')

        color = 'tab:blue'
        ax1[0].set_ylabel('hPa') # we already handled the x-label with ax1
        ax1[0].plot(df[0], df[3], color='#DBC506',label='Pressure')
        ax1[0].grid()
        plt.grid()
        ax1[1].set_ylim(-10,100)
        ax1[1].set_xlim(0,35)
        ax1[0].set_xlim(0,35)
        ax1[1].legend(loc = 2)
        ax1[0].legend(loc = 4)
        plt.title('Grind: ' + str(grind) + '\tDose: ' + str(dose) + ' g\t$Q$: ' 
        	+ str(round(1e3 * fit[0], 2)) + ' g/s\t$\Delta p:$ ' + str(round(dp, 2))
        	+ ' hPa\tk: ' + str(round(1e5 * fit[0] / dp, 2)) + ' * $10^-5$ g / s * hPa')
        plt.savefig('Plots/'+str(entry.name[:len(entry.name) - 4])+'.png')

plt.figure(50)

plt.plot(doses_4_50, k_4_50, 'go', label = 'grind 4.50')
plt.plot(doses_5_00, k_5_00, 'bo', label = 'grind 5.00')
plt.plot(doses_5_25, k_5_25, 'ro', label = 'grind 5.25')

plt.xlabel('Dose [g]')
plt.ylabel('$k = Q/\Delta p$ [g/s * hPa]')

plt.ylim(0.0, 0.000005)
plt.legend(loc = 2)
plt.title('Darcy coefficient')
plt.savefig('Plots/k.png')

