'''把字符串写入二进制文件中'''
def write_strings_to_binary_file(strings, filename):
    # 打开文件，以二进制写入模式
    with open(filename, 'wb') as file:
        for string in strings:
            # 将字符串编码为字节，然后写入文件
            file.write(string.encode('utf-8'))
            file.write(b'\n')  # 以二进制形式写入换行符


strings = ['Border relations with Canada have never been better.','1 2 4 8 16 32','1 311','7 0'
           ,'9/>567','4 3 2 1 6 5']
filename = 'output.bin'
'''
phase_1:字符藏在地址里
phase_2:对输入的6个数循环着判断
phase_3:switch
phase_4:递归数学题（我投降）呜呜呜
phase_5:输入的字符作为索引去藏在地址的字符串中挨个取出需要的字母
phase_6:根据索引,串连一众全局变量的结点,并且要求每个索引指向的结点中存储的值要小于等于下一个结点（我投降）呜呜呜
sercet_phase:甚至看不懂答案,说是二叉树结构（我投降）呜呜呜
'''
# 调用函数
write_strings_to_binary_file(strings, filename)


