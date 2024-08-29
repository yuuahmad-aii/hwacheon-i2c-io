import pandas as pd

# Membaca data dari file CSV
df = pd.read_csv('datasaya.csv', header=None)

# Menghapus kolom 'time'
df = df.drop(columns=['time'])

# Menghapus baris yang memiliki nilai yang sama dengan baris sebelumnya
df_cleaned = df.loc[(df.shift() != df).any(axis=1)]

# Menyimpan hasil yang telah diformat ke file baru
df_cleaned.to_csv('datasaya_bersih.csv', index=False)

# df = pd.read_csv('datasaya_bersih.csv')
# df = df.drop(columns=['Unnamed: 4'])

# df_cleaned.to_csv('datasaya_bersih.csv', index=False)

print("Kolom 'time' telah dihapus, dan data telah dibersihkan serta disimpan sebagai datasaya_bersih.csv")
