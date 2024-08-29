import pandas as pd

# Baca data dari file .csv
df = pd.read_csv('filtered_data.csv', header=None)

# Fungsi untuk mendeteksi perubahan nilai pada setiap baris dan menambahkan 0 di depan jika kurang dari 8 bit
def detect_changes_and_pad(df):
    # Inisialisasi list untuk menyimpan baris yang berubah
    changed_rows = []
    
    # Ambil baris pertama
    prev_row = [row.zfill(8) for row in df.iloc[0].astype(str).tolist()] # Tambahkan padding 0
    changed_rows.append(prev_row)  # Baris pertama selalu dimasukkan
    
    # Loop melalui setiap baris mulai dari baris kedua
    for i in range(1, len(df)):
        current_row = [row.zfill(8) for row in df.iloc[i].astype(str).tolist()] # Tambahkan padding 0
        if current_row != prev_row:
            changed_rows.append(current_row)
        prev_row = current_row
    
    return pd.DataFrame(changed_rows)

# Deteksi perubahan pada data dan tambahkan padding 0 jika kurang dari 8 bit
filtered_df = detect_changes_and_pad(df)

# Simpan data yang telah difilter ke file baru
filtered_df.to_csv('output.csv', index=False, header=False)

print("Data telah difilter, diubah menjadi 8 bit, dan disimpan ke 'output.csv'.")
