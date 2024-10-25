import random
import string

# Define the file size in bytes (2 MB)
file_size = 2 * 1024 * 1024  # 2 MB = 2 * 1024 KB = 2 * 1024 * 1024 bytes

# Generate random text
random_text = ''.join(random.choices(string.ascii_letters + string.digits, k=file_size))

# Write the random text to a file
with open("test.txt", "w") as file:
    file.write(random_text)

print("2MB file generated as '2MB_random_text.txt'")