import os
import hashlib
from PIL import Image
from natsort import natsorted

def calculate_checksum(filename):
    with open(filename, 'rb') as f:
        data = f.read()
        return hashlib.sha1(data).hexdigest()

directory = 'test_volume'
files = os.listdir(directory)

files = natsorted(files)

for i in range(0, 172*2,2):
    if i == 0:
        continue
    hash1 = calculate_checksum(os.path.join(directory, files[i]))
    hash2 = calculate_checksum(os.path.join(directory, files[i+1]))

    if hash1 != hash2:
        print(os.path.join(directory, files[i]))
        print(os.path.join(directory, files[i+1]))
        print(f'Volume integrity compromised')
        break
