import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# df = pd.read_csv('datasaya_bersih.csv', header=None)
# plt.style.use('_mpl-gallery')

sample_data = pd.read_csv('output.csv')
fig, ax = plt.subplots(9, 1,sharex=True)

# plt.title("nilai pembacaan encoder")

ax[8].stairs(sample_data.p_8, linewidth=2.5)
ax[8].set_ylabel('p_8')
ax[8].grid(False)

ax[7].stairs(sample_data.p_7, linewidth=2.5)
ax[7].set_ylabel('p_7')
ax[7].grid(False)

ax[6].stairs(sample_data.p_6, linewidth=2.5)
ax[6].set_ylabel('p_6')
ax[6].grid(False)

ax[5].stairs(sample_data.p_5, linewidth=2.5)
ax[5].set_ylabel('p_5')
ax[5].grid(False)

ax[4].stairs(sample_data.p_4, linewidth=2.5)
ax[4].set_ylabel('p_4')
ax[4].grid(False)

ax[3].stairs(sample_data.p_3, linewidth=2.5)
ax[3].set_ylabel('p_3')
ax[3].grid(False)

ax[2].stairs(sample_data.p_2, linewidth=2.5)
ax[2].set_ylabel('p_2')
ax[2].grid(False)

ax[1].stairs(sample_data.p_1, linewidth=2.5)
ax[1].set_ylabel('p_1')
ax[1].grid(False)

ax[0].stairs(sample_data.p_0, linewidth=2.5)
ax[0].set_ylabel('p_0')
ax[0].grid(False)

# buat layout menjadi kecil
# plt.tight_layout()
fig.align_titles()
plt.xlabel("waktu")
# plt.ylabel("nilai biner")

# plt.plot(sample_data.index,sample_data.data_1)
plt.show()