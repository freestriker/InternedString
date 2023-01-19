# InternedString

## 简介 Introduction

InternedString是一个轻量级的字符串可重用系统，原理与虚幻引擎中的FName一致，做了部分简化与修改。系统内的每个字符串只会在池中存储一次，内部只存储访问用的键，所以拷贝与比较负担很小。而在访问字符串时使用键在字符串池中查找访问。  
InternedString is a lightweight string reusability system, based on the same principle as FName in Unreal Engine, with some simplifications and modifications. Each string in the system is only stored once in the pool, and only the keys used to access it are stored internally, so the copy and comparison burden is minimal. When accessing a string, the key is used to find the access in the string pool.

## 性能比较 Performance Comparison
在msvc x64 release下，使用随机生成的长度从6~25的总共15200个字符进行构造、拷贝、map构造、map查找、unordered_map构造、unordered_map查找进行测试。结果显示构造略微快一点，拷贝提升巨大，map查找的提升大概在3~4倍左右，unordered_map查找的提升大概在7倍左右。  
Under msvc x64 release, we tested construction, copy, map construction, map lookup, unordered_map construction, and unordered_map lookup using a total of 15,200 randomly generated characters of length 6 to 25. The results show that the construction is slightly faster, the copy improvement is huge, the map lookup improvement is about 3~4 times, and the unordered_map lookup improvement is about 7 times.  
|  | construction | copy |  map construction | map lookup | unordered_map construction | unordered_map lookup |
| ------ | ------ | ------ | ------ | ------ | ------ | ------ |
| std::string | 0.036984s | 0.000476s |  0.004711s | 0.004573s | 0.002412s | 0.001065s |
| InternedString | 0.031286s | 1.3e-05s |  0.001655s | 0.001151s | 0.001087s | 0.000144s |

## 算法思路 Algorithm ideas
见<https://zhuanlan.zhihu.com/p/599751716>。  
see<https://zhuanlan.zhihu.com/p/599751716>.

## 如何使用 How to use
* 安装xmake。 <https://github.com/xmake-io/xmake/>  
* 安装vcpkg。 <https://github.com/microsoft/vcpkg>  
* 使用xmake project -k vsxmake 生成vs工程。
* 使用xmake install -o YOUR_INSTALL_DIR 安装导出。
* Install xmake. <https://github.com/xmake-io/xmake/>  
* Install vcpkg. <https://github.com/microsoft/vcpkg>  
* Use xmake project -k vsxmake to generate vs project.
* Use xmake install -o YOUR_INSTALL_DIR to install the export.

## 与虚幻引擎FName的不同 The difference with Unreal Engine FName
* 去除FName中额外解析存储形如“Test_123456”结尾数字部分的功能。
* 去除FName中EName相关0消耗创建预缓存字符串的功能（可能之后会加上）。
* 未使用SSE手动加载Cache Line以提高性能。
* 不支持宽字符串。
* 数据结构略有不同。
* 判断是否为空的方式不一样，FName使用的是预加载的“None”字符串的特殊映射值，InternedString使用了一个标志位。
* Remove the additional parsing of the numeric part of FName ending with "Test_123456".
* Removed the ability to create pre-cached strings with EName related 0 consumption in FName (may be added later).
* No manual loading of Cache Line using SSE to improve performance.
* Wide strings are not supported.
* Slightly different data structure.
* The way to determine if it is empty is different, FName uses a special mapped value of the preloaded "None" string, InternedString uses a flag bit.

## 更新计划 Update Plan
* 整理代码结构。
* 添加FName中EName相关0消耗创建预缓存字符串的功能。
* 尝试使用SSE加速。
* Tidy up the code structure.
* Add the ability to create pre-cached strings with EName related 0 consumption in FName.
* Try to use SSE acceleration.

