def compare_files(file1_path, file2_path):
    try:
        # Check if files are the same by comparing line by line
        with open(file1_path, 'r', encoding='utf-8') as f1, open(file2_path, 'r', encoding='utf-8') as f2:
            # Use readlines() to read all lines into lists
            lines1 = f1.readlines()
            lines2 = f2.readlines()
            
            # First check if files have same number of lines
            if len(lines1) != len(lines2):
                return False, f"Files have different number of lines. File1: {len(lines1)} lines, File2: {len(lines2)} lines"
            
            # Compare each line
            for line_num, (line1, line2) in enumerate(zip(lines1, lines2), 1):
                if line1 != line2:
                    return False, f"Files differ at line {line_num}"
            
            return True, "Files are identical"
                
    except FileNotFoundError as e:
        return False, f"File not found: {str(e)}"
    except PermissionError:
        return False, "Permission denied to access one or both files"
    except Exception as e:
        return False, f"Error comparing files: {str(e)}"

# Here's how to test it:
def test_comparison():
    # Create test files
    with open('test1.txt', 'w', encoding='utf-8') as f1:
        f1.write("Line 1\nLine 2\nLine 3")
    
    with open('test2.txt', 'w', encoding='utf-8') as f2:
        f2.write("Line 1\nLine 2\nLine 3")
    
    with open('test3.txt', 'w', encoding='utf-8') as f3:
        f3.write("Line 1\nLine 2\nDifferent Line 3")
    
    # Test identical files
    result, message = compare_files('test1.txt', 'test2.txt')
    print("Test 1 (identical files):", message)
    
    # Test different files
    result, message = compare_files('test1.txt', 'test3.txt')
    print("Test 2 (different files):", message)
    
    # Test non-existent file
    result, message = compare_files('test1.txt', 'nonexistent.txt')
    print("Test 3 (missing file):", message)

# Run the test
if __name__ == "__main__":
    test_comparison()

result, message = compare_files('test.txt', 'testoutput.txt')
print(message)