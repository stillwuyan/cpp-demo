### 使用xmake构建工程
1. 创建命令：`xmake create headers-ut`
2. 编译命令
   ```
   cd headers-ut
   xmake
   ```
3. 执行命令：`xmake run`

### 查看doctest的宏的原理
1. 生成预处理文件
   ```
   gcc -E -I ../include/ src/main.cpp  > main.pre.cpp
   ```
