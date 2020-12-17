# socket学习

使用socket编写的一个http请求demo, 只实现了最基础的`GET`请求

## 关于Socket

 - [about socket](https://zh.wikipedia.org/wiki/Winsock)

> Windows Sockets API (WSA), 简短记为Winsock, 是Windows的TCP/IP网络编程接口（API）。其函数名称兼容于Berkeley套接字API。实际上，Winsock的实现库(winsock.dll)使用的是长名字。

> Winsock是一种能使Windows程序通过任意网络传输协议发送数据的API。Winsock中有几个只支持TCP/IP协议的函数（例如gethostbyaddr()），但是在Winsock 2中新增了所有这些函数的通用版本，以允许开发者使用其它的传输协议。

- [socket client demo](https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client)

## HTTP协议

 - [http](https://developer.mozilla.org/zh-CN/docs/Web/HTTP)

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
