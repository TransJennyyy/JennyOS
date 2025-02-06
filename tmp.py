import os

def count_lines(filename):
  with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
    return len(f.readlines())

extensions = {'.cpp','.c','.py','.asm'}

print(sum(count_lines(os.path.join(root, file)) for root, _, files in os.walk(os.getcwd()) for file in files if any(file.endswith(ext) for ext in extensions)))