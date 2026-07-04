#!/usr/bin/env python3
"""
批量删除非solution文件中的空runTests()函数，避免链接时重复定义
"""
import os
import re
import glob

def remove_empty_runTests(filepath):
    """删除文件中的空runTests函数"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        # 匹配空的runTests函数，带不同的TODO注释
        patterns = [
            # 模式1: // 测试代码将在solution中实现
            r'\nvoid runTests\(\) \{\s*\n\s*// 测试代码将在solution中实现\s*\n\}\n?',
            # 模式2: // TODO: 添加测试用例
            r'\nvoid runTests\(\) \{\s*\n\s*// TODO: 添加测试用例\s*\n\}\n?',
            # 模式3: 其他空实现
            r'\nvoid runTests\(\) \{\s*\n\s*// TODO.*?\s*\n\}\n?',
        ]

        modified = False
        for pattern in patterns:
            new_content, count = re.subn(pattern, '\n', content)
            if count > 0:
                content = new_content
                modified = True

        if modified:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✓ 已修复: {filepath}")
            return True
        else:
            return False

    except Exception as e:
        print(f"✗ 处理失败 {filepath}: {e}")
        return False

def main():
    base_dir = r"C:\Users\asus\cpp\CLionProjects\cppDir\interview\coding"

    # 查找所有非solution的cpp文件
    all_cpp = glob.glob(os.path.join(base_dir, "**/*.cpp"), recursive=True)
    non_solution_files = [f for f in all_cpp if not f.endswith('_solution.cpp') and 'test_runner' not in f and 'Util' not in f]

    print(f"找到 {len(non_solution_files)} 个非solution文件\n")

    fixed_count = 0
    for filepath in sorted(non_solution_files):
        if remove_empty_runTests(filepath):
            fixed_count += 1

    print(f"\n完成！共修复 {fixed_count} 个文件")

if __name__ == "__main__":
    main()
