# socket学习

使用socket编写的一个http请求demo, 只实现了最基础的`GET`请求

## 编译

**目前只支持windows平台**

使用cmake进行编译

首先需要配置好`cmake`需要的环境变量

如何使用`vscode`可以按照相关插件进行自动配置

```bash
cmake build  #需要配置好build目录(交给插件完成)

# 进入build目录
cd build
# 编译
make
# 运行
http.exe http://www.baidu.com/
```

# LICENSE

[mit](LICENSE)
