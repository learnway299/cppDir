#!/usr/bin/env python3
"""
批量修改题目文件，将main函数替换为namespace包装的测试代码
"""
import os
import re

# 需要处理的文件列表
files_to_process = [
    # 03-tree
    r"interview\coding\03-tree\tree_properties.cpp",
    r"interview\coding\03-tree\bst_operations.cpp",
    r"interview\coding\03-tree\lca.cpp",
    r"interview\coding\03-tree\path_problems.cpp",
    r"interview\coding\03-tree\construct_tree.cpp",
    r"interview\coding\03-tree\serialize_tree.cpp",
    # 04-dp
    r"interview\coding\04-dp\linear_dp.cpp",
    r"interview\coding\04-dp\knapsack.cpp",
    r"interview\coding\04-dp\subsequence_dp.cpp",
    r"interview\coding\04-dp\coin_change.cpp",
    r"interview\coding\04-dp\interval_dp.cpp",
]

def get_namespace_name(filename):
    """从文件名生成namespace名称"""
    base = os.path.basename(filename)
    name = base.replace('.cpp', '')
    # 保持下划线不变
    return name

def process_file(filepath):
    """处理单个文件"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        # 查找测试代码标记
        test_marker = "// ==================== 测试代码 ===================="
        if test_marker not in content:
            print(f"跳过 {filepath}: 未找到测试代码标记")
            return False

        # 分割内容
        parts = content.split(test_marker)
        if len(parts) != 2:
            print(f"跳过 {filepath}: 测试代码标记格式异常")
            return False

        before_test = parts[0]
        test_section = parts[1]

        # 查找int main()并删除
        main_pattern = r'\nint main\(\) \{[^}]*?return 0;\n\}\s*$'
        main_match = re.search(main_pattern, test_section, re.DOTALL)

        if not main_match:
            print(f"跳过 {filepath}: 未找到main函数")
            return False

        # 提取main函数内容
        main_content = main_match.group(0)

        # 提取测试代码部分(在main之前)
        test_code = test_section[:main_match.start()]

        # 提取main函数体内的内容(去掉main函数声明和return 0)
        main_body_match = re.search(r'int main\(\) \{\s*(.*?)\s*return 0;\s*\}', main_content, re.DOTALL)
        if not main_body_match:
            print(f"跳过 {filepath}: 无法解析main函数体")
            return False

        main_body = main_body_match.group(1).strip()

        # 生成namespace名称
        namespace_name = get_namespace_name(filepath)

        # 构建新的测试部分
        new_test_section = f"{test_marker}\n"
        new_test_section += test_code
        new_test_section += f"\nnamespace {namespace_name} {{\n\n"
        new_test_section += f"void runAllTests() {{\n"
        # 添加main函数原有内容(保持缩进)
        for line in main_body.split('\n'):
            new_test_section += f"    {line}\n"
        new_test_section += "}\n\n"
        new_test_section += f"}} // namespace {namespace_name}\n"

        # 组合新内容
        new_content = before_test + new_test_section

        # 写入文件
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)

        print(f"✓ 已处理: {filepath}")
        return True

    except Exception as e:
        print(f"✗ 处理失败 {filepath}: {e}")
        return False

def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))

    print("开始批量处理文件...\n")
    success_count = 0
    fail_count = 0

    for file_path in files_to_process:
        full_path = os.path.join(base_dir, file_path)
        if not os.path.exists(full_path):
            print(f"✗ 文件不存在: {full_path}")
            fail_count += 1
            continue

        if process_file(full_path):
            success_count += 1
        else:
            fail_count += 1

    print(f"\n处理完成:")
    print(f"  成功: {success_count}")
    print(f"  失败: {fail_count}")

if __name__ == "__main__":
    main()
