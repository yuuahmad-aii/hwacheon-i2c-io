import pandas as pd

# Membaca data dari file CSV
df = pd.read_csv('datasaya.csv')

# Menghapus kolom 'time'
df = df.drop(columns=['time'])

# Menghapus baris yang memiliki nilai yang sama dengan nilai sebelumnya pada kolom 'Graph 0'
df = df[df['Graph 0'].shift() != df['Graph 0']]

# Menyimpan data yang telah diolah ke dalam file CSV baru
df.to_csv('filtered_data.csv', index=False)

# Menampilkan hasil akhir
print(df)
