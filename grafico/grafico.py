import matplotlib.pyplot as plt
import pandas as pd

file = pd.read_csv('grafico.csv')

plt.plot(file['TE'])
plt.plot(file['TI'])
plt.plot(file['TR'])

plt.ylabel('temperatura')
plt.xlabel('tempo')
plt.show()

pd.close()