#!/usr/bin/env python3
"""
Fix the malformed string literals in StudentService.h
"""
import re

file_path = r"c:\Users\User\Pictures\BSCS-24119-PROJECT-3\backend\StudentService.h"

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Fix lines 128-130 (0-indexed 127-129)
if len(lines) > 130:
    lines[127] = '                   << "\\"courseID\\":\\"" << sc.courseID << "\\","\r\n'
    lines[128] = '                   << "\\"courseName\\":\\"" << sc.courseName << "\\","\r\n'
    lines[129] = '                   << "\\"teacherName\\":\\"" << sc.teacherName << "\\","\r\n'

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(lines)

print("Fixed StudentService.h string literals")
